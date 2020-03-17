#include "map.h"

Map Map::instance;

// TODO TEST CODE - REMOVE
#include "map/questionpanel.h"

const sf::Color Map::sampleMap(const sf::Vector2f &sample) {
    sf::Color color;
    // see if sample coords fall inside any of the objects
    for (const FloorObjectPtr &object : instance.floorObjects) {
        if (object->sampleColor(sample, color)) {
            // yes they do, return their color (objects are on top of the map)
            return color;
        }
    }
    // no objects detected, return map instead
    return sampleAsset(instance.assetCourse, sample);
}

const sf::Image Map::mode7(const sf::Vector2f &position, const float angle,
                           const float fovHalf, const float clipNear,
                           const float clipFar, const sf::Vector2u &size,
                           const bool perspective) {
    float fovMin = angle - fovHalf;
    float fovMax = angle + fovHalf;
    sf::Vector2f trigMin = sf::Vector2f(cosf(fovMin), sinf(fovMin));
    sf::Vector2f trigMax = sf::Vector2f(cosf(fovMax), sinf(fovMax));

    // 4 points that define the frustum
    sf::Vector2f far1 = position + trigMin * clipFar;
    sf::Vector2f far2 = position + trigMax * clipFar;
    sf::Vector2f near1 = position + trigMin * clipNear;
    sf::Vector2f near2 = position + trigMax * clipNear;

    // create image and sample pixels from original course
    sf::Image mapImage;
    float width = size.x;
    float mapHeight = size.y;
    mapImage.create(width, mapHeight);
    for (int y = 1; y < mapHeight; y++) {
        // interpolate sides of the frustum and build a line
        float sampleDepth = y / mapHeight;
        sf::Vector2f start, end;
        if (perspective) {
            start = near1 + (far1 - near1) / sampleDepth;
            end = near2 + (far2 - near2) / sampleDepth;
        } else {
            start = near1 + (far1 - near1) * (1.0f - sampleDepth);
            end = near2 + (far2 - near2) * (1.0f - sampleDepth);
        }

        for (int x = 0; x < width; x++) {
            // interpolate created line from asset or
            float sampleWidth = x / width;
            sf::Vector2f sample = start + (end - start) * sampleWidth;

            sf::Color color;
            if (sample.x >= 0.0f && sample.x <= 1.0f &&  // inside map
                sample.y >= 0.0f && sample.y <= 1.0f) {
                color = sampleMap(sample);
            } else {
                // edges are only a tile's size instead of standard map
                sample.x = fmodf(sample.x, EDGES_SIZE / (float)ASSETS_WIDTH);
                sample.y = fmodf(sample.y, EDGES_SIZE / (float)ASSETS_HEIGHT);
                if (sample.x < 0.0f)
                    sample.x += EDGES_SIZE / (float)ASSETS_WIDTH;
                if (sample.y < 0.0f)
                    sample.y += EDGES_SIZE / (float)ASSETS_HEIGHT;
                sample.x *= ASSETS_WIDTH / (float)EDGES_SIZE;
                sample.y *= ASSETS_HEIGHT / (float)EDGES_SIZE;
                color = sampleAsset(instance.assetEdges, sample);
            }
            mapImage.setPixel(x, y, color);
        }
    }

    return mapImage;
}

void Map::setGameWindow(const Game &game) {
    instance.gameWindow = &game.getWindow();
}

bool Map::loadCourse(const std::string &course) {
    // Check if files exist
    std::ifstream inCourse(course + ".png");
    std::ifstream inSkyBack(course + "_sky_back.png");
    std::ifstream inSkyFront(course + "_sky_front.png");
    std::ifstream inEdges(course + "_edge.png");
    std::ifstream inFile(course + ".txt");
    if (!inCourse.is_open() || !inSkyBack.is_open() || !inSkyFront.is_open() ||
        !inEdges.is_open() || !inFile.is_open()) {
        std::cerr << "ERROR: Can't find files for '" << course << "'"
                  << std::endl;
        return false;
    }

    // Load assets from the files
    instance.assetCourse.loadFromFile(course + ".png");
    instance.assetSkyBack.loadFromFile(course + "_sky_back.png");
    instance.assetSkyFront.loadFromFile(course + "_sky_front.png");
    instance.assetEdges.loadFromFile(course + "_edge.png");
    for (int y = 0; y < TILES_HEIGHT; y++) {
        char landChar;
        for (int x = 0; x < TILES_WIDTH; x++) {
            inFile >> landChar;
            instance.landTiles[y][x] = Land(landChar - '0');
        }
    }

    // Load meta pos
    float meta_x, meta_y, meta_w, meta_h;
    inFile >> meta_x >> meta_y >> meta_w >> meta_h;
    instance.goal =
        sf::FloatRect(meta_x / ASSETS_WIDTH, meta_y / ASSETS_HEIGHT,
                      meta_w / ASSETS_WIDTH, meta_h / ASSETS_HEIGHT);

    // Checkpoint zones
    float cp_x, cp_y, cp_w, cp_h;
    inFile >> instance.nCp;
    instance.checkpoints = std::list<sf::FloatRect>();
    for (int i = 0; i < instance.nCp; i++) {
        inFile >> cp_x >> cp_y >> cp_w >> cp_h;
        sf::FloatRect cp(cp_x / ASSETS_WIDTH, cp_y / ASSETS_HEIGHT,
                         cp_w / ASSETS_WIDTH, cp_h / ASSETS_HEIGHT);
        instance.checkpoints.push_front(cp);
    }

    // Load floor objects
    instance.floorObjects.empty();
    instance.floorObjects.push_back(
        FloorObjectPtr(new QuestionPanel(sf::Vector2f(32.0f, 32.0f))));

    // Generate minimap image
    sf::Vector2u windowSize = instance.gameWindow->getSize();
    float min = MINIMAP_POS_DISTANCE - 1.0f - 0.05f;
    float max = MINIMAP_POS_DISTANCE + 0.15f;
    instance.assetMinimap = instance.mode7(
        sf::Vector2f(0.5f, MINIMAP_POS_DISTANCE),
        3.0f * M_PI_2,     // position, angle
        MINIMAP_FOV_HALF,  // field of view
        min / cosf(MINIMAP_FOV_HALF),
        max / cosf(MINIMAP_FOV_HALF),  // frustrum limits
        sf::Vector2u(windowSize.x, windowSize.y * MINIMAP_HEIGHT_PCT),  // size
        false);  // no perspective
    return true;
}

void Map::updateFloor(const std::vector<DriverPtr> drivers) {
    for (const DriverPtr &driver : drivers) {
        for (const FloorObjectPtr &object : instance.floorObjects) {
            if (object->collidesWith(driver)) {
                object->interactWith(driver);
            }
        }
    }
}

void Map::skyTextures(const DriverPtr &player, sf::Texture &skyBack,
                      sf::Texture &skyFront) {
    // Scale window so its in the same scale as asset images
    sf::Vector2u windowSize = instance.gameWindow->getSize();
    float textureWidth = windowSize.x / SKY_SCALE;
    float textureHeight = windowSize.y * SKY_HEIGHT_PCT / SKY_SCALE;

    // The sky texture's top is cut by a bit
    sf::Vector2u skyBackSize = instance.assetSkyBack.getSize();
    skyBackSize.x /= 2;  // texture is tiled 2 times horizontally
    sf::Vector2u skyFrontSize = instance.assetSkyFront.getSize();
    skyFrontSize.x /= 2;
    float textureScale = textureHeight / (SKY_CUT_PCT * skyBackSize.y);
    textureHeight /= textureScale;
    textureWidth /= textureScale;
    int backCutPixels = skyBackSize.y - textureHeight;
    int frontCutPixels = skyFrontSize.y - textureHeight;

    // Calculate angle used for horizontal translations
    float modAngle = fmodf(player->posAngle, 2.0f * M_PI);
    if (modAngle < 0.0f) modAngle += 2.0f * M_PI;
    int sampleBackX = modAngle * skyBackSize.x / (2.0f * M_PI);
    // Different angle gives parallax effect
    float angleFront = fmodf(modAngle, M_PI);
    int sampleFrontX = angleFront * skyFrontSize.x / M_PI;

    // Create textures
    skyBack.create(textureWidth, skyBackSize.y - backCutPixels);
    skyFront.create(textureWidth, skyBackSize.y - frontCutPixels);
    skyBack.loadFromImage(  // load given rect from image
        instance.assetSkyBack,
        sf::IntRect(sampleBackX, backCutPixels, textureWidth,
                    skyBackSize.y - backCutPixels));
    skyFront.loadFromImage(
        instance.assetSkyFront,
        sf::IntRect(sampleFrontX, frontCutPixels, textureWidth,
                    skyBackSize.y - frontCutPixels));
}

void Map::circuitTexture(const DriverPtr &player, sf::Texture &circuitTexture) {
    sf::Vector2f cameraPosition;
    cameraPosition.x =
        player->position.x -
        cosf(player->posAngle) * (CAM_2_PLAYER_DST / ASSETS_WIDTH);
    cameraPosition.y =
        player->position.y -
        sinf(player->posAngle) * (CAM_2_PLAYER_DST / ASSETS_HEIGHT);

    sf::Vector2u windowSize = instance.gameWindow->getSize();
    sf::Vector2u circuitSize =
        sf::Vector2u(windowSize.x, windowSize.y * CIRCUIT_HEIGHT_PCT);
    sf::Image mapImage =
        instance.mode7(cameraPosition, player->posAngle, MODE7_FOV_HALF,
                       MODE7_CLIP_NEAR, MODE7_CLIP_FAR, circuitSize, true);

    circuitTexture.create(circuitSize.x, circuitSize.y);
    circuitTexture.loadFromImage(mapImage);
}

void Map::mapTexture(sf::Texture &mapTexture) {
    mapTexture.loadFromImage(instance.assetMinimap);
}

sf::Vector2f Map::mapCoordinates(sf::Vector2f &position) {
    sf::Vector2f bottom(MINIMAP_BOTTOM_X, MINIMAP_BOTTOM_Y);
    sf::Vector2f top(MINIMAP_TOP_X, MINIMAP_TOP_Y);

    sf::Vector2f middleLeft = top + (bottom - top) * position.y;
    sf::Vector2f middleRight(1.0f - middleLeft.x, middleLeft.y);
    return middleLeft + (middleRight - middleLeft) * position.x;
}

bool Map::mapToScreen(const DriverPtr &player, const sf::Vector2f &mapCoords,
                      sf::Vector2f &screenCoords) {
    sf::Vector2f cameraPosition;
    cameraPosition.x =
        player->position.x -
        cosf(player->posAngle) * (CAM_2_PLAYER_DST / ASSETS_WIDTH);
    cameraPosition.y =
        player->position.y -
        sinf(player->posAngle) * (CAM_2_PLAYER_DST / ASSETS_HEIGHT);

    // map coords relative to camera
    sf::Vector2f relPoint = mapCoords - cameraPosition;
    float relPointMod =
        sqrtf(relPoint.x * relPoint.x + relPoint.y * relPoint.y);
    // forward direction for player
    sf::Vector2f forward =
        sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle));
    // calculate cos using dot product
    float cosFov = cosf(MODE7_FOV_HALF);
    float sinFov = sqrtf(1.0f - cosFov * cosFov);
    float cos = (forward.x * relPoint.x + forward.y * relPoint.y) / relPointMod;
    // sign is important
    float sin = (forward.x * relPoint.y - forward.y * relPoint.x) / relPointMod;
    // projected to forward player axis
    float y = MODE7_CLIP_FAR * cosf(MODE7_FOV_HALF) / (relPointMod * cos);
    // percent of horizontal bar
    float x = (sin + sinFov) / (2.0f * sinFov);

    screenCoords = sf::Vector2f(x, y);
    return x >= 0.0f && x < 1.0f && y >= 0.0f && y <= 1.0f;
}
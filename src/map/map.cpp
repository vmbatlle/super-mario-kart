#include "map.h"

Map Map::instance;

const sf::Color Map::sampleMap(const sf::Vector2f &sample) {
    return sampleAsset(instance.assetObjects, sample);
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
                sample.x =
                    fmodf(sample.x, MAP_EDGES_SIZE / (float)MAP_ASSETS_WIDTH);
                sample.y =
                    fmodf(sample.y, MAP_EDGES_SIZE / (float)MAP_ASSETS_HEIGHT);
                if (sample.x < 0.0f)
                    sample.x += MAP_EDGES_SIZE / (float)MAP_ASSETS_WIDTH;
                if (sample.y < 0.0f)
                    sample.y += MAP_EDGES_SIZE / (float)MAP_ASSETS_HEIGHT;
                sample.x *= MAP_ASSETS_WIDTH / (float)MAP_EDGES_SIZE;
                sample.y *= MAP_ASSETS_HEIGHT / (float)MAP_EDGES_SIZE;
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
    std::ifstream inCourse(course + "/base.png");
    std::ifstream inSkyBack(course + "/sky_back.png");
    std::ifstream inSkyFront(course + "/sky_front.png");
    std::ifstream inEdges(course + "/edge.png");
    std::ifstream inObjFile(course + "/objects.txt");
    std::ifstream inFile(course + "/base.txt");
    if (!inCourse.is_open() || !inSkyBack.is_open() || !inSkyFront.is_open() ||
        !inEdges.is_open() || !inObjFile.is_open() || !inFile.is_open()) {
        std::cerr << "ERROR: Can't find files for '" << course << "'"
                  << std::endl;
        return false;
    }
    // Free resources
    inCourse.close();
    inSkyBack.close();
    inSkyFront.close();
    inEdges.close();

    // Load assets from the files
    instance.assetCourse.loadFromFile(course + "/base.png");
    instance.assetObjects.loadFromFile(course + "/base.png");
    instance.assetSkyBack.loadFromFile(course + "/sky_back.png");
    instance.assetSkyFront.loadFromFile(course + "/sky_front.png");
    instance.assetEdges.loadFromFile(course + "/edge.png");
    for (int y = 0; y < MAP_TILES_HEIGHT; y++) {
        char landChar;
        for (int x = 0; x < MAP_TILES_WIDTH; x++) {
            inFile >> landChar;
            instance.landTiles[y][x] = MapLand(landChar - '0');
        }
    }

    // Load meta pos
    float meta_x, meta_y, meta_w, meta_h;
    inFile >> meta_x >> meta_y >> meta_w >> meta_h;
    instance.goal =
        sf::FloatRect(meta_x / MAP_ASSETS_WIDTH, meta_y / MAP_ASSETS_HEIGHT,
                      meta_w / MAP_ASSETS_WIDTH, meta_h / MAP_ASSETS_HEIGHT);

    // Checkpoint zones
    float cp_x, cp_y, cp_w, cp_h;
    inFile >> instance.nCp;
    instance.checkpoints = std::list<sf::FloatRect>();
    for (int i = 0; i < instance.nCp; i++) {
        inFile >> cp_x >> cp_y >> cp_w >> cp_h;
        sf::FloatRect cp(cp_x / MAP_ASSETS_WIDTH, cp_y / MAP_ASSETS_HEIGHT,
                         cp_w / MAP_ASSETS_WIDTH, cp_h / MAP_ASSETS_HEIGHT);
        instance.checkpoints.push_front(cp);
    }

    // Load floor objects
    instance.floorObjects.clear();
    int numObjects;
    inObjFile >> numObjects;
    instance.floorObjects.resize(numObjects);
    for (int i = 0; i < numObjects; i++) {
        // Read parameters from file
        int typeId, orientId;
        float pixelX, pixelY, sizeX, sizeY;
        inObjFile >> typeId >> pixelX >> pixelY >> orientId >> sizeX >> sizeY;
        FloorObjectOrientation orientation = FloorObjectOrientation(orientId);
        // Generate floor object
        FloorObjectPtr ptr;
        sf::Vector2f pos(pixelX, pixelY);
        switch (FloorObjectType(typeId)) {
            case FloorObjectType::ZIPPER:
                ptr = FloorObjectPtr(new Zipper(pos, orientation));
                break;
            case FloorObjectType::QUESTION_PANEL:
                ptr = FloorObjectPtr(new QuestionPanel(pos, orientation));
                instance.specialFloorObjects.push_back(ptr);
                break;
            case FloorObjectType::OIL_SLICK:
                ptr = FloorObjectPtr(new OilSlick(pos, orientation));
                break;
            case FloorObjectType::COIN:
                ptr = FloorObjectPtr(new Coin(pos, orientation));
                instance.specialFloorObjects.push_back(ptr);
                break;
            case FloorObjectType::RAMP_HORIZONTAL:
                // TODO
                ptr = FloorObjectPtr(new Coin(pos, orientation));
                break;
            case FloorObjectType::RAMP_VERTICAL:
                // TODO
                ptr = FloorObjectPtr(new Coin(pos, orientation));
                break;
            default:
                std::cerr << "ERROR: Invalid floor object type (" << typeId
                          << ")" << std::endl;
                break;
        }
        // Add it to the map's objects
        instance.floorObjects[i] = ptr;
    }
    // Initialize objects
    for (const FloorObjectPtr &object : instance.floorObjects) {
        object->setState(FloorObjectState::INITIAL);
    }

    // Generate course with objects
    FloorObject::applyAllChanges();

    // Generate minimap image
    updateMinimap();

    // Load wall objects
    instance.wallObjects.clear();
    inObjFile >> numObjects;
    instance.wallObjects.resize(numObjects);
    for (int i = 0; i < numObjects; i++) {
        // Read parameters from file
        int typeId;
        float centerX, centerY;
        inObjFile >> typeId >> centerX >> centerY;
        // Generate wall object
        WallObjectPtr ptr;
        sf::Vector2f pos(centerX, centerY);
        switch (WallObjectType(typeId)) {
            case WallObjectType::NORMAL_THWOMP:
                ptr = WallObjectPtr(new Thwomp(pos, false));
                break;
            case WallObjectType::SUPER_THWOMP:
                ptr = WallObjectPtr(new Thwomp(pos, true));
                break;
            case WallObjectType::GREEN_PIPE:
                ptr = WallObjectPtr(new Pipe(pos, true));
                break;
            case WallObjectType::ORANGE_PIPE:
                ptr = WallObjectPtr(new Pipe(pos, false));
                break;
            default:
                std::cerr << "ERROR: Invalid wall object type (" << typeId
                          << ")" << std::endl;
                break;
        }
        // Add it to the map's objects
        instance.wallObjects[i] = ptr;
    }

    // now that all floor objects are in place, update AI
    AIGradientDescent::updateGradient(instance.landTiles, instance.goal);

    // Load music TODO CAMBIAR DE SITIO
    // if (!instance.music.openFromFile(course + "/music.ogg")) {
    //     std::cerr << "AAAAA";
    // }
    // instance.music.setPosition(0, 1, 10);  // change its 3D position
    // instance.music.setPitch(1);            // increase the pitch ( 1 =
    // default) instance.music.setVolume(10);          // reduce the volume
    // instance.music.setLoop(true);          // make it loop

    return true;
}

void Map::startCourse() {
    // instance.music.play();
}

// Special course-dependent AI variables
int Map::getCurrentMapAIFarVision() {
    return instance.aiFarVision;
}

void Map::collideWithSpecialFloorObject(const DriverPtr &driver) {
    for (const FloorObjectPtr &object : instance.specialFloorObjects) {
        if (object->collidesWith(driver)) {
            object->interactWith(driver);
        }
    }
}

void Map::registerWallObjects() {
    for (const WallObjectPtr &object : instance.wallObjects) {
        CollisionHashMap::registerObject(object);
    }
}

void Map::updateObjects(const sf::Time &deltaTime) {
    // TODO maybe update items here too?
    for (const WallObjectPtr &object : instance.wallObjects) {
        object->update(deltaTime);
    }
}

void Map::updateMinimap() {
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
}

void Map::updateAssetCourse(const sf::Image &newAsset,
                            const sf::Vector2f &topLeftPixels) {
    float left = topLeftPixels.x;
    float top = topLeftPixels.y;
    float width = newAsset.getSize().x;
    float height = newAsset.getSize().y;
    // reset map tile(s)
    instance.assetObjects.copy(instance.assetCourse, left, top,
                               sf::IntRect(left, top, width, height), false);
    // copy new asset into course map with alpha channel
    instance.assetObjects.copy(newAsset, left, top,
                               sf::IntRect(0, 0, width, height), true);
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
        cosf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_WIDTH);
    cameraPosition.y =
        player->position.y -
        sinf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_HEIGHT);

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
                      sf::Vector2f &screenCoords, float &z) {
    sf::Vector2f cameraPosition;
    cameraPosition.x =
        player->position.x -
        cosf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_WIDTH);
    cameraPosition.y =
        player->position.y -
        sinf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_HEIGHT);

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
    z = (forward.x * relPoint.x + forward.y * relPoint.y);
    float cos = z / relPointMod;
    // sign is important
    float sin = (forward.x * relPoint.y - forward.y * relPoint.x) / relPointMod;
    // projected to forward player axis
    float y = MODE7_CLIP_FAR * cosf(MODE7_FOV_HALF) / (relPointMod * cos);
    // percent of horizontal bar
    float x = (sin + sinFov) / (2.0f * sinFov);

    screenCoords = sf::Vector2f(x, y);
    return x >= -0.1f && x < 1.1f && y >= 0.0f && y <= 2.0f;
}

void Map::getWallDrawables(
    const sf::RenderTarget &window, const DriverPtr &player,
    std::vector<std::pair<float, sf::Sprite *>> &drawables) {
    sf::Vector2u windowSize = window.getSize();
    for (const WallObjectPtr &object : instance.wallObjects) {
        sf::Vector2f radius =
            sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            object->radius;
        sf::Vector2f screen;
        float z;
        if (Map::mapToScreen(player, object->position - radius, screen, z)) {
            sf::Sprite &sprite = object->getSprite();
            sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
            screen.x *= windowSize.x;
            screen.y *= windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
            screen.y += windowSize.y * Map::SKY_HEIGHT_PCT;
            float scale = 1.0f / (3.6f * logf(1.02f + 0.8f * z));
            screen.y -= object->height * scale;
            sprite.scale(scale, scale);
            sprite.setPosition(screen);
            drawables.push_back(std::make_pair(z, &sprite));
        }
    }
}

void Map::getDriverDrawables(
    const sf::RenderTarget &window, const DriverPtr &player,
    const std::vector<DriverPtr> &drivers,
    std::vector<std::pair<float, sf::Sprite *>> &drawables) {
    sf::Vector2u windowSize = window.getSize();
    for (const DriverPtr &object : drivers) {
        if (object == player) {
            continue;
        }
        sf::Vector2f radius =
            sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            object->radius;
        sf::Vector2f screen;
        float z;
        if (Map::mapToScreen(player, object->position - radius, screen, z)) {
            object->animator.setViewSprite(player->posAngle, object->posAngle);
            sf::Sprite &sprite = object->getSprite();
            //sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
            screen.x *= windowSize.x;
            screen.y *= windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
            screen.y += windowSize.y * Map::SKY_HEIGHT_PCT;
            float scale = 1.0f / (3.6f * logf(1.02f + 0.8f * z));
            screen.y -= object->height * scale;
            sprite.scale(scale, scale);
            sprite.setPosition(screen);
            drawables.push_back(std::make_pair(z, &sprite));
        }
    }
}

sf::Vector2f Map::getPlayerInitialPosition(int position) {
    // TODO: change to position read from file
    // Mario Circuit 2
    // sf::Vector2f posGoal(920.0f, 412.0f);
    // Donut Plains 1
    // sf::Vector2f posGoal(132.0f, 508.0f);
    // instance.aiFarVision = 16; // TODO this shouldnt be done here (read from file)
    // Rainbow Road
    sf::Vector2f posGoal(64.0f, 432.0f);
    instance.aiFarVision = 8; // TODO this shouldnt be done here (read from file)
    // Bowser Castle 1
    // sf::Vector2f posGoal(928.0f, 652.0f);
    // instance.aiFarVision = 12;
    // Ghost Valley 1
    // sf::Vector2f posGoal(968.0f, 572.0f);
    float deltaX = posGoal.x < MAP_ASSETS_WIDTH / 2.0
                       ? 16.0f * (2.0f * (position % 2) - 1.0f)
                       : 16.0f * (1.0f - 2.0f * (position % 2));
    float deltaY = 28.0f + 24.0f * (position - 1);
    sf::Vector2f posPlayer = posGoal + sf::Vector2f(deltaX, deltaY);

    // Center player in tile
    posPlayer.x = std::floor(posPlayer.x / 8.0) * 8.0;

    return posPlayer;
}
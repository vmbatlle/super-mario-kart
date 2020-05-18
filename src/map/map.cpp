#include "map.h"

// #define DEBUG_ITEMS  // print extra information about items

Map Map::instance;

const sf::Color Map::sampleMap(const sf::Vector2f &sample) {
    return sampleAsset(instance.assetObjects, sample);
}

const sf::Color Map::sampleMapColor(const sf::Vector2f &sample) {
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
    instance.course = course;

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

    // Load material types
    int materialNormalId, materialSlowId, materialOuterId;
    inObjFile >> materialNormalId >> materialSlowId >> materialOuterId;
    instance.materialNormal = LandMaterial(materialNormalId);
    instance.materialSlow = LandMaterial(materialSlowId);
    instance.materialOuter = LandMaterial(materialOuterId);

    // Load meta pos
    float meta_x, meta_y, meta_w, meta_h;
    inObjFile >> meta_x >> meta_y >> meta_w >> meta_h;
    instance.stretchedGoal =
        sf::FloatRect(meta_x / MAP_ASSETS_WIDTH, meta_y / MAP_ASSETS_HEIGHT,
                      meta_w / MAP_ASSETS_WIDTH, meta_h / MAP_ASSETS_HEIGHT);
    inObjFile >> meta_x >> meta_y >> meta_w >> meta_h;
    instance.centeredGoal =
        sf::FloatRect(meta_x / MAP_ASSETS_WIDTH, meta_y / MAP_ASSETS_HEIGHT,
                      meta_w / MAP_ASSETS_WIDTH, meta_h / MAP_ASSETS_HEIGHT);

    // Load AI options
    inObjFile >> instance.aiFarVision;

    // Load floor objects
    FloorObject::resetChanges();
    instance.floorObjects.clear();
    instance.specialFloorObjects.clear();
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
        sf::Vector2f size(sizeX, sizeY);
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
                ptr =
                    FloorObjectPtr(new RampHorizontal(pos, orientation, size));
                break;
            case FloorObjectType::RAMP_VERTICAL:
                ptr = FloorObjectPtr(new RampVertical(pos, orientation, size));
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
    instance.itemObjects.clear();
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

    // Static collision registering
    CollisionHashMap::resetStatic();
    CollisionHashMap::resetDynamic();
    for (const WallObjectPtr &wallObject : instance.wallObjects) {
        CollisionHashMap::registerStatic(wallObject);
    }

    return true;
}

std::string Map::getCourse() { return instance.course; }

// AI-specific loading (gradient)
void Map::loadAI() {
    AIGradientDescent::updateGradient(instance.landTiles,
                                      instance.stretchedGoal);
}

// Special course-dependent AI variables
int Map::getCurrentMapAIFarVision() { return instance.aiFarVision; }

// Add thrown item to be shown as wallobject
void Map::addItem(const ItemPtr &item) { instance.itemObjects.push_back(item); }

// Add said effect if the map's outer tiles are water
void Map::addEffectDrown(const sf::Vector2f &position, bool isPlayer) {
    if (instance.materialOuter == LandMaterial::WATER) {
        // water splash (no tint)
        if (isPlayer) Audio::play(SFX::CIRCUIT_PLAYER_FALL_WATER);
        Map::addItem(ItemPtr(new EffectDrown(position, sf::Color::White)));
    } else if (instance.materialOuter == LandMaterial::LAVA) {
        // lava splash (red tint)
        if (isPlayer) Audio::play(SFX::CIRCUIT_PLAYER_FALL_LAVA);
        Map::addItem(ItemPtr(new EffectDrown(position, sf::Color::Red)));
    } else {
        if (isPlayer) Audio::play(SFX::CIRCUIT_PLAYER_FALL);
    }
}

// Add said effect for an item (always)
void Map::addEffectBreak(Item *item) {
    Map::addItem(ItemPtr(new EffectBreak(item)));
}

// Add said effect in the position (always)
void Map::addEffectSparkles(const sf::Vector2f &position) {
    Map::addItem(ItemPtr(new EffectSparkles(position)));
}

// Add said effect for a specified player (always)
void Map::addEffectCoin(const Driver *driver, const int number,
                        const bool positive) {
    sf::Time delay = sf::Time::Zero;
    for (int i = 0; i < number; i++) {
        Map::addItem(ItemPtr(new EffectCoin(driver, delay, positive)));
        if (positive) {
            delay += sf::seconds(0.3f);
        } else {
            delay += sf::seconds(0.1f);
        }
    }
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
        CollisionHashMap::registerStatic(object);
    }
}

void Map::registerItemObjects() {
    for (const ItemPtr &object : instance.itemObjects) {
        CollisionHashMap::registerDynamic(object);
    }
}

void Map::reactivateQuestionPanels() {
    for (FloorObjectPtr &object : instance.specialFloorObjects) {
        QuestionPanel *ptr = dynamic_cast<QuestionPanel *>(object.get());
        if (ptr) {
            ptr->setState(FloorObjectState::ACTIVE);
        }
    }
}

void Map::updateObjects(const sf::Time &deltaTime) {
    for (const WallObjectPtr &object : instance.wallObjects) {
        object->update(deltaTime);
    }
#ifdef DEBUG_ITEMS
    std::cout << "---- Item list: " << std::endl;
#endif
    for (const ItemPtr &item : instance.itemObjects) {
        // this check shouldn't be here but there's a weird bug
        // where pointers go to nullptr after seemingly random stuff
        if (item) {
#ifdef DEBUG_ITEMS
            std::cout << item->string() << std::endl;
#endif
            item->update(deltaTime);
        } else {
#ifdef DEBUG_ITEMS
            std::cout << "nullptr" << std::endl;
#endif
        }
    }
    // remove all used items
    instance.itemObjects.erase(
        std::remove_if(instance.itemObjects.begin(), instance.itemObjects.end(),
                       [](const ItemPtr &item) { return !item || item->used; }),
        instance.itemObjects.end());
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

void Map::circuitTexture(const DriverPtr &player, sf::Texture &circuitTexture,
                         sf::Vector2u windowSize) {
    sf::Vector2f cameraPosition;
    cameraPosition.x =
        player->position.x -
        cosf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_WIDTH);
    cameraPosition.y =
        player->position.y -
        sinf(player->posAngle) * (CAM_2_PLAYER_DST / MAP_ASSETS_HEIGHT);

    if (windowSize.x == 0 || windowSize.y == 0) {
        windowSize = instance.gameWindow->getSize();
    }
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
    const float screenScale,
    std::vector<std::pair<float, sf::Sprite *>> &drawables) {
    sf::Vector2u windowSize = window.getSize();
    for (const WallObjectPtr &object : instance.wallObjects) {
        sf::Vector2f radius =
            sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            object->visualRadius;
        sf::Vector2f screen;
        float z;
        if (Map::mapToScreen(player, object->position - radius, screen, z)) {
            sf::Sprite &sprite = object->getSprite();
            sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
            screen.x *= windowSize.x;
            screen.y *= windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
            screen.y += windowSize.y * Map::SKY_HEIGHT_PCT;
            float scale = 1.0f / (3.6f * logf(1.02f + 0.8f * z));

            // add shadow on the floor
            if (object->height > 0.0f) {
                sf::Sprite &shadow = object->spriteShadow;
                float zShadow = z + 10000.0f;  // shadows behind everything else
                shadow.setScale(Map::CIRCUIT_HEIGHT_PCT,
                                Map::CIRCUIT_HEIGHT_PCT);
                shadow.scale(scale * screenScale,
                             scale * screenScale / fmaxf(1.0f, z * 5.0f));
                shadow.setPosition(screen);
                int alpha = std::fmaxf((40.0f - object->height) * 5.5f, 0.0f);
                sf::Color color(255, 255, 255, alpha);
                shadow.setColor(color);
                drawables.push_back(std::make_pair(zShadow, &shadow));
            }

            screen.y -= object->height * scale * screenScale;
            sprite.scale(scale * screenScale, scale * screenScale);
            sprite.setPosition(screen);
            drawables.push_back(std::make_pair(z, &sprite));
        }
    }
}

void Map::getItemDrawables(
    const sf::RenderTarget &window, const DriverPtr &player,
    const float screenScale,
    std::vector<std::pair<float, sf::Sprite *>> &drawables) {
    sf::Vector2u windowSize = window.getSize();
    for (const WallObjectPtr &object : instance.itemObjects) {
        sf::Vector2f radius =
            sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            object->visualRadius;
        sf::Vector2f screen;
        float z;
        if (Map::mapToScreen(player, object->position - radius, screen, z)) {
            sf::Sprite &sprite = object->getSprite();
            sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
            screen.x *= windowSize.x;
            screen.y *= windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
            screen.y += windowSize.y * Map::SKY_HEIGHT_PCT;
            float scale = 1.0f / (3.6f * logf(1.02f + 0.8f * z));
            // item drawables don't cast a shadow, this includes
            // all effects (effectdrown, effectbreak...) that we don't want
            // casting any shadows
            // bananas are the only exception who should cast a shadow,
            // poor bananas
            screen.y -= object->height * scale * screenScale;
            sprite.scale(scale * screenScale, scale * screenScale);
            sprite.setPosition(screen);
            drawables.push_back(std::make_pair(z, &sprite));
        }
    }
}

void Map::getDriverDrawables(
    const sf::RenderTarget &window, const DriverPtr &player,
    const DriverArray &drivers, const float screenScale,
    std::vector<std::pair<float, sf::Sprite *>> &drawables) {
    sf::Vector2u windowSize = window.getSize();
    for (const DriverPtr &object : drivers) {
        if (object == player || !object->isVisible()) {
            continue;
        }
        sf::Vector2f radius =
            sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            object->visualRadius;
        sf::Vector2f screen;
        float z;
        if (Map::mapToScreen(player, object->position - radius, screen, z)) {
            float driverAngle = player->posAngle - object->posAngle;
            object->animator.setViewSprite(driverAngle);
            sf::Sprite &sprite = object->getSprite();
            screen.x *= windowSize.x;
            screen.y *= windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
            screen.y += windowSize.y * Map::SKY_HEIGHT_PCT;
            screen.y -= object->animator.spriteMovementSpeed;
            float scale = 1.0f / (3.6f * logf(1.02f + 0.8f * z));

            // add shadow on the floor
            if (object->height > 0.0f) {
                sf::Sprite &shadow = object->spriteShadow;
                float zShadow = z + 10000.0f;  // shadows behind everything else
                shadow.setScale(object->getSprite().getScale() *
                                Map::CIRCUIT_HEIGHT_PCT * 2.0f);
                shadow.scale(scale * screenScale,
                             scale * screenScale / fmaxf(1.0f, z * 5.0f));
                shadow.setPosition(screen);
                int alpha = std::fmaxf((40.0f - object->height) * 5.5f, 0.0f);
                sf::Color color(255, 255, 255, alpha);
                shadow.setColor(color);
                drawables.push_back(std::make_pair(zShadow, &shadow));
            }

            screen.y -= object->height * scale * screenScale;
            sprite.scale(scale * screenScale, scale * screenScale);
            sprite.setPosition(screen);
            drawables.push_back(std::make_pair(z, &sprite));
        }
    }
}

sf::Vector2f Map::getPlayerInitialPosition(int position) {
    sf::Vector2f posGoal(
        (instance.centeredGoal.left + instance.centeredGoal.width / 2.0) *
            MAP_ASSETS_WIDTH,
        (instance.centeredGoal.top + instance.centeredGoal.height) *
            MAP_ASSETS_HEIGHT);

    float deltaX = posGoal.x < MAP_ASSETS_WIDTH / 2.0
                       ? 16.0f * (2.0f * (position % 2) - 1.0f)
                       : 16.0f * (1.0f - 2.0f * (position % 2));
    float deltaY = 28.0f + 24.0f * (position - 1);
    sf::Vector2f posPlayer = posGoal + sf::Vector2f(deltaX, deltaY);

    // Center player in tile
    posPlayer.x = std::floor(posPlayer.x / 8.0) * 8.0;

    return posPlayer;
}
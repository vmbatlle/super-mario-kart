#include "map.h"

Map Map::instance;

bool Map::loadCourse(const std::string &course) {
    // Check if files exist
    std::ifstream inCourse(course + ".png");
    std::ifstream inEdges(course + "_edge.png");
    std::ifstream inFile(course + ".txt");
    if (!inCourse.is_open() || !inEdges.is_open() || !inFile.is_open()) {
        std::cerr << "ERROR: Can't find files for '" << course << "'"
                  << std::endl;
        return false;
    }

    // Load assets from the files
    instance.assetCourse.loadFromFile(course + ".png");
    instance.assetEdges.loadFromFile(course + "_edge.png");
    for (int y = 0; y < TILES_HEIGHT; y++) {
        char landChar;
        for (int x = 0; x < TILES_WIDTH; x++) {
            inFile >> landChar;
            instance.landTiles[y][x] = Land(landChar - '0');
        }
    } 
    //Load meta pos
    float meta_x, meta_y, meta_w, meta_h;
    inFile >> meta_x >> meta_y >> meta_w >> meta_h;
    instance.meta = sf::FloatRect(meta_x/ASSETS_WIDTH, meta_y/ASSETS_HEIGHT, 
                                  meta_w/ASSETS_WIDTH, meta_h/ASSETS_HEIGHT);

    //Checkpoint zones
    float cp_x, cp_y, cp_w, cp_h;
    inFile >> instance.nCp;
    instance.checkpoints = std::list<sf::FloatRect>();
    for (int i = 0; i < instance.nCp; i++) {
        inFile >> cp_x >> cp_y >> cp_w >> cp_h;
        sf::FloatRect cp(cp_x/ASSETS_WIDTH, cp_y/ASSETS_HEIGHT, 
                         cp_w/ASSETS_WIDTH, cp_h/ASSETS_HEIGHT);
        instance.checkpoints.push_front(cp);
    }

    return true;
}

void Map::setGameWindow(const Game &game) {
    instance.gameWindow = &game.getWindow();
}

void Map::drawMap(const sf::Vector2f &playerPosition, const float playerAngle,
                  sf::RenderTarget &window) {
    sf::Vector2f cameraPosition;
    cameraPosition.x = playerPosition.x -
                       cosf(playerAngle) * (CAM_2_PLAYER_DST / ASSETS_WIDTH);
    cameraPosition.y = playerPosition.y -
                       sinf(playerAngle) * (CAM_2_PLAYER_DST / ASSETS_HEIGHT);

    float fovMin = playerAngle - MODE7_FOV_HALF;
    float fovMax = playerAngle + MODE7_FOV_HALF;
    sf::Vector2f trigMin = sf::Vector2f(cosf(fovMin), sinf(fovMin));
    sf::Vector2f trigMax = sf::Vector2f(cosf(fovMax), sinf(fovMax));

    // 4 points that define the frustum
    sf::Vector2f far1 = cameraPosition + trigMin * MODE7_CLIP_FAR;
    sf::Vector2f far2 = cameraPosition + trigMax * MODE7_CLIP_FAR;
    sf::Vector2f near1 = cameraPosition + trigMax * MODE7_CLIP_NEAR;
    sf::Vector2f near2 = cameraPosition + trigMax * MODE7_CLIP_NEAR;

    // create image and sample pixels from original course
    sf::Vector2u windowSize = instance.gameWindow->getSize();
    sf::Image mapImage;
    float width = windowSize.x;
    float halfHeight = windowSize.y * MODE7_HEIGHT_PCT;
    mapImage.create(windowSize.x, halfHeight);
    for (int y = 1; y < halfHeight; y++) {
        // interpolate sides of the frustum and build a line
        float sampleDepth = y / halfHeight;
        sf::Vector2f start = near1 + (far1 - near1) / sampleDepth;
        sf::Vector2f end = near2 + (far2 - near2) / sampleDepth;

        for (int x = 0; x < width; x++) {
            // interpolate created line from asset or
            float sampleWidth = x / width;
            sf::Vector2f sample = start + (end - start) * sampleWidth;

            sf::Color color;
            if (sample.x >= 0.0f && sample.x <= 1.0f &&  // inside map
                sample.y >= 0.0f && sample.y <= 1.0f) {
                color = sampleAsset(instance.assetCourse, sample);
            } else {
                // edges are only a tile's size instead of standard map
                sample.x = fmodf(sample.x, EDGES_SIZE / (float)ASSETS_WIDTH);
                sample.y = fmodf(sample.y, EDGES_SIZE / (float)ASSETS_HEIGHT);
                if (sample.x < 0.0f) sample.x += EDGES_SIZE / (float)ASSETS_WIDTH;
                if (sample.y < 0.0f) sample.y += EDGES_SIZE / (float)ASSETS_HEIGHT;
                sample.x *= ASSETS_WIDTH / (float)EDGES_SIZE;
                sample.y *= ASSETS_HEIGHT / (float)EDGES_SIZE;
                color = sampleAsset(instance.assetEdges, sample);
            }
            mapImage.setPixel(x, y, color);
        }
    }

    // Draw on target window
    sf::Texture mapTexture;
    mapTexture.loadFromImage(mapImage);
    sf::Sprite mapSprite(mapTexture);
    mapSprite.setPosition(sf::Vector2f(0.0f, halfHeight));
    window.draw(mapSprite);
}
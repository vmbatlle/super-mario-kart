#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include "game.h"

class Map {
   private:
    // Singleton instance
    static Map instance;

    // Constants for ALL courses' map/tile data
    // Course image should be a width x height image
    static constexpr int ASSETS_WIDTH = 1024;
    static constexpr int ASSETS_HEIGHT = 1024;
    // Each image is divided in sizexsize regions
    static constexpr int TILE_SIZE = 8;
    // Map's outer border is made of a tile this big (sizexsize)
    static constexpr int EDGES_SIZE = 8;
    // Number of tiles in the whole map
    static constexpr int TILES_WIDTH = ASSETS_WIDTH / TILE_SIZE;
    static constexpr int TILES_HEIGHT = ASSETS_HEIGHT / TILE_SIZE;
    // Driver to Camera distance (in image pixels)
    static constexpr float CAM_2_PLAYER_DST = 56.0f;

    // Generate image as width * (height * height_pct) rectangle
    static constexpr float MODE7_HEIGHT_PCT = 0.5f;
    // Frustum configuration
    static constexpr float MODE7_FOV_HALF = 0.4f;
    static constexpr float MODE7_CLIP_NEAR = 0.0005f;
    static constexpr float MODE7_CLIP_FAR = 0.045f;

   public:
    enum class Land : uint8_t {
        TRACK,  // kart goes at normal speed
        BLOCK,  // kart collision (walls, etc.)
        SLOW,   // kart goes at half speed (grass, etc.)
        OUTER   // kart falls to the void
    };

   private:
    // Image with specified maps
    const sf::RenderWindow *gameWindow;
    sf::Image assetCourse, assetEdges;
    static inline sf::Color sampleAsset(const sf::Image &asset,
                                        const sf::Vector2f &sample) {
        sf::Vector2u size = asset.getSize();
        return asset.getPixel(sample.x * size.x, sample.y * size.y);
    }
    Land landTiles[TILES_HEIGHT][TILES_WIDTH];

   public:
    static void setGameWindow(const Game &game);

    // Get a point in the map
    static inline Land getLand(const sf::Vector2f &position) {
        // position in 0-1 range
        return instance.landTiles[int(position.y * TILES_HEIGHT)]
                                 [int(position.x * TILES_WIDTH)];
    }

    // Load all map resources so all interactions
    static bool loadCourse(const std::string &course);

    // Mode 7 perspective image with given position and angle
    static void drawMap(const sf::Vector2f &playerPosition,
                        const float playerAngle, sf::RenderTarget &window);
};
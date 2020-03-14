#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include "entities/driver.h"
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

    // Frustum configuration
    static constexpr float MODE7_FOV_HALF = 0.4f;
    static constexpr float MODE7_CLIP_NEAR = 0.0005f;
    static constexpr float MODE7_CLIP_FAR = 0.045f;

    static constexpr float MINIMAP_POS_DISTANCE = 6.5f;
    static constexpr float MINIMAP_FOV_HALF = M_PI / 32.0f;

   public:
    // Generate image as window width * (height * height_pct) rectangle
    static constexpr float SKY_SCALE = 2.0f;
    static constexpr float SKY_CUT_PCT = 20.0f / 32.0f;
    static constexpr float SKY_HEIGHT_PCT = 2.0f / 22.4f;
    static constexpr float CIRCUIT_HEIGHT_PCT = 9.2f / 22.4f;
    static constexpr float MINIMAP_HEIGHT_PCT = 11.2f / 22.4f;

    enum class Land : uint8_t {
        TRACK,  // kart goes at normal speed
        BLOCK,  // kart collision (walls, etc.)
        SLOW,   // kart goes at half speed (grass, etc.)
        OUTER   // kart falls to the void
    };

   private:
    // Image with specified maps
    const sf::RenderWindow *gameWindow;
    // Assets read directly from files
    sf::Image assetCourse, assetSkyBack, assetSkyFront, assetEdges;
    // Assets generated from other assets
    sf::Image assetMinimap;  // minimap generated from assetCourse
    static inline sf::Color sampleAsset(const sf::Image &asset,
                                        const sf::Vector2f &sample) {
        sf::Vector2u size = asset.getSize();
        return asset.getPixel(sample.x * size.x, sample.y * size.y);
    }
    Land landTiles[TILES_HEIGHT][TILES_WIDTH];

    const sf::Image mode7(const sf::Vector2f &position, const float angle,
                          const float fovHalf, const float clipNear,
                          const float clipFar, const sf::Vector2u &size,
                          const bool perspective);

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

    // Sky rectangle image with parallax effect
    static void skyTextures(const DriverPtr &player, sf::Texture &skyBack,
                            sf::Texture &skyFront);

    // Mode 7 perspective image with given position and angle
    static void circuitTexture(const DriverPtr &player,
                               sf::Texture &circuitTexture);

    // Mode 7 minimap on lower half of the screen
    static void mapTexture(sf::Texture &mapTexture);
};
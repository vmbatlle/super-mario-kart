#pragma once

// Constants for ALL courses' map/tile data
// Course image should be a width x height image
constexpr int MAP_ASSETS_WIDTH = 1024;
constexpr int MAP_ASSETS_HEIGHT = 1024;

// Each image is divided in sizexsize regions
constexpr int MAP_TILE_SIZE = 8;
// Map's outer border is made of a tile this big (sizexsize)
constexpr int MAP_EDGES_SIZE = 8;
// Number of tiles in the whole map
constexpr int MAP_TILES_WIDTH = MAP_ASSETS_WIDTH / MAP_TILE_SIZE;
constexpr int MAP_TILES_HEIGHT = MAP_ASSETS_HEIGHT / MAP_TILE_SIZE;

// # of laps drivers have to complete to finish
constexpr int NUM_LAPS_IN_CIRCUIT = 5;

// cup speed selection menu
enum class CCOption : unsigned int {
    CC50,
    CC100,
    CC150,
    __COUNT,
};

enum class MapLand : int {
    TRACK,            // kart goes at normal speed
    BLOCK,            // kart collision (walls, etc.)
    SLOW,             // kart goes at half speed (grass, etc.)
    OUTER,            // kart falls to the void
    RAMP,             // generic ramp
    OIL_SLICK,        // kart spins uncontrolled for a period of time
    RAMP_HORIZONTAL,  // kart jumps along axis Y
    RAMP_VERTICAL,    // kart jumps along axis X
    ZIPPER,           // kart goes faster for a period of time
    OTHER,            // driver activates another floorobject
    SPECIAL_13H,      // special case for mario circuit 2 gradient calculation
                      // teleport player 13 blocks horizontally
};

typedef std::array<std::array<MapLand, MAP_TILES_WIDTH>, MAP_TILES_HEIGHT>
    MapLandMatrix;

enum class LandMaterial : int {
    DIRT = 0,  // also includes sand
    GRASS = 1,
    // WOOD, // should be in donut plains 1 but it's a special case,
    //       // we dont care
    SPOOKY_WOOD = 2,  // ghost valley 1 variant of wood
    STONE = 3,        // also includes rainbow or asphalt
    WATER = 4,
    LAVA = 5,
    VOID = 6,
    RAINBOW = 7
};

enum class FloorObjectType : int {
    ZIPPER,
    QUESTION_PANEL,
    OIL_SLICK,
    COIN,
    RAMP_HORIZONTAL,
    RAMP_VERTICAL,
};

enum class FloorObjectOrientation : int { UP, RIGHT, DOWN, LEFT, __COUNT };
enum class FloorObjectState : int { INITIAL, NONE, ACTIVE, INACTIVE, __COUNT };
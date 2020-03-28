#pragma once

enum class MapLand : int {
    TRACK,            // kart goes at normal speed
    BLOCK,            // kart collision (walls, etc.)
    SLOW,             // kart goes at half speed (grass, etc.)
    OUTER,            // kart falls to the void
    OIL_SLICK,        // kart spins uncontrolled for a period of time
    RAMP_HORIZONTAL,  // kart jumps along axis Y
    RAMP_VERTICAL,    // kart jumps along axis X
    ZIPPER,           // kart goes faster for a period of time
    OTHER             // driver activates another floorobject
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
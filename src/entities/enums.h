#pragma once

enum class WallObjectType : int {
    NORMAL_THWOMP,
    SUPER_THWOMP,
    GREEN_PIPE,
    ORANGE_PIPE,
};

enum class DriverControlType : int {
    DISABLED,
    AI_GRADIENT,
    PLAYER,
};

enum class PowerUps : int {
    NONE,
    MUSHROOM,
    COIN,
    STAR,
    BANANA,
    GREEN_SHELL,
    RED_SHELL,
    THUNDER,
};
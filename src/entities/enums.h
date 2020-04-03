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
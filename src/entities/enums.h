#pragma once

#include <array>
#include "entities/vehicleproperties.h"

enum class WallObjectType : int {
    NORMAL_THWOMP,
    SUPER_THWOMP,
    GREEN_PIPE,
    ORANGE_PIPE,
};

// mario - peach - bowser - koopa
// luigi - yoshi - dk jr - toad
enum class MenuPlayer : int {
    MARIO,
    PEACH,
    BOWSER,
    KOOPA,
    LUIGI,
    YOSHI,
    DK,
    TOAD,
    __COUNT
};

typedef std::array<MenuPlayer, (int)MenuPlayer::__COUNT> PlayerArray;

const std::array<std::string, (int)MenuPlayer::__COUNT> DRIVER_ASSET_NAMES = {
    "assets/drivers/mario.png",  "assets/drivers/peach.png",
    "assets/drivers/bowser.png", "assets/drivers/koopa.png",
    "assets/drivers/luigi.png",  "assets/drivers/yoshi.png",
    "assets/drivers/dk.png",     "assets/drivers/toad.png"};
const std::array<const VehicleProperties *, (int)MenuPlayer::__COUNT>
    DRIVER_PROPERTIES = {
        &VehicleProperties::BALANCED, &VehicleProperties::ACCELERATION,
        &VehicleProperties::HEAVY,    &VehicleProperties::HANDLING,
        &VehicleProperties::BALANCED, &VehicleProperties::ACCELERATION,
        &VehicleProperties::HEAVY,    &VehicleProperties::HANDLING,
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
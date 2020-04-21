#pragma once

#include <array>
#include <memory>
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

class Driver;
typedef std::shared_ptr<Driver> DriverPtr;
typedef std::array<DriverPtr, (int)MenuPlayer::__COUNT> DriverArray;
typedef std::array<Driver *, (int)MenuPlayer::__COUNT> RaceRankingArray;
typedef std::array<std::pair<Driver *, int>, (int)MenuPlayer::__COUNT>
    GrandPrixRankingArray;

// taken from the original super mario kart
const std::array<std::string, (int)MenuPlayer::__COUNT> DRIVER_DISPLAY_NAMES = {
    "mario", "princess", "bowser",         "koopa troopa",
    "luigi", "yoshi",    "donkey kong jr", "toad"};
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

static constexpr const unsigned int BASIC_WIDTH = 256, BASIC_HEIGHT = 224;

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
    __COUNT
};
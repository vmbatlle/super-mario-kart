#pragma once

#define _USE_MATH_DEFINES

#include <array>
#include <memory>
#include <random>

#include "entities/vehicleproperties.h"
#include "map/enums.h"

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

static constexpr const float BASIC_WIDTH = 512.0f, BASIC_HEIGHT = 448.0f;

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

static std::random_device rd;
static std::mt19937 randGen(rd());

static const int FAR_VISIONS[(int)CCOption::__COUNT][(int)MenuPlayer::__COUNT] =
    {{0, 0, 0, 0, -1, 1, 1, 1},
     {0, 0, 1, 1, -1, -2, 2, 2},
     {1, 1, 1, -1, -1, -2, 2, 2}};

static const int ITEM_PROB_MODS[(int)CCOption::__COUNT][(
    int)MenuPlayer::__COUNT] = {{7, 7, 6, 6, 6, 6, 2, 1},
                                {4, 4, 4, 4, 3, 2, 2, 1},
                                {2, 1, 1, 1, 1, 1, 1, 1}};

static const unsigned int IMPEDIMENTS[(int)CCOption::__COUNT][(
    int)MenuPlayer::__COUNT] = {{128, 128, 96, 96, 96, 64, 64, 48},
                                {96, 64, 64, 48, 48, 48, 32, 32},
                                {64, 64, 64, 48, 48, 32, 32, 16}};
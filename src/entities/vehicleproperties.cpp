#include "vehicleproperties.h"

#include <iostream>
#include <utility>
// https://www.mariowiki.com/Super_Mario_Kart#Drivers

// in order: acceleration, handling, top speed, weight
VehicleProperties  // medium, medium, high, medium
    VehicleProperties::BALANCED(0.11f, 0.14f, 0.10200f, 1.0f, false);
VehicleProperties  // very high, low, medium, medium
    VehicleProperties::ACCELERATION(0.15f, 0.13f, 0.10175f, 1.0f, false);
VehicleProperties  // low, low, very high, high
    VehicleProperties::HEAVY(0.10f, 0.13f, 0.10250f, 2.0f, true);
VehicleProperties  // high, high, low, low
    VehicleProperties::HANDLING(0.12f, 0.15f, 0.10150f, 0.5f, false);

const VehicleProperties  // not even fair
    VehicleProperties::GODMODE(0.2f, 0.3f, 0.2f, 10.0f, false);

float VehicleProperties::PLAYER_CHARACTER_MULTIPLIER = 1.0f;
VehicleProperties VehicleProperties::PLAYER = BALANCED;

void VehicleProperties::setScaleFactor(const float scaleFactor,
                                       const float playerCharacterMultiplier) {
    BALANCED = std::move(
        VehicleProperties(0.11f * scaleFactor, 0.14f * scaleFactor / 1.5f,
                          0.10200f * scaleFactor, 1.0f, false));
    ACCELERATION = std::move(
        VehicleProperties(0.15f * scaleFactor, 0.13f * scaleFactor / 1.5f,
                          0.10175f * scaleFactor, 1.0f, false));
    HEAVY = std::move(VehicleProperties(0.10f * scaleFactor,
                                        0.13f * scaleFactor / 1.5f,
                                        0.10250f * scaleFactor, 2.0f, true));
    HANDLING = std::move(
        VehicleProperties(0.12f * scaleFactor, 0.15f * scaleFactor / 1.5f,
                          0.10150f * scaleFactor, 0.5f, false));

    PLAYER_CHARACTER_MULTIPLIER = playerCharacterMultiplier;
}

const VehicleProperties &VehicleProperties::makePlayer() const {
    PLAYER = std::move(VehicleProperties(
        motorAcceleration * PLAYER_CHARACTER_MULTIPLIER,
        turningAcceleration * PLAYER_CHARACTER_MULTIPLIER,
        maxNormalLinearSpeed * PLAYER_CHARACTER_MULTIPLIER, weight, convex));
    return PLAYER;
}
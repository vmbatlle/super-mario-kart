#include "vehicleproperties.h"

// https://www.mariowiki.com/Super_Mario_Kart#Drivers

const float scaleFactor = 1.0f;

// in order: acceleration, handling, top speed, weight
const VehicleProperties  // medium, medium, high, medium
    VehicleProperties::BALANCED(0.11f * scaleFactor, 0.14f * scaleFactor,
                                0.103f * scaleFactor, 1.0f, false);
const VehicleProperties  // very high, low, medium, medium
    VehicleProperties::ACCELERATION(0.15f * scaleFactor, 0.13f * scaleFactor,
                                    0.102f * scaleFactor, 1.0f, false);
const VehicleProperties  // low, low, very high, high
    VehicleProperties::HEAVY(0.10f * scaleFactor, 0.13f * scaleFactor,
                             0.104f * scaleFactor, 2.0f, true);
const VehicleProperties  // high, high, low, low
    VehicleProperties::HANDLING(0.12f * scaleFactor, 0.15f * scaleFactor,
                                0.100f * scaleFactor, 0.5f, false);

const VehicleProperties  // not even fair
    VehicleProperties::GODMODE(0.2f, 0.3f, 0.2f, 10.0f, false);
#include "vehicleproperties.h"

// https://www.mariowiki.com/Super_Mario_Kart#Drivers

// in order: acceleration, handling, top speed, weight
const VehicleProperties  // medium, medium, high, medium
    VehicleProperties::BALANCED(0.1f, 0.15f, 0.11f, 1.0f);
const VehicleProperties  // very high, low, medium, medium
    VehicleProperties::ACCELERATION(0.14f, 0.1f, 0.1f, 1.0f);
const VehicleProperties  // low, low, very high, high
    VehicleProperties::HEAVY(0.08f, 0.1f, 0.12f, 2.0f);
const VehicleProperties  // high, high, low, low
    VehicleProperties::HANDLING(0.12f, 0.2f, 0.09f, 0.5f);

const VehicleProperties  // not even fair
    VehicleProperties::GODMODE(0.2f, 0.3f, 0.2f, 10.0f);
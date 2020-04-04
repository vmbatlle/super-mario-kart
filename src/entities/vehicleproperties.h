#pragma once

class VehicleProperties {
   public:
    // mario/luigi: good overall
    static const VehicleProperties BALANCED;
    // peach/yoshi: accelerates fast
    static const VehicleProperties ACCELERATION;
    // bowser/dk jr.: more top speed, slower
    static const VehicleProperties HEAVY;
    // koopa/toad: better turns
    static const VehicleProperties HANDLING;
    // debug kart
    static const VehicleProperties GODMODE;

    static constexpr const float FRICTION_LINEAR_ACELERATION = -0.03f;
    static constexpr const float BREAK_ACELERATION = -0.2f;
    static constexpr const float SLOW_LAND_LINEAR_ACELERATION = -0.15f;

    const float motorAcceleration;    // acceleration

    const float maxNormalLinearSpeed;  // top speed
    const float maxSpeedUpLinearSpeed;
    const float maxSpeedDownLinearSpeed;

    const float turningAcceleration;  // handling
    const float maxTurningAngularSpeed;

    const float slowLandMaxLinearSpeed;

    const float weight;  // weight

   private:
    constexpr VehicleProperties(const float _motorAcceleration,
                                const float _turningAcceleration,
                                const float _maxNormalLinearSpeed,
                                const float _weight)
        : motorAcceleration(_motorAcceleration),
          maxNormalLinearSpeed(_maxNormalLinearSpeed),
          maxSpeedUpLinearSpeed(_maxNormalLinearSpeed * 2.0f),
          maxSpeedDownLinearSpeed(_maxNormalLinearSpeed * 0.5f),
          turningAcceleration(_turningAcceleration),
          maxTurningAngularSpeed(_turningAcceleration * 15.0f),
          slowLandMaxLinearSpeed(_maxNormalLinearSpeed * 0.5f),
          weight(_weight) {}
};
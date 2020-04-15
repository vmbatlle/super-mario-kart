#pragma once

class VehicleProperties {
   public:
    // mario/luigi: good overall
    static VehicleProperties BALANCED;
    // peach/yoshi: accelerates fast
    static VehicleProperties ACCELERATION;
    // bowser/dk jr.: more top speed, slower
    static VehicleProperties HEAVY;
    // koopa/toad: better turns
    static VehicleProperties HANDLING;
    // debug kart
    static const VehicleProperties GODMODE;

    static constexpr const float FRICTION_LINEAR_ACELERATION = -0.03f;
    static constexpr const float BREAK_ACELERATION = -0.2f;
    static constexpr const float SLOW_LAND_LINEAR_ACELERATION = -0.15f;

    float motorAcceleration;  // acceleration

    float maxNormalLinearSpeed;  // top speed
    float maxSpeedUpLinearSpeed;
    float maxSpeedDownLinearSpeed;

    float turningAcceleration;  // handling
    float maxTurningAngularSpeed;

    float slowLandMaxLinearSpeed;

    float weight;  // weight

    bool convex;  // speed convexity

    // Used for 50cc-100cc-150cc
    static void setScaleFactor(const float scaleFactor);

   private:
    constexpr VehicleProperties(const float _motorAcceleration,
                                const float _turningAcceleration,
                                const float _maxNormalLinearSpeed,
                                const float _weight, const bool _convex)
        : motorAcceleration(_motorAcceleration),
          maxNormalLinearSpeed(_maxNormalLinearSpeed),
          maxSpeedUpLinearSpeed(_maxNormalLinearSpeed * 2.0f),
          maxSpeedDownLinearSpeed(_maxNormalLinearSpeed * 0.5f),
          turningAcceleration(_turningAcceleration),
          maxTurningAngularSpeed(_turningAcceleration * 10.0f),
          slowLandMaxLinearSpeed(_maxNormalLinearSpeed * 0.5f),
          weight(_weight),
          convex(_convex) {}
};
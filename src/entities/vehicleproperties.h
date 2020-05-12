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

    static float PLAYER_CHARACTER_MULTIPLIER;
    static constexpr const float FRICTION_LINEAR_ACELERATION = -0.04f;
    static constexpr const float BREAK_ACELERATION = -0.2f;
    static constexpr const float SLOW_LAND_LINEAR_ACELERATION = -0.20f;

    // 1st place: x1 acceleration, 2nd place: x1.02, 3rd: x1.04 until 8th
    static constexpr const float POSITION_ACCELERATION_BONUS_PCT = 0.0085f;

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
    static void setScaleFactor(const float scaleFactor,
                               const float playerCharacterMultiplier);

    // return modified vehicleproperties with playercharactermultiplier applied
    const VehicleProperties &makePlayer() const;

   private:
    // temporal copy with playercharctermultiplier applied
    static VehicleProperties PLAYER;

    constexpr VehicleProperties(const float _motorAcceleration,
                                const float _turningAcceleration,
                                const float _maxNormalLinearSpeed,
                                const float _weight, const bool _convex)
        : motorAcceleration(_motorAcceleration),
          maxNormalLinearSpeed(_maxNormalLinearSpeed),
          maxSpeedUpLinearSpeed(_maxNormalLinearSpeed * 2.0f),
          maxSpeedDownLinearSpeed(_maxNormalLinearSpeed * 0.85f),
          turningAcceleration(_turningAcceleration),
          maxTurningAngularSpeed(_turningAcceleration * 10.0f),
          slowLandMaxLinearSpeed(_maxNormalLinearSpeed * 0.45f),
          weight(_weight),
          convex(_convex) {}
};
#pragma once

#include <SFML/Graphics.hpp>

#include "entities/wallobject.h"
#include "map/map.h"

class Thwomp : public WallObject {
   private:
    enum class State {
        UP,
        DOWN,
        GOING_UP,
        GOING_DOWN,
    };

    static constexpr const float MAX_HEIGHT = 20.0f;
    static constexpr const float STOMP_HEIGHT = 4.0f;
    static sf::Texture assetNormal, assetSuper;
    sf::Sprite sprite;
    State currentState;
    float currentTime;
    bool isSuper;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roiNormal,
                           const sf::IntRect &roiSuper);

    Thwomp(const sf::Vector2f &position, bool _isSuper);

    void update(const sf::Time &deltaTime) override;
    sf::Sprite &getSprite() { return sprite; }

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;
};
#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "entities/enums.h"

class DriverAnimator {
   private:
    const sf::Image spriteMap;

    enum class PlayerState {
        GO_RIGHT,
        GO_LEFT,
        GO_FORWARD,
        GO_BACK,
        FALLING,
        HIT,
    };
    PlayerState state;

    sf::Time starTime;
    int starColor;

    float sScale = 0;
    float fallScale = 1;
    int hitPos = 0;
    int hitTextuIdx[23] = {0,  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                           10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    int driftIndex = 0;

    static constexpr const float MOVEMENT_DRIFT_AMPLITUDE = 2.5f,
                                 MOVEMENT_SPEED_AMPLITUDE = 0.4f,
                                 MOVEMENT_DRIFT_PERIOD = 20.0f,
                                 MOVEMENT_SPEED_PERIOD = 0.015f;
    // time that it has been moving, used to convert it to pixels
    float spriteMovementDriftTime = 0.0f, spriteMovementSpeedTime = 0.0f;

   public:
    sf::Texture driving[12];
    sf::Texture others[5];
    sf::Texture textureParticles[3];
    sf::Sprite sprite, driftParticles[5];

    sf::Time smashTime;
    sf::Time smallTime;

    bool drifting = false;
    // movement in pixels
    float spriteMovementDrift = 0.0f, spriteMovementSpeed = 0.0f;

    DriverAnimator(const char *spriteFile, DriverControlType control);
    DriverAnimator(const char *spriteFile)
        : DriverAnimator(spriteFile, DriverControlType::DISABLED){};

    void goForward();
    void goRight(bool drift = false);
    void goLeft(bool drift = false);
    void fall();
    void hit();

    void small(sf::Time duration);
    void smash(sf::Time duration);
    void star(sf::Time duration);

    void drawParticles(sf::RenderTarget &window, sf::Sprite &driver,
                       bool small);

    void update(const float speedForward, const float speedTurn,
                const float height, const sf::Time &deltaTime);

    bool canDrive() const;

    void setViewSprite(float viewerAngle, float driverAngle);

    sf::Sprite getMinimapSprite(float angle, const float screenScale) const;

    void reset();
};

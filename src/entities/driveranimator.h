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
    int hitPos = 0;
    int hitTextuIdx[23] = {0,  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                           10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    int driftIndex = 0;

   public:
    sf::Texture driving[12];
    sf::Texture others[5];
    sf::Texture textureParticles[3];
    sf::Sprite sprite, driftParticles[5];

    sf::Time smashTime;
    sf::Time smallTime;

    bool drifting = false;

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

    void drawParticles(sf::RenderTarget &window, sf::Sprite *driver,
                       bool small);

    void update(float speedTurn, const sf::Time &deltaTime);

    bool canDrive() const;

    void setViewSprite(float viewerAngle, float driverAngle);

    sf::Sprite getMinimapSprite(float angle) const;

    void reset();
};

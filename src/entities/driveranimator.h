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
        CRASH,
        FALLING,
        SMASH,
        HIT
    };
    PlayerState state;

    sf::Time smashTime;
    sf::Time starTime;
    int starColor;

    float sScale = 0;
    int hitPos = 0;
    int hitTextuIdx[23] = {0,  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                           10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

   public:
    sf::Texture driving[12];
    sf::Texture others[5];
    sf::Sprite sprite;
 
    DriverAnimator(const char* spriteFile, DriverControlType control);
    DriverAnimator(const char* spriteFile) : DriverAnimator(spriteFile,DriverControlType::DISABLED) {}; 

    void goForward();
    void goRight();
    void goLeft();
    void fall();
    void hit();

    void smash(sf::Time duration);
    void star(sf::Time duration);

    void update(float speedTurn, const sf::Time &deltaTime);

    bool canDrive() const;

    void setViewSprite(float viewerAngle, float driverAngle);

    sf::Sprite getMinimapSprite(float angle) const;
};

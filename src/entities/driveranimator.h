#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

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
        HIT
    };
    PlayerState state;

    float sScale = 2;
    int hitPos = 0;
    int hitTextuIdx[23] = {0,  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                           10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

   public:
    sf::Texture driving[12];
    sf::Texture others[5];
    sf::Sprite sprite;

    DriverAnimator(const char* spriteFile);

    void goForward();
    void goRight();
    void goLeft();
    void fall();
    void hit();

    void update(float speedTurn);

    bool canDrive() const;

    void setViewSprite(float viewerAngle, float driverAngle);

    sf::Sprite getMinimapSprite(float angle) const;
};

#pragma once

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

class Effects {
   private:
    static const sf::Time SPEED_UPDATE_TIME;

   public:
    sf::Texture speeds[5];
    sf::RectangleShape speedEffect;
    float speedTime;
    int speedTexture;
    sf::Time speedUpdateTime;

    sf::RectangleShape whiteScreen, blackScreen;
    bool drawThunder;
    float thunderTime;
    float fadeTime, fadeInitialTime;
    bool fromBlack;

    sf::Vector2u winSize;
    float updateTime;

    Effects();

    void setWindowSize(sf::Vector2u s);

    void thunder(float time);
    void speed(float time);

    void blackFade(float time, bool _fromBlack);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);

    void stop();
    void reset();
};
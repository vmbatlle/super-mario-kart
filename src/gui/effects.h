#pragma once

#include <SFML/Graphics.hpp>
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

    sf::RectangleShape whiteScreen;
    bool drawThunder;
    float thunderTime;

    sf::Vector2u winSize;
    float updateTime;

    Effects();

    void setWindowSize(sf::Vector2u s);

    void thunder(float time);
    void speed(float time);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
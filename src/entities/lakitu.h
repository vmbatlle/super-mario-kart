#pragma once

#include <SFML/Graphics.hpp>

class Lakitu {
    public:
    sf::Texture finish[3];
    sf::Texture wrongDir[2];

    sf::Texture lakituLaps;
    sf::Texture lakituCatchPlayer;

    sf::Texture laps[4];
    sf::Texture lights[4];
    sf::Sprite sprite;

    bool onScreen;
    float screenTime;

    Lakitu();

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
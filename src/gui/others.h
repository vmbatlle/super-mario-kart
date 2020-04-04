#pragma once

#include <SFML/Graphics.hpp>

class Others {
    public:
    sf::Texture digits[10];      // 0 1 2 3 4 5 6 7 8 9
    sf::Texture simbols[9];      // : - .
    sf::Texture coinTx; 

    sf::Sprite coinCount[2];
    sf::Sprite coin, simbolX;

    sf::Vector2f rightUpCorner;
    sf::Vector2u winSize;

    int coins;

    Others();

    void addCoin();

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
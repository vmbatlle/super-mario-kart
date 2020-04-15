#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Others {
    public:
    // Coins
    sf::Texture digits[10];      // 0 1 2 3 4 5 6 7 8 9
    sf::Texture simbols[9];      // : - .
    sf::Texture coinTx; 

    sf::Sprite coinCount[2];
    sf::Sprite coin, simbolX;

    // Ranking
    sf::Texture ranks[8];           // 1 2 3 4 5 6 7 8
    sf::Texture ranksBlue[3];       // 1 2 3 4
    sf::Texture ranksBlue2[4];      // 1 2 3 4

    sf::Sprite rankSprite;

    sf::Vector2f rightDownCorner;
    sf::Vector2u winSize;

    int colours[3][3] = { {255, 255, 255},
                          {0, 0, 255},
                          {0, 255, 255} };
    int colorIndex;

    int coins;
    int rank;

    float updateTime;

    Others();

    void addCoin(int ammount = 1);
    void setRanking(int p);

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);

    void reset();
};
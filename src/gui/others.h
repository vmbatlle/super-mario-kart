#pragma once

#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "entities/enums.h"
#include "textutils.h"

class Others {
   public:
    // Coins
    sf::Texture coinTx;
    sf::Sprite coin;

    // Ranking
    sf::Texture ranks[8];       // 1 2 3 4 5 6 7 8
    sf::Texture ranksBlue[3];   // 1 2 3 4
    sf::Texture ranksBlue2[4];  // 1 2 3 4

    sf::Sprite rankSprite;

    sf::Vector2f rightDownCorner;
    sf::Vector2u winSize;

    int colours[3][3] = {{255, 255, 255}, {0, 0, 255}, {0, 255, 255}};
    int colorIndex;

    int coins;
    std::string strCoins;
    int rank;

    float updateTime;

    sf::Vector2f txtPos;

    float coinScale, rankScale, simbolScale, factor;

    Others();

    void addCoin(int ammount = 1);
    void setRanking(int p, bool scale = false);

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);

    void reset(bool rankReset);
};
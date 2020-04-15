#include "others.h"

Others::Others() {
    //coins
    std::string spriteFile = "assets/gui/letters.png";
    for (int i = 0; i < 10; i++)
        digits[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 9), 19, 8, 8));
    for (int i = 0; i < 8; i++)
        simbols[i].loadFromFile(spriteFile, sf::IntRect(91 + (i * 9), 19, 8, 8));
    simbols[8].loadFromFile(spriteFile, sf::IntRect(91, 10, 8, 8));
    coinTx.loadFromFile(spriteFile, sf::IntRect(90, 62, 10, 14));

    for (int i = 0; i < 2; i++) {
        coinCount[i].setTexture(digits[0]);
        coinCount[i].scale(2,2);
        coinCount[i].setOrigin(coinCount[i].getLocalBounds().width, coinCount[i].getLocalBounds().height);
    }

    coins = 0;

    coin.setTexture(coinTx);
    coin.scale(1.5, 1.5);
    coin.setOrigin(coin.getLocalBounds().width/2, coin.getLocalBounds().height/2);

    simbolX.setTexture(simbols[8]);
    simbolX.scale(2,2);
    simbolX.setOrigin(simbolX.getLocalBounds().width, simbolX.getLocalBounds().height);

    //Ranks
    spriteFile = "assets/gui/ranks.png";
    for (int i = 0; i < 8; i++)
        ranks[i].loadFromFile(spriteFile, sf::IntRect(0 + (i * 17), 0, 16, 20));
    for (int i = 0; i < 4; i++)
        ranksBlue[i].loadFromFile(spriteFile, sf::IntRect(0 + (i * 17), 21, 16, 20));
    for (int i = 0; i < 4; i++)
        ranksBlue2[i].loadFromFile(spriteFile, sf::IntRect(68 + (i * 17), 21, 16, 20));

    rankSprite.setTexture(ranks[7]);
    rankSprite.setScale(3,3);
    rankSprite.setColor(sf::Color(255, 255, 255, 180));
    rankSprite.setOrigin(rankSprite.getLocalBounds().width, rankSprite.getLocalBounds().height);

    colorIndex = 0;

    updateTime = 0.1;
    rightDownCorner = sf::Vector2f(0,0);
    winSize = sf::Vector2u(0,0);

}

void Others::setWindowSize(sf::Vector2u s) {
    winSize = s;

    //Update sprite position
    int separationPixels = 2;
    int xSizeSprite = coinCount[0].getGlobalBounds().width;
    rightDownCorner = sf::Vector2f(s.x*95/100, s.y*45/100);

    int x_pos = rightDownCorner.x;
    int y_pos = rightDownCorner.y;

    rankSprite.setPosition(x_pos, y_pos);
    x_pos -= rankSprite.getGlobalBounds().width + 6;
    //y_pos -= rankSprite.getGlobalBounds().width/4;
    coinCount[0].setPosition(x_pos, y_pos);
    x_pos -= xSizeSprite + separationPixels;
    coinCount[1].setPosition(x_pos, y_pos);
    x_pos -= xSizeSprite + separationPixels;
    simbolX.setPosition(x_pos, y_pos);
    x_pos -= xSizeSprite + separationPixels;
    coin.setPosition(x_pos - coin.getGlobalBounds().width/2, y_pos - coinCount[0].getGlobalBounds().height/2);
    
}

void Others::addCoin(int ammount) {
    coins += ammount;
    if (coins > 99) 
        coins = 99;
}

void Others::setRanking(int i) {
    if (i > 8) i = 8;
    if (i < 1) i = 1;
    rankSprite.setTexture(ranks[i-1]);
    if (i < rank) {
        rankSprite.setScale(6,6);
        //rankSprite.scale(2,2);
    }
    rank = i;
}

void Others::update(const sf::Time &deltaTime) {

    updateTime -= deltaTime.asSeconds();

    int coinD = coins/10;
    int coinU = coins % 10;
    coinCount[0].setTexture(digits[coinU]);
    coinCount[1].setTexture(digits[coinD]);

    if (rankSprite.getScale().x > 3) {
        rankSprite.scale(0.95,0.95);
        if (rankSprite.getScale().x < 3)
            rankSprite.setScale(3,3);
    }

    if (updateTime <= 0) {
        if (rank <= 4) {
            colorIndex = (colorIndex + 1) % 3;
            rankSprite.setColor(sf::Color(colours[colorIndex][0], 
                                        colours[colorIndex][1],
                                        colours[colorIndex][2], 180));
        } else {
            rankSprite.setColor(sf::Color(colours[0][0], 
                                        colours[0][1],
                                        colours[0][2], 180));
        }
        updateTime = 0.1;
    }

}

void Others::draw(sf::RenderTarget &window) {
    for (int i = 0; i < 2; i++) {
        window.draw(coinCount[i]);
    }
    window.draw(rankSprite);
    window.draw(coin);
    window.draw(simbolX);
    
}

void Others::reset() {
    coins = 0;
}
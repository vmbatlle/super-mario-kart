#include "others.h"

Others::Others() {
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
    }

    coins = 0;

    coin.setTexture(coinTx);
    coin.scale(1.5, 1.5);
    coin.setOrigin(coin.getLocalBounds().width/2, coin.getLocalBounds().height/2);

    simbolX.setTexture(simbols[8]);
    simbolX.scale(2,2);

    rightUpCorner = sf::Vector2f(0,0);
    winSize = sf::Vector2u(0,0);

}

void Others::setWindowSize(sf::Vector2u s) {
    winSize = s;

    //Update sprite position
    int separationPixels = 2;
    int xSizeSprite = coinCount[0].getGlobalBounds().width;
    rightUpCorner = sf::Vector2f(s.x*98/100, s.y*40/100);

    int x_pos = rightUpCorner.x - xSizeSprite - separationPixels;

    coinCount[0].setPosition(x_pos, rightUpCorner.y);
    x_pos -= xSizeSprite + separationPixels;
    coinCount[1].setPosition(x_pos, rightUpCorner.y);
    x_pos -= xSizeSprite + separationPixels;
    simbolX.setPosition(x_pos, rightUpCorner.y);
    x_pos -= xSizeSprite + separationPixels;
    coin.setPosition(x_pos, rightUpCorner.y + coinCount[0].getGlobalBounds().height/2);
    
}

void Others::addCoin() {
    coins++;
    if (coins > 99) 
        coins = 99;
}

void Others::update(const sf::Time &deltaTime) {

    int coinD = coins/10;
    int coinU = coins % 10;
    coinCount[0].setTexture(digits[coinU]);
    coinCount[1].setTexture(digits[coinD]);

}

void Others::draw(sf::RenderTarget &window) {
    for (int i = 0; i < 2; i++) {
        window.draw(coinCount[i]);
    }
    window.draw(coin);
    window.draw(simbolX);
}
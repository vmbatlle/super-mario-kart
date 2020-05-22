#include "others.h"

Others::Others() {
    // coins
    std::string spriteFile = "assets/gui/letters.png";
    coinTx.loadFromFile(spriteFile, sf::IntRect(90, 62, 10, 14));
    simbolScale = 2;

    coins = 0;
    strCoins = "00";

    coin.setTexture(coinTx);
    coinScale = 1.5;
    coin.scale(coinScale, coinScale);
    coin.setOrigin(coin.getLocalBounds().width, coin.getLocalBounds().height);

    // Ranks
    spriteFile = "assets/gui/ranks.png";
    for (int i = 0; i < 8; i++)
        ranks[i].loadFromFile(spriteFile, sf::IntRect(0 + (i * 17), 0, 16, 20));
    for (int i = 0; i < 4; i++)
        ranksBlue[i].loadFromFile(spriteFile,
                                  sf::IntRect(0 + (i * 17), 21, 16, 20));
    for (int i = 0; i < 4; i++)
        ranksBlue2[i].loadFromFile(spriteFile,
                                   sf::IntRect(68 + (i * 17), 21, 16, 20));

    rankSprite.setTexture(ranks[7]);
    rankScale = 3;
    rankSprite.setScale(rankScale, rankScale);
    rankSprite.setColor(sf::Color(255, 255, 255, 180));
    rankSprite.setOrigin(rankSprite.getLocalBounds().width,
                         rankSprite.getLocalBounds().height);

    colorIndex = 0;

    updateTime = 0.1;
    rightDownCorner = sf::Vector2f(0, 0);
    winSize = sf::Vector2u(0, 0);
    txtPos = sf::Vector2f(0, 0);
    factor = 1;
}

void Others::setWindowSize(sf::Vector2u s) {
    winSize = s;

    factor = winSize.x / BASIC_WIDTH;
    rankSprite.setScale(rankScale * factor, rankScale * factor);
    coin.setScale(coinScale * factor, coinScale * factor);
    simbolScale = 2 * factor;
    // Update sprite position
    rightDownCorner = sf::Vector2f(s.x * 95 / 100, s.y * 45 / 100);

    int x_pos = rightDownCorner.x;
    int y_pos = rightDownCorner.y;

    rankSprite.setPosition(x_pos, y_pos);
    x_pos -= rankSprite.getGlobalBounds().width + 6 * factor;
    txtPos = sf::Vector2f(x_pos, y_pos);
    x_pos -= 17 * 3 * factor;
    coin.setPosition(x_pos - coin.getGlobalBounds().width / 2, y_pos);
}

void Others::addCoin(int ammount) {
    coins += ammount;
    if (coins > 10) {
        coins = 10;
    } else if (coins < 0) {
        coins = 0;
    }
}

void Others::setRanking(int i, bool scale) {
    if (i > 8) i = 8;
    if (i < 1) i = 1;
    rankSprite.setTexture(ranks[i - 1]);
    if (i < rank || scale) {
        rankSprite.setScale(2.5 * rankScale * factor, 2.5 * rankScale * factor);
    }
    rank = i;
}

void Others::update(const sf::Time &deltaTime) {
    updateTime -= deltaTime.asSeconds();

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << coins;
    strCoins = ss.str();

    if (rankSprite.getScale().x > rankScale * factor) {
        rankSprite.scale(0.95, 0.95);
        if (rankSprite.getScale().x < rankScale * factor)
            rankSprite.setScale(rankScale * factor, rankScale * factor);
    }

    if (updateTime <= 0) {
        if (rank <= 4) {
            colorIndex = (colorIndex + 1) % 3;
            rankSprite.setColor(sf::Color(colours[colorIndex][0],
                                          colours[colorIndex][1],
                                          colours[colorIndex][2], 180));
        } else {
            rankSprite.setColor(
                sf::Color(colours[0][0], colours[0][1], colours[0][2], 180));
        }
        updateTime = 0.1;
    }
}

void Others::draw(sf::RenderTarget &window) {
    window.draw(rankSprite);
    window.draw(coin);

    TextUtils::write(window, "x" + strCoins, txtPos, simbolScale,
                     sf::Color::Black, true, TextUtils::TextAlign::RIGHT,
                     TextUtils::TextVerticalAlign::BOTTOM);
}

void Others::reset(bool rankReset) {
    coins = 0;
    strCoins = "00";
    rankSprite.setColor(
        sf::Color(colours[0][0], colours[0][1], colours[0][2], 180));
    rankSprite.setScale(rankScale * factor, rankScale * factor);

    if (rankReset) {
        rankSprite.setTexture(ranks[7]);
    }
}
#include "timer.h"

Timer::Timer() {
    std::string spriteFile = "assets/gui/digits.png";
    for (int i = 0; i < 10; i++)
        digits[i].loadFromFile(spriteFile, sf::IntRect(0 + (i * 9), 0, 8, 14));
    for (int i = 0; i < 2; i++)
        comas[i].loadFromFile(spriteFile, sf::IntRect(90 + (i * 9), 0, 8, 14));
    for (int i = 0; i < 3; i++)
        custom[i].loadFromFile(spriteFile,
                               sf::IntRect(137 + (i * 11), 0, 8, 14));

    scaleFactor = sf::Vector2f(2, 2);
    for (int i = 0; i < 6; i++) {
        timerDigits[i].setTexture(digits[0]);
        timerDigits[i].scale(scaleFactor);
    }

    timerCommas[0].setTexture(comas[0]);
    timerCommas[0].scale(scaleFactor);
    timerCommas[1].setTexture(comas[1]);
    timerCommas[1].scale(scaleFactor);

    time = time.Zero;
    factor = 0;

    leftUpCorner = sf::Vector2f(0, 0);
    winSize = sf::Vector2u(0, 0);
}

sf::Vector2f Timer::getItemPos() {
    return sf::Vector2f(
        leftUpCorner.x,
        leftUpCorner.y + timerDigits[0].getGlobalBounds().height / 2);
}

void Timer::setWindowSize(sf::Vector2u s) {
    winSize = s;

    factor = winSize.x / BASIC_WIDTH;
    for (int i = 0; i < 6; i++) {
        timerDigits[i].setScale(scaleFactor.x * factor, scaleFactor.y * factor);
    }

    timerCommas[0].setScale(scaleFactor.x * factor, scaleFactor.y * factor);
    timerCommas[1].setScale(scaleFactor.x * factor, scaleFactor.y * factor);

    // Update sprite position
    int separationPixels = 2 * factor;
    int xSizeSprite = timerDigits[0].getGlobalBounds().width;
    leftUpCorner = sf::Vector2f(s.x * 98 / 100, s.y * 2 / 100);

    int x_pos = leftUpCorner.x - 8 * (xSizeSprite + separationPixels);
    leftUpCorner = sf::Vector2f(x_pos, leftUpCorner.y);
    int digitIndex = 0;
    for (int i = 0; i < 8; i++) {
        if (i == 2) {  // coma 1
            timerCommas[0].setPosition(x_pos, leftUpCorner.y);
        } else if (i == 5) {  // coma 2
            timerCommas[1].setPosition(x_pos, leftUpCorner.y);
        } else {  // digit
            timerDigits[digitIndex].setPosition(x_pos, leftUpCorner.y);
            digitIndex++;
        }
        x_pos += xSizeSprite + separationPixels;
    }
}

void Timer::update(const sf::Time &deltaTime) {
    time += deltaTime;

    long timeAsMilli = time.asMilliseconds();
    int minutes = timeAsMilli / 60000;
    timeAsMilli -= minutes * 60000;
    int seconds = timeAsMilli / 1000;
    timeAsMilli -= seconds * 1000;
    int millis = timeAsMilli / 10;

    timerDigits[0].setTexture(digits[minutes / 10]);
    timerDigits[1].setTexture(digits[minutes % 10]);

    timerDigits[2].setTexture(digits[seconds / 10]);
    timerDigits[3].setTexture(digits[seconds % 10]);

    timerDigits[4].setTexture(digits[millis / 10]);
    timerDigits[5].setTexture(digits[millis % 10]);
}

void Timer::draw(sf::RenderTarget &window, const sf::Color &color) {
    for (int i = 0; i < 6; i++) {
        timerDigits[i].setColor(color);
        window.draw(timerDigits[i]);
    }
    timerCommas[0].setColor(color);
    window.draw(timerCommas[0]);
    timerCommas[1].setColor(color);
    window.draw(timerCommas[1]);
}

void Timer::reset() {
    time = time.Zero;
    timerDigits[0].setTexture(digits[0]);
    timerDigits[1].setTexture(digits[0]);
    timerDigits[2].setTexture(digits[0]);
    timerDigits[3].setTexture(digits[0]);
    timerDigits[4].setTexture(digits[0]);
    timerDigits[5].setTexture(digits[0]);
}
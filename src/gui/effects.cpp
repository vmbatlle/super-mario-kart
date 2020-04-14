#include "effects.h"

const sf::Time Effects::SPEED_UPDATE_TIME = sf::seconds(0.07f);

Effects::Effects() {
    for (int i = 0; i < 5; i++) {
        std::string spriteFile = "assets/speed/speed" + std::to_string(i) + ".png";
        speeds[i].loadFromFile(spriteFile);
    }
    speedEffect.setTexture(&speeds[0]);
    speedTime = 0;
    speedTexture = 0;
    speedUpdateTime = SPEED_UPDATE_TIME;

    whiteScreen.setFillColor(sf::Color(255,255,255,175));
    thunderTime = 0;
    drawThunder = false;
}

void Effects::setWindowSize(sf::Vector2u s) {
    whiteScreen.setSize(sf::Vector2f(s));
    speedEffect.setSize(sf::Vector2f(s));
}

void Effects::update(const sf::Time &deltaTime) {

    if (thunderTime > 0) {
        drawThunder = !drawThunder;
        thunderTime -= deltaTime.asSeconds();
    } else {
        drawThunder = false;
    }
    
    if (speedTime > 0) {
        if (speedUpdateTime <= sf::seconds(0)) {
            speedUpdateTime = SPEED_UPDATE_TIME;
            speedTexture = (speedTexture + 1) % 5;
            speedEffect.setTexture(&speeds[speedTexture]);
        } else {
            speedUpdateTime -= deltaTime;
        }
        speedTime -= deltaTime.asSeconds();
    }
}

void Effects::thunder(float time) {
    thunderTime = time;
}

void Effects::speed(float time) {
    speedTime = time;
}

void Effects::draw(sf::RenderTarget &window) {
    if (drawThunder) {
        window.draw(whiteScreen);
    }
    if (speedTime > 0) {
        window.draw(speedEffect);
    }
}
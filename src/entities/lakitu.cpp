#include "lakitu.h"

Lakitu::Lakitu() {
    std::string spriteFile = "assets/misc/lakitu.png";
    for (int i = 0; i < 3; i++)
        finish[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 3; i < 5; i++)
        worngDir[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 0; i < 4; i++)
       laps[i].loadFromFile(spriteFile, sf::IntRect(43 + (i * 25), 76, 24, 16));
    for (int i = 0; i < 4; i++)
        lights[i].loadFromFile(spriteFile, sf::IntRect(149 + (i * 9), 69, 8, 24));
    lakituLaps.loadFromFile(spriteFile, sf::IntRect(11, 68, 22, 31));

    sprite.setTexture(finish[0]);
    sprite.setPosition(0,0);
    sprite.setScale(3,3);
    onScreen = false;
    screenTime = 5;
}


void Lakitu::update(const sf::Time &deltaTime) {
    if (onScreen) {
        screenTime -= deltaTime.asSeconds();
        if (screenTime <= 0) { 
            onScreen = false;
            screenTime = 5;
        }
    }
}

void Lakitu::draw(sf::RenderTarget &window) {
    sprite.setPosition(window.getSize().x/4.f, window.getSize().y/4.f);
    window.draw(sprite);
}
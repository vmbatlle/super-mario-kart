#include "driver.h"
#include <iostream>

Driver::Driver() {
    sprite.setOrigin(0,0);
}

void Driver::init(char* file) {

    //sprite.setColor(sf::Color::Green);
    texture[1].loadFromFile(file, sf::IntRect(95, 32, 33, 32));
    texture[0].loadFromFile(file, sf::IntRect(0, 0, 32, 31));
    sprite.setTexture(texture[0]);

    state = States::GO_FORWARD;
    sprite.setOrigin(texture[0].getSize().x/2, texture[0].getSize().y/2 );
    sprite.scale(3,3);
};


void Driver::goForward() {
    sprite.setTexture(texture[0]);
    sprite.setScale(3,3);
};

void Driver::goRight() {
    sprite.setTexture(texture[1]);
    sprite.setScale(3,3);
};

void Driver::goLeft() {
    sprite.setTexture(texture[1]);
    sprite.setScale(3,-3);

};


sf::Sprite Driver::getSprite() {
    return sprite;
}


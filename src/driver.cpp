#include "driver.h"
#include <iostream>

Driver::Driver() { sprite.setOrigin(0, 0); }

void Driver::init(const char* file) {
    // sprite.setColor(sf::Color::Green);
    for (int i = 0; i < 12; i++)
        driving[i].loadFromFile(file, sf::IntRect(32*i, 32, 32, 32));
    for (int i = 0; i < 5; i++)
        others[i].loadFromFile(file, sf::IntRect(32*i, 0, 32, 32));

    sprite.setTexture(driving[0]);

    state = States::GO_FORWARD;
    sprite.setOrigin(driving[0].getSize().x / 2, driving[0].getSize().y / 2);
    sprite.scale(sScale, sScale);
};

void Driver::goForward() {
    if ( canDrive()) state = States::GO_FORWARD;
    sprite.setTexture(driving[0]);
    sprite.setScale(sScale, sScale);
};

void Driver::goRight(float speedTurn) {
    if ( canDrive()) state = States::GO_RIGHT;
         if (speedTurn < 1.0f *  (1.f/4))   sprite.setTexture(driving[1]);
    else if (speedTurn < 1.0f *  (1.f/2))   sprite.setTexture(driving[2]);
    else if (speedTurn < 1.0f *  (3.f/4))   sprite.setTexture(driving[3]);
    else                                    sprite.setTexture(driving[4]);
    sprite.setScale(sScale, sScale);
};

void Driver::goLeft(float speedTurn) {
    if ( canDrive()) state = States::GO_LEFT;
         if (speedTurn > -1.0f *  (1.f/4))   sprite.setTexture(driving[1]);
    else if (speedTurn > -1.0f *  (1.f/2))   sprite.setTexture(driving[2]);
    else if (speedTurn > -1.0f *  (3.f/4))   sprite.setTexture(driving[3]);
    else                                     sprite.setTexture(driving[4]);
    sprite.setScale(-sScale, sScale);
};

void Driver::hiten(const sf::Time& time) {
    state = States::HITEN;
    // int sec = (int)(t.asSeconds() * 1000 ) % 22;
    // sprite.setTexture(driving[sec-11]);
    // if ( sec > 11 ) sprite.setScale(-sScale, sScale);

    hitPos = (hitPos + 1) % 22;
    sprite.setTexture(driving[hitTextuIdx[hitPos]]);
    if ( hitPos > 11 ) sprite.setScale(-sScale, sScale);
};

void Driver::fall(const sf::Time& time) {
    state = States::FALLING;

    sprite.scale(0.9f, 0.95f);
    sprite.move(sf::Vector2f(0,-2));
};

void Driver::update(const sf::Time& time, float speedTurn) {
    switch (state) {
    case States::GO_FORWARD:
        sprite.setTexture(driving[0]);
        sprite.setScale(sScale, sScale);
        break;

    case States::GO_RIGHT:
             if (speedTurn < 1.0f *  (1.f/4))   sprite.setTexture(driving[1]);
        else if (speedTurn < 1.0f *  (1.f/2))   sprite.setTexture(driving[2]);
        else if (speedTurn < 1.0f *  (3.f/4))   sprite.setTexture(driving[3]);
        else                                    sprite.setTexture(driving[4]);
        sprite.setScale(sScale, sScale);
        break;
    
    case States::GO_LEFT:
             if (speedTurn > -1.0f *  (1.f/4))   sprite.setTexture(driving[1]);
        else if (speedTurn > -1.0f *  (1.f/2))   sprite.setTexture(driving[2]);
        else if (speedTurn > -1.0f *  (3.f/4))   sprite.setTexture(driving[3]);
        else                                     sprite.setTexture(driving[4]);
        sprite.setScale(-sScale, sScale);
        break;
    
    case States::GO_BACK:
        /* code */
        break;
    
    case States::HITEN:
        hitPos = (hitPos + 1) % 22;
        sprite.setTexture(driving[hitTextuIdx[hitPos]]);
        if ( hitPos > 11 ) sprite.setScale(-sScale, sScale);
        break;
    
    case States::FALLING:   // NO va
        {
            sf::Vector2f s = sprite.getScale();
            if (s.x > 0) { 
                sprite.setScale(s.x - 0.5f, s.y - 0.5f);
                sprite.move(sf::Vector2f(0,-2));
            }
            break;
        }
    
    case States::CRASH:
        /* code */
        break;
    
    default:
        sprite.setTexture(driving[0]);
        break;
    }

}

bool Driver::canDrive() {
    return state != States::HITEN || state != States::FALLING;
}

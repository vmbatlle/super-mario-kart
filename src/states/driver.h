#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum States {
    GO_RIGHT,
    GO_LEFT,
    GO_FORWARD,
    GO_BACK,
    CRASH
};

class Driver {
    private:
    
    sf::Image spriteMap;
    States state;

    public:

    sf::Texture texture[3];
    sf::Sprite sprite;

    Driver();

    void init(char* file);
    void goForward();
    void goRight();
    void goLeft();

    sf::Sprite getSprite();
};

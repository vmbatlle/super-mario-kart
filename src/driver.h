#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum States { GO_RIGHT, GO_LEFT, GO_FORWARD, GO_BACK, CRASH, FALLING, HITEN };

class Driver {
   private:
    sf::Image spriteMap;
    States state;

    float sScale = 3;
    int hitPos = 0;
    int hitTextuIdx[22] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                      10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

   public:
    sf::Texture driving[12];
    sf::Texture others[5];
    sf::Sprite sprite;

    Driver();

    void init(const char* file);
    void goForward();
    void goRight(float speedTurn);
    void goLeft(float speedTurn);


    void fall(const sf::Time& t);
    void hiten(const sf::Time& t);

    void update(const sf::Time& time, float speedTurn);

    bool canDrive();
};

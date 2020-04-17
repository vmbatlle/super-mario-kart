#pragma once

class Lakitu;

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include <iostream>

class Lakitu {
   private:
    Lakitu();
    static Lakitu instance;

   public:
    sf::Texture finish[3];
    sf::Texture wrongDir[2];

    sf::Texture lakituLaps;
    sf::Texture lakituCatchPlayer;

    sf::Texture laps[4];
    sf::Texture start[2];
    sf::Texture lights[4];
    sf::Sprite sprite, signSprite, lightSprite;

    Driver *ptrDriver;

    const int finishAnim[4] = {0, 1, 2, 1};

    int textIndex;

    enum class LakituState { START, WRONG_DIR, LAP, FINISH, SLEEP, PICKUP };
    LakituState state;

    int lap;
    int light;

    float screenTime, wrongTime;
    float nextFrameTime;
    float frameTime;
    bool started;

    bool inAnimation;

    sf::Vector2u winSize;

    static void showStart();
    static void showLap(int numLap);
    static void showFinish();

    static void setWrongDir(bool wrongDir);
    static void pickUpDriver(Driver *driver);

    static void showUntil(float seconds, const sf::Time &deltaTime);

    static void sleep();

    static bool hasStarted();
    static bool isSleeping();

    static void setWindowSize(sf::Vector2u s);

    static void update(const sf::Time &deltaTime);
    static void draw(sf::RenderTarget &window);
};
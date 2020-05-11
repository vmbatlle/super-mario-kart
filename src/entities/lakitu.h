#pragma once

class Lakitu;

#include <SFML/Graphics.hpp>
#include <iostream>

#include "entities/driver.h"
#include "entities/enums.h"

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

    sf::Texture assetShadow;
    sf::Sprite spriteShadow;

    Driver *ptrDriver;

    const int finishAnim[4] = {0, 1, 2, 1};

    int textIndex;

    float sScale = 0;

    enum class LakituState : int {
        START,
        WRONG_DIR,
        LAP,
        FINISH,
        SLEEP,
        PICKUP,
        _COUNT
    };
    int animationPriorities[(int)LakituState::_COUNT] = {0};

    LakituState state;

    int lap;
    int light;

    float screenTime, wrongTime;
    float nextFrameTime;
    float frameTime;
    bool started;

    bool soundStarted;

    bool drawShadows;

    int currentAnimationPriority;

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
    static void drawShadow(sf::RenderTarget &window);

    static void reset();
};
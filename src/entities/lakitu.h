#pragma once

class Lakitu;

#include <SFML/Graphics.hpp>
//#include "../game.h"
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

    const int finishAnim[4] = {0, 1, 2, 1};

    int textIndex;

    enum class LakituState {
        START,
        WORNG_DIR,
        LAP,
        FINISH,
        SLEEP
    };
    LakituState state;

    int lap;
    int light;
    
    float screenTime;
    float nextFrameTime;
    float frameTime;
    bool started;

    sf::Vector2u winSize;

    static void showStart();
    static void showLap(int numLap);
    static void showUntil(float seconds, const sf::Time &deltaTime);
    static void showFinish();

    static bool hasStarted();
    static bool isSleeping();

    static void setWindowSize(sf::Vector2u s);

    static void update(const sf::Time &deltaTime);
    static void draw(sf::RenderTarget &window);
};
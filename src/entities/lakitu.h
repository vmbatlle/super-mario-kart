#pragma once

#include <SFML/Graphics.hpp>
//#include "../game.h"

class Lakitu {
    public:
    sf::Texture finish[3];
    sf::Texture wrongDir[2];

    sf::Texture lakituLaps;
    sf::Texture lakituCatchPlayer;

    sf::Texture laps[4];
    sf::Texture lights[4];
    sf::Sprite sprite, object;

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
    float screenTime;
    float nextFrameTime;
    float frameTime;

    sf::Vector2u winSize;

    Lakitu();

    void showLap(int numLap);
    void showUntil(float seconds, const sf::Time &deltaTime);
    void showFinish();

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
#pragma once

#include <SFML/Graphics.hpp>

class Lakitu {
    public:
    sf::Texture finish[3];
    sf::Texture wrongDir[2];

    sf::Texture lakituLaps;
    sf::Texture lakituCatchPlayer;

    sf::Texture laps[4];
    sf::Texture lights[4];
    sf::Sprite sprite, object;

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

    Lakitu();

    void showLap(int numLap);
    void showUntil(float seconds, const sf::Time &deltaTime);
    void showFinish();

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
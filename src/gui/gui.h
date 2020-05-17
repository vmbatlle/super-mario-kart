#pragma once

class Gui;

#include <SFML/Graphics.hpp>
#include <iostream>

#include "entities/enums.h"
#include "gui/effects.h"
#include "gui/itemIndicator.h"
#include "gui/others.h"
#include "gui/timer.h"

class Gui {
   private:
    Gui();
    static Gui instance;

   public:
    Timer timer;
    ItemIndicator itemInd;
    Others others;
    Effects effects;

    sf::Vector2u winSize;

    static void setPowerUp(PowerUps power);
    static void addCoin(int ammount = 1);

    static void setRanking(int r);

    static void thunder();
    static void speed(float time);
    static void fade(float time, bool fromBlack);

    static bool canUseItem();
    static bool isBlackScreen(bool total = false);

    static void setWindowSize(sf::Vector2u s);

    static void update(const sf::Time &deltaTime);
    static void draw(sf::RenderTarget &window,
                     const sf::Color &timerColor = sf::Color::White);

    static void endRace();
    static void reset(bool rankReset = true);
    static void stopEffects();
};
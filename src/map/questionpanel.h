#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class QuestionPanel : public FloorObject {
   private:
    static sf::Image assetActive, assetInactive;

   public:
    bool active;
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &activeRect,
                           const sf::IntRect &inactiveRect);

    QuestionPanel(const sf::Vector2f topLeftPixels);

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
};
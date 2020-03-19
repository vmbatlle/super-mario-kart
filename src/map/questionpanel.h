#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class QuestionPanel : public FloorObject {
   private:
    static sf::Image assetsActive[(int)Orientation::__COUNT],
        assetsInactive[(int)Orientation::__COUNT];

   public:
    bool active;
    static void loadAssets(const std::string &assetName,
                           sf::IntRect activeRect, sf::IntRect inactiveRect);

    QuestionPanel(const sf::Vector2f &topLeftPixels,
                  const Orientation _orientation);

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
};
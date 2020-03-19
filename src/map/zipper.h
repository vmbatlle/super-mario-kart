#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class Zipper : public FloorObject {
   private:
    static sf::Image assets[(int)Orientation::__COUNT];

   public:
    static void loadAssets(const std::string &assetName, sf::IntRect roi);

    Zipper(const sf::Vector2f &topLeftPixels, const Orientation _orientation);

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
};
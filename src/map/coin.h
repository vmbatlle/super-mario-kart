#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class Coin : public FloorObject {
   private:
    static sf::Image assetsActive[(int)Orientation::__COUNT];
    static sf::Image assetInactive;

   public:
    bool active;
    static void loadAssets(const std::string &assetName, sf::IntRect roi);

    Coin(const sf::Vector2f &topLeftPixels, const Orientation _orientation);

    virtual void update() const override;

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
};
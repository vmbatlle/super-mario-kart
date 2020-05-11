#pragma once

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class Coin : public FloorObject {
   private:
    static sf::Image assetsActive[(int)FloorObjectOrientation::__COUNT];
    static sf::Image assetInactive;

    virtual FloorObjectState getInitialState() const override {
        return FloorObjectState::ACTIVE;
    };

   public:
    static void loadAssets(const std::string &assetName, sf::IntRect roi);

    Coin(const sf::Vector2f &topLeftPixels,
         const FloorObjectOrientation _orientation);

    void interactWith(const DriverPtr &driver) override;

    virtual void applyChanges() const override;

    const sf::Image &getCurrentImage() const override;
    virtual MapLand getCurrentLand() const override;
};
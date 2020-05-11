#pragma once

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class OilSlick : public FloorObject {
   private:
    static sf::Image assets[(int)FloorObjectOrientation::__COUNT];

    virtual FloorObjectState getInitialState() const override {
        return FloorObjectState::NONE;
    }

   public:
    static void loadAssets(const std::string &assetName, sf::IntRect roi);

    OilSlick(const sf::Vector2f &topLeftPixels,
             const FloorObjectOrientation _orientation);

    virtual void applyChanges() const override;

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
    virtual MapLand getCurrentLand() const override;
};
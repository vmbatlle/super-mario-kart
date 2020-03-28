#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"

class QuestionPanel : public FloorObject {
   private:
    static sf::Image assetsActive[(int)FloorObjectOrientation::__COUNT],
        assetsInactive[(int)FloorObjectOrientation::__COUNT];

    virtual FloorObjectState getInitialState() const override {
        return FloorObjectState::ACTIVE;
    }

   public:
    static void loadAssets(const std::string &assetName, sf::IntRect activeRect,
                           sf::IntRect inactiveRect);

    QuestionPanel(const sf::Vector2f &topLeftPixels,
                  const FloorObjectOrientation _orientation);

    virtual void applyChanges() const override;

    void interactWith(const DriverPtr &driver) override;

    const sf::Image &getCurrentImage() const override;
    virtual MapLand getCurrentLand() const override;
};
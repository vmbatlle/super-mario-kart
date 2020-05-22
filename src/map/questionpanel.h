#pragma once

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

#include "entities/driver.h"
#include "gui/gui.h"
#include "map/floorobject.h"
#include "map/map.h"

class QuestionPanel : public FloorObject {
   private:
    static constexpr const int NUM_ITEMS_ARRAY = 16;
    using ItemArray = std::array<PowerUps, NUM_ITEMS_ARRAY>;
    // item buckets depending on user's position
    static ItemArray ITEMS_1, ITEMS_23, ITEMS_45, ITEMS_67, ITEMS_8;
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
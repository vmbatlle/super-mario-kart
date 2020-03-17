#pragma once

#include <SFML/Graphics.hpp>
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

    const sf::Image &getCurrentImage() const override;
};
#include "questionpanel.h"

sf::Image QuestionPanel::assetActive;
sf::Image QuestionPanel::assetInactive;

void QuestionPanel::loadAssets(const std::string &assetName,
                               const sf::IntRect &activeRect,
                               const sf::IntRect &inactiveRect) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    assetActive.create(activeRect.width, activeRect.height);
    assetInactive.create(inactiveRect.width, inactiveRect.height);
    assetActive.copy(rawImage, 0, 0, activeRect);
    assetInactive.copy(rawImage, 0, 0, inactiveRect);
}

QuestionPanel::QuestionPanel(const sf::Vector2f topLeftPixels)
    : FloorObject(topLeftPixels, sf::Vector2f(assetActive.getSize()),
                  Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT),
      active(true) {}

const sf::Image &QuestionPanel::getCurrentImage() const {
    return active ? assetActive : assetInactive;
}
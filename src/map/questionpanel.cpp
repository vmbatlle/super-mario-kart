#include "questionpanel.h"

sf::Image QuestionPanel::assetsActive[];
sf::Image QuestionPanel::assetsInactive[];

void QuestionPanel::loadAssets(const std::string &assetName,
                               sf::IntRect activeRect,
                               sf::IntRect inactiveRect) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)Orientation::__COUNT; i++) {
        assetsActive[i].create(activeRect.width, activeRect.height);
        assetsInactive[i].create(inactiveRect.width, inactiveRect.height);
        assetsActive[i].copy(rawImage, 0, 0, activeRect);
        assetsInactive[i].copy(rawImage, 0, 0, inactiveRect);
        // expect next rotation on the left side
        activeRect.left += activeRect.width;
        inactiveRect.left += inactiveRect.width;
    }
}

QuestionPanel::QuestionPanel(const sf::Vector2f &topLeftPixels,
                             const Orientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assetsActive[(int)_orientation].getSize()),
                  Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT, _orientation),
      active(true) {}

void QuestionPanel::update() const {
    Map::updateAssetCourse(getCurrentImage(), topLeftPixel);
    Map::setLand(sf::Vector2f(hitbox.left, hitbox.top),
                 sf::Vector2f(getCurrentImage().getSize()), Map::Land::OTHER);
}

const sf::Image &QuestionPanel::getCurrentImage() const {
    return active ? assetsActive[(int)orientation]
                  : assetsInactive[(int)orientation];
}

void QuestionPanel::interactWith(const DriverPtr &driver) {
    if (active) {
        active = false;
        update();
        // TODO example behaviour
        // Item item = Item::random();
        // driver->addItem(item);
    }
}
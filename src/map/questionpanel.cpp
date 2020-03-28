#include "questionpanel.h"

sf::Image QuestionPanel::assetsActive[];
sf::Image QuestionPanel::assetsInactive[];

void QuestionPanel::loadAssets(const std::string &assetName,
                               sf::IntRect activeRect,
                               sf::IntRect inactiveRect) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)FloorObjectOrientation::__COUNT; i++) {
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
                             const FloorObjectOrientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assetsActive[(int)_orientation].getSize()),
                  Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT, _orientation) {}

void QuestionPanel::applyChanges() const {
    FloorObject::defaultApplyChanges(this);
};

const sf::Image &QuestionPanel::getCurrentImage() const {
    return getState() == FloorObjectState::ACTIVE
               ? assetsActive[(int)orientation]
               : assetsInactive[(int)orientation];
}

MapLand QuestionPanel::getCurrentLand() const {
    return getState() == FloorObjectState::ACTIVE ? MapLand::OTHER
                                                  : MapLand::TRACK;
}

void QuestionPanel::interactWith(const DriverPtr &) {
    if (getState() == FloorObjectState::ACTIVE) {
        std::cerr << "+1 POWER-UP" << std::endl;
        setState(FloorObjectState::INACTIVE);
        // TODO example behaviour
        // Item item = Item::random();
        // driver->addItem(item);
    }
}
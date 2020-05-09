#include "coin.h"

sf::Image Coin::assetsActive[];
sf::Image Coin::assetInactive;

void Coin::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)FloorObjectOrientation::__COUNT; i++) {
        assetsActive[i].create(roi.width, roi.height);
        assetsActive[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
    assetInactive.create(roi.width, roi.height);
    assetInactive.copy(rawImage, 0, 0, roi);
}

Coin::Coin(const sf::Vector2f &topLeftPixels,
           const FloorObjectOrientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assetsActive[(int)_orientation].getSize()),
                  MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, _orientation) {}

void Coin::applyChanges() const { FloorObject::defaultApplyChanges(this); }

const sf::Image &Coin::getCurrentImage() const {
    return getState() == FloorObjectState::ACTIVE
               ? assetsActive[(int)orientation]
               : assetInactive;
}

MapLand Coin::getCurrentLand() const {
    return getState() == FloorObjectState::ACTIVE ? MapLand::OTHER
                                                  : MapLand::TRACK;
}

void Coin::interactWith(const DriverPtr &driver) {
    if (getState() == FloorObjectState::ACTIVE) {
        setState(FloorObjectState::INACTIVE);
        driver->addCoin();
    }
}
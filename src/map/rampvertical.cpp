#include "rampvertical.h"

sf::Image RampVertical::assets[3];

void RampVertical::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < 3; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

RampVertical::RampVertical(const sf::Vector2f &topLeftPixels,
                           const FloorObjectOrientation _orientation,
                           const sf::Vector2f &_size)
    : FloorObject(topLeftPixels, _size, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
                  _orientation) {
    completeAsset.create(_size.x, _size.y);
    completeAsset.copy(assets[0], 0, 0);
    for (int i = 8; i < _size.y - 8; i += 8) {
        completeAsset.copy(assets[1], 0, i);
    }
    completeAsset.copy(assets[2], 0, _size.y - 8);
}

void RampVertical::applyChanges() const {
    FloorObject::defaultApplyChanges(this);
}

const sf::Image &RampVertical::getCurrentImage() const { return completeAsset; }

MapLand RampVertical::getCurrentLand() const { return MapLand::RAMP_VERTICAL; }

// [deprecated]
void RampVertical::interactWith(const DriverPtr &) {
    // Nothing
}
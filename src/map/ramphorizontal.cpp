#include "ramphorizontal.h"

sf::Image RampHorizontal::assets[3];

void RampHorizontal::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < 3; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

RampHorizontal::RampHorizontal(const sf::Vector2f &topLeftPixels,
                               const FloorObjectOrientation _orientation,
                               const sf::Vector2f &_size)
    : FloorObject(topLeftPixels, _size, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
                  _orientation) {
    completeAsset.create(_size.x, _size.y);
    completeAsset.copy(assets[0], 0, 0);
    for (int i = 8; i < _size.x - 8; i += 8) {
        completeAsset.copy(assets[1], i, 0);
    }
    completeAsset.copy(assets[2], _size.x - 8, 0);
}

void RampHorizontal::applyChanges() const {
    FloorObject::defaultApplyChanges(this);
}

const sf::Image &RampHorizontal::getCurrentImage() const {
    return completeAsset;
}

MapLand RampHorizontal::getCurrentLand() const {
    return MapLand::RAMP_HORIZONTAL;
}

// [deprecated]
void RampHorizontal::interactWith(const DriverPtr &) {
    // Nothing
}
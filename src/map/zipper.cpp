#include "zipper.h"

sf::Image Zipper::assets[];

void Zipper::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)FloorObjectOrientation::__COUNT; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

Zipper::Zipper(const sf::Vector2f &topLeftPixels,
               const FloorObjectOrientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assets[(int)_orientation].getSize()),
                  MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, _orientation) {}

void Zipper::applyChanges() const { FloorObject::defaultApplyChanges(this); }

const sf::Image &Zipper::getCurrentImage() const {
    return assets[(int)orientation];
}

MapLand Zipper::getCurrentLand() const { return MapLand::ZIPPER; }

// [[deprecated]]
void Zipper::interactWith(const DriverPtr &driver) {
    driver->position.x += driver->speedForward * cosf(driver->posAngle) * 0.2f;
    driver->position.y += driver->speedForward * sinf(driver->posAngle) * 0.2f;
}
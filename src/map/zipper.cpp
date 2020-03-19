#include "zipper.h"

sf::Image Zipper::assets[];

void Zipper::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)Orientation::__COUNT; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

Zipper::Zipper(const sf::Vector2f &topLeftPixels,
               const Orientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assets[(int)_orientation].getSize()),
                  Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT, _orientation) {}

const sf::Image &Zipper::getCurrentImage() const {
    return assets[(int)orientation];
}

void Zipper::interactWith(const DriverPtr &driver) {
    // TODO cooldown/depend on time?/orientation/etc
    driver->position.x += driver->speedForward * cosf(driver->posAngle) * 0.2f;
    driver->position.y += driver->speedForward * sinf(driver->posAngle) * 0.2f;
}
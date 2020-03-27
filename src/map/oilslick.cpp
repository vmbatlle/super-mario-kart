#include "oilslick.h"

sf::Image OilSlick::assets[];

void OilSlick::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)Orientation::__COUNT; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

OilSlick::OilSlick(const sf::Vector2f &topLeftPixels,
                   const Orientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assets[(int)_orientation].getSize()),
                  Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT, _orientation) {}

void OilSlick::update() const {
    Map::updateAssetCourse(getCurrentImage(), topLeftPixel);
    Map::setLand(sf::Vector2f(hitbox.left, hitbox.top), Map::Land::OIL);
}

const sf::Image &OilSlick::getCurrentImage() const {
    return assets[(int)orientation];
}

// [[deprecated]]
void OilSlick::interactWith(const DriverPtr &driver) {
    // TODO cooldown/depend on time?/etc
    driver->animator.hit();
    driver->posAngle += M_PI / 32.0f;
}
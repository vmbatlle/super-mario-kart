#include "oilslick.h"

sf::Image OilSlick::assets[];

void OilSlick::loadAssets(const std::string &assetName, sf::IntRect roi) {
    sf::Image rawImage;
    rawImage.loadFromFile(assetName);
    for (int i = 0; i < (int)FloorObjectOrientation::__COUNT; i++) {
        assets[i].create(roi.width, roi.height);
        assets[i].copy(rawImage, 0, 0, roi);
        roi.left += roi.width;
    }
}

OilSlick::OilSlick(const sf::Vector2f &topLeftPixels,
                   const FloorObjectOrientation _orientation)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assets[(int)_orientation].getSize()),
                  MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, _orientation) {}

void OilSlick::applyChanges() const { FloorObject::defaultApplyChanges(this); }

const sf::Image &OilSlick::getCurrentImage() const {
    return assets[(int)orientation];
}

MapLand OilSlick::getCurrentLand() const { return MapLand::OIL_SLICK; }

// [[deprecated]]
void OilSlick::interactWith(const DriverPtr &driver) {
    driver->animator.hit();
    driver->posAngle += M_PI / 32.0f;
}
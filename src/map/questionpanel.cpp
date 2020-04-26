#include "questionpanel.h"

sf::Image QuestionPanel::assetsActive[];
sf::Image QuestionPanel::assetsInactive[];

QuestionPanel::ItemArray QuestionPanel::ITEMS_1 = {
    PowerUps::BANANA,      PowerUps::BANANA,      PowerUps::BANANA,
    PowerUps::BANANA,      PowerUps::BANANA,      PowerUps::COIN,
    PowerUps::COIN,        PowerUps::COIN,        PowerUps::COIN,
    PowerUps::GREEN_SHELL, PowerUps::GREEN_SHELL, PowerUps::GREEN_SHELL,
    PowerUps::RED_SHELL,   PowerUps::RED_SHELL,   PowerUps::RED_SHELL,
    PowerUps::MUSHROOM,
};
QuestionPanel::ItemArray QuestionPanel::ITEMS_24 = {
    PowerUps::STAR,      PowerUps::STAR,        PowerUps::THUNDER,
    PowerUps::BANANA,    PowerUps::BANANA,      PowerUps::BANANA,
    PowerUps::MUSHROOM,  PowerUps::MUSHROOM,    PowerUps::MUSHROOM,
    PowerUps::RED_SHELL, PowerUps::RED_SHELL,   PowerUps::RED_SHELL,
    PowerUps::COIN,      PowerUps::GREEN_SHELL, PowerUps::GREEN_SHELL,
    PowerUps::THUNDER,
};
QuestionPanel::ItemArray QuestionPanel::ITEMS_58 = {
    PowerUps::MUSHROOM,  PowerUps::MUSHROOM,    PowerUps::MUSHROOM,
    PowerUps::MUSHROOM,  PowerUps::GREEN_SHELL, PowerUps::GREEN_SHELL,
    PowerUps::RED_SHELL, PowerUps::RED_SHELL,   PowerUps::RED_SHELL,
    PowerUps::STAR,      PowerUps::STAR,        PowerUps::STAR,
    PowerUps::THUNDER,   PowerUps::THUNDER,     PowerUps::BANANA,
    PowerUps::COIN,
};

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
                  MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, _orientation) {}

void QuestionPanel::applyChanges() const {
    FloorObject::defaultApplyChanges(this);
}

const sf::Image &QuestionPanel::getCurrentImage() const {
    return getState() == FloorObjectState::ACTIVE
               ? assetsActive[(int)orientation]
               : assetsInactive[(int)orientation];
}

MapLand QuestionPanel::getCurrentLand() const {
    return getState() == FloorObjectState::ACTIVE ? MapLand::OTHER
                                                  : MapLand::TRACK;
}

void QuestionPanel::interactWith(const DriverPtr &driver) {
    if (getState() == FloorObjectState::ACTIVE &&
        driver->getPowerUp() == PowerUps::NONE) {
        setState(FloorObjectState::INACTIVE);
        int id = rand() % NUM_ITEMS_ARRAY;
        PowerUps item;
        switch (driver->rank) {
            case 0: // first
                item = ITEMS_1[id];
                break;
            case 1: // second to fourth
            case 2:
            case 3:
                item = ITEMS_24[id];
                break;
            default:
                item = ITEMS_58[id];
                break;
        }
        driver->pickUpPowerUp(item);
    }
}
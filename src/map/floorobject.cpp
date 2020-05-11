#include "floorobject.h"

#include "map/map.h"

std::vector<FloorObjectPtr> FloorObject::changesQueue;

FloorObject::FloorObject(const sf::Vector2f &position, const sf::Vector2f &size,
                         const int mapWidth, const int mapHeight,
                         const FloorObjectOrientation _orientation)
    : topLeftPixel(position) {
    float scaleX = 1.0f / mapWidth;
    float scaleY = 1.0f / mapHeight;
    hitbox =
        sf::FloatRect(sf::Vector2f(position.x * scaleX, position.y * scaleY),
                      sf::Vector2f(size.x * scaleX, size.y * scaleY));
    orientation = _orientation;
}

FloorObjectState FloorObject::getState() const { return state; }

void FloorObject::setState(FloorObjectState state) {
    if (state == FloorObjectState::INITIAL) {
        this->state = getInitialState();
    } else {
        this->state = state;
    }
    // set a custom destructor to avoid deletion of the object itself
    FloorObject::changesQueue.push_back(
        FloorObjectPtr(this, [](FloorObject *) {}));
}

bool FloorObject::collidesWith(const DriverPtr &driver) const {
    return hitbox.contains(driver->position);
}

// [[deprecated]]
bool FloorObject::sampleColor(const sf::Vector2f &mapCoordinates,
                              sf::Color &color) const {
    sf::Image image = getCurrentImage();

    float x = (mapCoordinates.x - hitbox.left) / hitbox.width;
    float y = (mapCoordinates.y - hitbox.top) / hitbox.height;
    if (x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f) {
        sf::Vector2u size = image.getSize();
        color = image.getPixel(x * size.x, y * size.y);
        return true;
    } else {
        return false;
    }
}

void FloorObject::resetChanges() { changesQueue.clear(); }

bool FloorObject::applyAllChanges() {
    if (changesQueue.empty()) {
        return false;
    } else {
        for (const FloorObjectPtr &object : FloorObject::changesQueue) {
            object->applyChanges();
        }
        FloorObject::changesQueue.clear();
        return true;
    }
}

void FloorObject::defaultApplyChanges(const FloorObject *that) {
    Map::updateAssetCourse(that->getCurrentImage(), that->topLeftPixel);
    Map::setLand(sf::Vector2f(that->hitbox.left, that->hitbox.top),
                 sf::Vector2f(that->getCurrentImage().getSize()),
                 that->getCurrentLand());
}
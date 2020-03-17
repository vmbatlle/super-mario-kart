#include "floorobject.h"

FloorObject::FloorObject(const sf::Vector2f &position, const sf::Vector2f &size,
                         const int mapWidth, const int mapHeight) {
    float scaleX = 1.0f / mapWidth;
    float scaleY = 1.0f / mapHeight;
    hitbox =
        sf::FloatRect(sf::Vector2f(position.x * scaleX, position.y * scaleY),
                      sf::Vector2f(size.x * scaleX, size.y * scaleY));
}

bool FloorObject::collidesWith(const DriverPtr &driver) const {
    return hitbox.contains(driver->position);
}

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
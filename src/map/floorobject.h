#pragma once

class FloorObject;
#include <memory>
typedef std::shared_ptr<FloorObject> FloorObjectPtr;

#include <SFML/Graphics.hpp>

// Common functionalities for all objects that lie on the floor
// (collision detection, rectangle hitbox, etc.)
class FloorObject {
   protected:
    // all rect's coords (left/top/etc.) should be within 0-1 range
    sf::FloatRect hitbox;

   public:
    // Coords in raw pixels (not 0-1 range)
    FloorObject(const sf::Vector2f &position, const sf::Vector2f &size,
                const int mapWidth, const int mapHeight);

    // collision with point hitbox
    bool collidesWith(const sf::Vector2f &position) const;

    virtual const sf::Image &getCurrentImage() const = 0;
    // returns true if point is inside object, with given color from texture
    bool sampleColor(const sf::Vector2f &mapCoordinates,
                     sf::Color &color) const;
};
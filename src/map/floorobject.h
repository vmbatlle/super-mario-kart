#pragma once

class FloorObject;
#include <memory>
typedef std::shared_ptr<FloorObject> FloorObjectPtr;

#include <SFML/Graphics.hpp>
#include "entities/driver.h"

enum class FloorObjectType : int {
    ZIPPER,
    QUESTION_PANEL,
    OIL_SLICK,
    COIN,
    RAMP_HORIZONTAL,
    RAMP_VERTICAL,
};

enum class Orientation : int { UP, RIGHT, DOWN, LEFT, __COUNT };

// Common functionalities for all objects that lie on the floor
// (collision detection, rectangle hitbox, etc.)
class FloorObject {
   protected:
    // position of the object in image coords
    sf::Vector2f topLeftPixel;
    // all rect's coords (left/top/etc.) should be within 0-1 range
    sf::FloatRect hitbox;
    // facing up/right/etc
    Orientation orientation;

   public:
    // Coords in raw pixels (not 0-1 range)
    FloorObject(const sf::Vector2f &position, const sf::Vector2f &size,
                const int mapWidth, const int mapHeight,
                const Orientation _orientation);

    // updates all needed resources according to its current state
    virtual void update() const = 0;

    // collision with point hitbox
    bool collidesWith(const DriverPtr &driver) const;
    virtual void interactWith(const DriverPtr &driver) = 0;

    virtual const sf::Image &getCurrentImage() const = 0;
    // returns true if point is inside object, with given color from texture
    // [[deprecated]]
    bool sampleColor(const sf::Vector2f &mapCoordinates,
                     sf::Color &color) const;
};
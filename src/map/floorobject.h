#pragma once

class FloorObject;
#include <memory>
typedef std::shared_ptr<FloorObject> FloorObjectPtr;

#include <SFML/Graphics.hpp>

#include "entities/driver.h"
#include "map/enums.h"

// Common functionalities for all objects that lie on the floor
// (collision detection, rectangle hitbox, etc.)
class FloorObject {
   protected:
    // position of the object in image coords
    sf::Vector2f topLeftPixel;
    // all rect's coords (left/top/etc.) should be within 0-1 range
    sf::FloatRect hitbox;
    // facing up/right/etc
    FloorObjectOrientation orientation;
    // internal object state
    FloorObjectState state;

    // static queue of floor objects pending to apply changes
    static std::vector<FloorObjectPtr> changesQueue;

    // Get the value of the object state
    virtual FloorObjectState getInitialState() const = 0;

   public:
    // Coords in raw pixels (not 0-1 range)
    FloorObject(const sf::Vector2f &position, const sf::Vector2f &size,
                const int mapWidth, const int mapHeight,
                const FloorObjectOrientation _orientation);

    // Get the current object state
    FloorObjectState getState() const;

    // Set the current object state.
    // NOTE: User should call `applyAllChanges()` afterwards.
    void setState(FloorObjectState state);

    // updates all needed resources according to its current state
    virtual void applyChanges() const = 0;
    static void defaultApplyChanges(const FloorObject *that);

    // collision with point hitbox
    bool collidesWith(const DriverPtr &driver) const;
    virtual void interactWith(const DriverPtr &driver) = 0;

    virtual const sf::Image &getCurrentImage() const = 0;
    virtual MapLand getCurrentLand() const = 0;
    // returns true if point is inside object, with given color from texture
    // [[deprecated]]
    bool sampleColor(const sf::Vector2f &mapCoordinates,
                     sf::Color &color) const;

    // applies all changes pending in the queue
    static bool applyAllChanges();
    // resets changes queue i.e. new map
    static void resetChanges();
};
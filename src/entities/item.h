#pragma once

#include <memory>
class Item;
typedef std::shared_ptr<Item> ItemPtr;

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "entities/wallobject.h"
#include "map/enums.h"

class Item : public WallObject {
   protected:
    sf::Sprite sprite;

   public:
    bool used;  // true = life of the item ended and should be deleted
    Item(const sf::Vector2f &_position, const float _visualRadius,
         const float _hitboxRadius, const float _height)
        : WallObject(_position, _visualRadius, _hitboxRadius, _height,
                     MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT),
          used(false) {}

    // applies changes to user and generates necesary wallobjects
    static void useItem(const DriverPtr &user, const bool isFront);

    // moves item (doesn't do collision)
    virtual void update(const sf::Time &deltaTime) = 0;
    sf::Sprite &getSprite() { return sprite; }
};
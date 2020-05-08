#include "effectbreak.h"

EffectBreak::EffectBreak(Item *item)
    : Item(sf::Vector2f(item->position.x * MAP_ASSETS_WIDTH,
                        item->position.y * MAP_ASSETS_HEIGHT),
           0.9f,  // less than players' so it goes in front of him
           0.0f, 0.01f),
      verticalSpeed(JUMP_SPEED) {
    sprite.setTexture(*item->getSprite().getTexture());
    sf::Vector2u spriteSize = sprite.getTexture()->getSize();
    sprite.setOrigin(spriteSize.x / 2.0f, spriteSize.y);
}

// moves item (doesn't do collision)
void EffectBreak::update(const sf::Time &deltaTime) {
    verticalSpeed += GRAVITY * deltaTime.asSeconds();
    height += verticalSpeed * deltaTime.asSeconds();
    if (height < 0.0f) {
        used = true;
    }
}

bool EffectBreak::solveCollision(CollisionData &, const sf::Vector2f &,
                                 const sf::Vector2f &, const float, const float,
                                 const bool, const float) {
    // this object doesnt collide with anything
    return false;
}
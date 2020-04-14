#include "banana.h"

sf::Texture Banana::assetBanana;

void Banana::loadAssets(const std::string &assetName, const sf::IntRect &roi) {
    assetBanana.loadFromFile(assetName, roi);
}

Banana::Banana(const sf::Vector2f &_position, const float forwardAngle,
               const bool forwardThrow)
    : Item(sf::Vector2f(0.0f, 0.0f), 0.05f, HITBOX_RADIUS, 1.0f) {
    sf::Vector2f forward =
        sf::Vector2f(cosf(forwardAngle), sinf(forwardAngle)) *
        ((HITBOX_RADIUS + Driver::HITBOX_RADIUS + 0.1f) / MAP_ASSETS_WIDTH);
    forward *= forwardThrow ? 1.0f : -1.0f;
    // move banana a bit forward so it doesn't collide with its own player
    // at this point position hasn't been initialized
    position = _position + forward;
    if (forwardThrow) {
        speed = sf::Vector3f(forward.x * SPEED * 2.5f, forward.y * SPEED * 2.5f,
                             SPEED * 4.0f);
    } else {
        speed = sf::Vector3f(forward.x, forward.y, 0.0f);
    }

    sprite.setTexture(assetBanana);
    sf::Vector2u bananaSize = assetBanana.getSize();
    sprite.setOrigin(bananaSize.x / 2.0f, bananaSize.y);
}

void Banana::update(const sf::Time &deltaTime) {
    if (height == 0.0f) {
        speed = sf::Vector3f(0.0f, 0.0f, 0.0f);
        return;
    }
    position += sf::Vector2f(speed.x, speed.y) * deltaTime.asSeconds();
    height = std::fmaxf(height + speed.z * deltaTime.asSeconds(), 0.0f);
    speed.z += GRAVITY * deltaTime.asSeconds();
    if (height == 0.0f && Map::getLand(position) == MapLand::OUTER) {
        used = true;
        // TODO drown banana
    }
}
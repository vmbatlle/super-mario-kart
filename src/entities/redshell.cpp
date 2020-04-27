#include "redshell.h"

sf::Texture RedShell::assetShell;

void RedShell::loadAssets(const std::string &assetName,
                          const sf::IntRect &roi) {
    assetShell.loadFromFile(assetName, roi);
}

RedShell::RedShell(const sf::Vector2f &_position, const Driver *_target,
                   const float forwardAngle, const bool forwardThrow)
    : Item(sf::Vector2f(0.0f, 0.0f), 0.05f, HITBOX_RADIUS, 0.0f),
      target(_target) {
    inactiveFrames = 10;
    float angle = forwardThrow ? forwardAngle : forwardAngle + M_PI;
    sf::Vector2f forward =
        sf::Vector2f(cosf(angle), sinf(angle)) *
        ((HITBOX_RADIUS + Driver::HITBOX_RADIUS + 0.1f) / MAP_ASSETS_WIDTH);
    // move banana a bit forward so it doesn't collide with its own player
    // at this point position hasn't been initialized
    position = _position + forward;
    speed = forward * SPEED;
    if (!forwardThrow) {
        target = nullptr;
    }

    sprite.setTexture(assetShell);
    sf::Vector2u shellSize = assetShell.getSize();
    sprite.setOrigin(shellSize.x / 2.0f, shellSize.y);
}

void RedShell::update(const sf::Time &deltaTime) {
    if (inactiveFrames > 0) {
        inactiveFrames--;
    }
    if (target == nullptr) {
        position += speed * deltaTime.asSeconds();
    } else {
        sf::Vector2f direction = target->position - position;
        float distance = sqrtf(direction.x * direction.x +
                               direction.y * direction.y + 1e-4f);
        if (distance < 0.08f) {
            position += direction * 0.5f;
        } else {
            direction = sf::Vector2f(0.0f, 0.0f);
            for (int i = 0; i < 3; i++) {
                direction +=
                    AIGradientDescent::getNextDirection(position + direction);
            }
            direction /= sqrtf(direction.x * direction.x +
                               direction.y * direction.y + 1e-2f);
            position += direction * 1.4f * deltaTime.asSeconds();
        }
    }
    MapLand land = Map::getLand(position);
    if (land == MapLand::BLOCK) {
        used = true;
    } else if (land == MapLand::OUTER) {
        used = true;
        // add drown effect sprite on the map
        Map::addEffectDrown(position);
    }
}

bool RedShell::solveCollision(CollisionData &data, const sf::Vector2f &,
                              const sf::Vector2f &, const float, const float) {
    if (used || inactiveFrames > 0) {
        return false;
    }
    data = CollisionData(sf::Vector2f(0.0f, 0.0f), 0.4f, CollisionType::HIT);
    used = true;
    return true;
}
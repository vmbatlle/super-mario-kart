#include "greenshell.h"

sf::Texture GreenShell::assetShell;
const sf::Time GreenShell::TIME_OF_FLIGHT = sf::seconds(1.0f);

void GreenShell::loadAssets(const std::string &assetName,
                            const sf::IntRect &roi) {
    assetShell.loadFromFile(assetName, roi);
}

GreenShell::GreenShell(const sf::Vector2f &_position, const float forwardAngle,
                       const bool forwardThrow)
    : Item(sf::Vector2f(0.0f, 0.0f), 0.05f, HITBOX_RADIUS, 0.0f),
      lives(NUM_LIVES) {
    float angle = forwardThrow ? forwardAngle : forwardAngle + M_PI;
    sf::Vector2f forward =
        sf::Vector2f(cosf(angle), sinf(angle)) *
        ((HITBOX_RADIUS + Driver::HITBOX_RADIUS + 0.1f) / MAP_ASSETS_WIDTH);
    // move banana a bit forward so it doesn't collide with its own player
    // at this point position hasn't been initialized
    position = _position + forward;
    speed = forward * SPEED;

    sprite.setTexture(assetShell);
    sf::Vector2u shellSize = assetShell.getSize();
    sprite.setOrigin(shellSize.x / 2.0f, shellSize.y);
}

void GreenShell::update(const sf::Time &deltaTime) {
    sf::Vector2f oldPosition = position;
    position += speed * deltaTime.asSeconds();

    if (flightRemainingTime > sf::Time::Zero) {
        flightRemainingTime -= deltaTime;
    }

    MapLand land = Map::getLand(position);
    if (land == MapLand::BLOCK && lives <= 0) {
        if (flightRemainingTime <= sf::Time::Zero) {
            used = true;
            // add break effect sprite on the map
            Map::addEffectBreak(this);
        }
    } else if (land == MapLand::BLOCK) {
        // reflect shell
        // detect direction of hit
        sf::Vector2f delta = (position - oldPosition) / (float)NUM_MARCHES;
        sf::Vector2f block = oldPosition + delta;
        while (Map::getLand(block) != MapLand::BLOCK) {
            block += delta;
        }
        sf::Vector2f blockCenter((int)(block.x * MAP_TILES_WIDTH),
                                    (int)(block.y * MAP_TILES_HEIGHT));
        blockCenter =
            sf::Vector2f((blockCenter.x + 0.5f) / MAP_TILES_WIDTH,
                            (blockCenter.y + 0.5f) / MAP_TILES_HEIGHT);
        if (fabsf(blockCenter.x - oldPosition.x) >
            fabsf(blockCenter.y - oldPosition.y)) {
            // hit an horizontal side
            speed.x *= -1.0f;
        } else {
            // vertical side
            speed.y *= -1.0f;
        }
        position = oldPosition;
        lives--;
    } else if (land == MapLand::RAMP_HORIZONTAL ||
               land == MapLand::RAMP_VERTICAL) {
        if (flightRemainingTime <= sf::Time::Zero) {
            flightRemainingTime = TIME_OF_FLIGHT;
        }
    } else if (land == MapLand::OUTER) {
        if (flightRemainingTime <= sf::Time::Zero) {
            used = true;
            // add drown effect sprite on the map
            Map::addEffectDrown(position);
        }
    } else if (position.x < 0.0f || position.x > 1.0f || position.y < 0.0f ||
               position.y > 1.0f) {
        used = true;
    }

    if (flightRemainingTime > sf::Time::Zero) {
        height = MAX_HEIGHT * (flightRemainingTime / TIME_OF_FLIGHT - 0.5);
        height = height < 0.0f ? height + (MAX_HEIGHT / 2.0f) : height;
    }
}

bool GreenShell::solveCollision(CollisionData &data, const sf::Vector2f &,
                                const sf::Vector2f &, const float,
                                const float otherHeight,
                                const bool otherIsImmune, const float) {
    if (WallObject::collisionHasHeightDifference(height, otherHeight)) {
        return false;
    }
    if (used) {
        return false;
    }
    if (otherIsImmune) {
        // other is in star-state, for example
        used = true;
        // add break effect sprite on the map
        Map::addEffectBreak(this);
        data = CollisionData(sf::Vector2f(0.0f, 0.0f), 1.0f,
                             CollisionType::NO_HIT);
        return true;
    }
    data = CollisionData(sf::Vector2f(0.0f, 0.0f), 0.4f, CollisionType::HIT);
    used = true;
    // add break effect sprite on the map
    Map::addEffectBreak(this);
    return true;
}
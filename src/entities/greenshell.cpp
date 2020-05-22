#include "greenshell.h"

sf::Texture GreenShell::assetShell;
const sf::Time GreenShell::TIME_OF_FLIGHT = sf::seconds(1.0f);

void GreenShell::loadAssets(const std::string &assetName,
                            const sf::IntRect &roi) {
    assetShell.loadFromFile(assetName, roi);
}

GreenShell::GreenShell(const sf::Vector2f &_position, const float forwardAngle,
                       const bool forwardThrow, const float playerHeight)
    : Item(sf::Vector2f(0.0f, 0.0f), 0.05f, HITBOX_RADIUS, playerHeight),
      lives(NUM_LIVES) {
    float angle = forwardThrow ? forwardAngle : forwardAngle + M_PI;
    sf::Vector2f forward =
        sf::Vector2f(cosf(angle), sinf(angle)) *
        ((HITBOX_RADIUS + Driver::HITBOX_RADIUS + 0.1f) / MAP_ASSETS_WIDTH);
    // move banana a bit forward so it doesn't collide with its own player
    // at this point position hasn't been initialized
    position = _position + forward;
    speed = forward * SPEED;
    verticalSpeed = 0.0f;

    sprite.setTexture(assetShell);
    sf::Vector2u shellSize = assetShell.getSize();
    sprite.setOrigin(shellSize.x / 2.0f, shellSize.y);
}

void GreenShell::update(const sf::Time &deltaTime) {
    for (unsigned int i = 0; i < NUM_MARCHES_UPDATE; i++){
        marchingUpdate(deltaTime / (float)NUM_MARCHES_UPDATE);
    }
}

void GreenShell::marchingUpdate(const sf::Time &deltaTime) {

    sf::Vector2f oldPosition = position;
    position += speed * deltaTime.asSeconds();
    verticalSpeed += GRAVITY * deltaTime.asSeconds();
    height = fmaxf(0.0f, height + verticalSpeed * deltaTime.asSeconds());

    if (position.x < 0.0f || position.x > 1.0f || position.y < 0.0f ||
        position.y > 1.0f) {
        used = true;
    } else if (height > 0.0f) {
        return;
    }

    MapLand land = Map::getLand(position);
    if (land == MapLand::BLOCK && lives <= 0) {
        used = true;
        // add break effect sprite on the map
        Map::addEffectBreak(this);
    } else if (land == MapLand::BLOCK) {
        // reflect shell
        // detect direction of hit
        sf::Vector2f delta = (position - oldPosition) / (float)NUM_MARCHES_HIT;
        sf::Vector2f block = oldPosition + delta;
        while (Map::getLand(block) != MapLand::BLOCK) {
            block += delta;
        }
        sf::Vector2f blockCenter((int)(block.x * MAP_TILES_WIDTH),
                                 (int)(block.y * MAP_TILES_HEIGHT));
        blockCenter = sf::Vector2f((blockCenter.x + 0.5f) / MAP_TILES_WIDTH,
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
        verticalSpeed = JUMP_SPEED;
    } else if (land == MapLand::OUTER) {
        used = true;
        // add drown effect sprite on the map
        Map::addEffectDrown(position);
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
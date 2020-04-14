#include "greenshell.h"

sf::Texture GreenShell::assetShell;

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
    MapLand land = Map::getLand(position);
    if (land == MapLand::BLOCK && lives <= 0) {
        used = true;
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
    } else if (land == MapLand::OUTER) {
        used = true;
        // TODO drown shell
    }
}

bool GreenShell::solveCollision(CollisionData &data, const sf::Vector2f &,
                                const sf::Vector2f &, const float,
                                const float) {
    data = CollisionData(sf::Vector2f(0.0f, 0.0f), 0.6f, CollisionType::HIT);
    used = true;
    return true;
}
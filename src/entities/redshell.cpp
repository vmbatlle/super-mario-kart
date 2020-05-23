#include "redshell.h"

sf::Texture RedShell::assetShell;
const sf::Time RedShell::TIME_OF_FLIGHT = sf::seconds(0.5f);
unsigned int RedShell::numRedShellsFollowingPlayer = 0u;

void RedShell::loadAssets(const std::string &assetName,
                          const sf::IntRect &roi) {
    assetShell.loadFromFile(assetName, roi);
}

RedShell::RedShell(const sf::Vector2f &_position, const Driver *_target,
                   const float forwardAngle, const bool forwardThrow,
                   const float playerHeight)
    : Item(sf::Vector2f(0.0f, 0.0f), 0.05f, HITBOX_RADIUS, playerHeight),
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

    // init values depending on player height
    if (playerHeight == 0.0f) {
        flightRemainingTime = sf::seconds(0.0f);
        lastDirection = sf::Vector2f(0.0f, 0.0f);
        gradientWhenRamp = -1;
    } else {
        flightRemainingTime = TIME_OF_FLIGHT;
        lastDirection = sf::Vector2f(cosf(angle), sinf(angle)) * 0.3f;
        gradientWhenRamp = AIGradientDescent::getPositionValue(position);
    }

    // Sound
    followingPlayer =
        target != nullptr && target->controlType == DriverControlType::PLAYER;
    if (followingPlayer) {
        numRedShellsFollowingPlayer++;
        Audio::play(SFX::CIRCUIT_ITEM_RED_SHELL, true);
    }

    sprite.setTexture(assetShell);
    sf::Vector2u shellSize = assetShell.getSize();
    sprite.setOrigin(shellSize.x / 2.0f, shellSize.y);
}

RedShell::~RedShell() {
    if (followingPlayer && --numRedShellsFollowingPlayer == 0) {
        Audio::stop(SFX::CIRCUIT_ITEM_RED_SHELL);
    }
}

void RedShell::update(const sf::Time &deltaTime) {
    if (inactiveFrames > 0) {
        inactiveFrames--;
    }
    if (target == nullptr) {
        position += speed * deltaTime.asSeconds();
    } else {
        sf::Vector2f direction = target->position - position;
        float distance = sqrtf(fmaxf(
            1e-12f, direction.x * direction.x + direction.y * direction.y));
        if (distance < 0.08f) {
            position += direction * 0.5f;
        } else {
            direction = sf::Vector2f(0.0f, 0.0f);
            if (flightRemainingTime <= sf::Time::Zero) {
                for (int i = 0; i < 3; i++) {
                    MapLand land = Map::getLand(position + direction);
                    if (i > 0 && land != MapLand::TRACK) {
                        break;
                    }
                    direction += AIGradientDescent::getNextDirection(position +
                                                                     direction);
                }
                direction /=
                    sqrtf(fmaxf(1e-12f, direction.x * direction.x +
                                            direction.y * direction.y));
                direction *= 0.3f;
                lastDirection = direction;
            } else {
                direction = lastDirection;
            }
            position += direction * 1.4f * deltaTime.asSeconds();
        }
    }

    if (flightRemainingTime > sf::Time::Zero) {
        int currentGradient = AIGradientDescent::getPositionValue(position);
        if ((currentGradient >= 0.0f && currentGradient <= gradientWhenRamp) ||
            flightRemainingTime > TIME_OF_FLIGHT / 2.0f) {
            flightRemainingTime -= deltaTime;
        }
    }

    MapLand land = Map::getLand(position);
    if (land == MapLand::BLOCK) {
        if (flightRemainingTime <= sf::Time::Zero) {
            used = true;
            // add break effect sprite on the map
            Map::addEffectBreak(this);
        }
    } else if (land == MapLand::RAMP_HORIZONTAL ||
               land == MapLand::RAMP_VERTICAL) {
        if (flightRemainingTime <= sf::Time::Zero) {
            flightRemainingTime = TIME_OF_FLIGHT;
            gradientWhenRamp = AIGradientDescent::getPositionValue(position);
            if (land == MapLand::RAMP_HORIZONTAL) {
                lastDirection.x = 0.0f;
                lastDirection.y = lastDirection.y < 0.0f ? -0.3f : 0.3f;
            } else {
                lastDirection.x = lastDirection.x < 0.0f ? -0.3f : 0.3f;
                lastDirection.y = 0.0f;
            }
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
        // Increase and decrease
        height = MAX_HEIGHT *
                 (0.5 - fabs(flightRemainingTime / TIME_OF_FLIGHT - 0.5));
    }
}

bool RedShell::solveCollision(CollisionData &data, const sf::Vector2f &,
                              const sf::Vector2f &, const float,
                              const float otherHeight, const bool otherIsImmune,
                              const float) {
    if (WallObject::collisionHasHeightDifference(height, otherHeight)) {
        return false;
    }
    if (used || inactiveFrames > 0) {
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
    if (target != nullptr && target->controlType == DriverControlType::PLAYER &&
        used) {
        Audio::stop(SFX::CIRCUIT_ITEM_RED_SHELL);
    }

    return true;
}
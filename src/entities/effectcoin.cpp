#include "effectcoin.h"

std::array<sf::Texture, EffectCoin::NUM_TEXTURES> EffectCoin::assetTextures;

const sf::Time EffectCoin::TIME_BETWEEN_FRAMES = sf::seconds(0.05f);

void EffectCoin::loadAssets(const std::string &assetName,
                            const sf::IntRect &roi0, const sf::IntRect &roi1,
                            const sf::IntRect &roi2) {
    assetTextures[0].loadFromFile(assetName, roi0);
    assetTextures[1].loadFromFile(assetName, roi1);
    assetTextures[2].loadFromFile(assetName, roi2);
    sf::Image img3;
    img3.loadFromFile(assetName);
    img3.flipHorizontally();
    assetTextures[3].loadFromImage(img3, roi1);
}

EffectCoin::EffectCoin(const Driver *_driver, const sf::Time &_delay,
                       const bool _positive)
    : Item(sf::Vector2f(_driver->position.x * MAP_ASSETS_WIDTH,
                        _driver->position.y * MAP_ASSETS_HEIGHT),
           -0.5f, 0.0f, _driver->height),
      driver(_driver),
      relativeHeight(2.0f),
      delay(_delay),
      positive(_positive),
      verticalSpeed(JUMP_SPEED),
      speedForward(0.05f + _driver->speedForward * 2.0f),
      posAngle(_driver->posAngle),
      currentTime(sf::Time::Zero),
      currentFrame(0),
      once(true) {}

void EffectCoin::setTexture(const int frame) {
    sprite = sf::Sprite(assetTextures[frame]);
    sf::Vector2u spriteSize = assetTextures[frame].getSize();
    sprite.setOrigin(spriteSize.x / 2.0f, spriteSize.y);
}

// moves item (doesn't do collision)
void EffectCoin::update(const sf::Time &deltaTime) {
    currentTime += deltaTime;
    if (currentTime < delay) {
        // invisible
        return;
    } else {
        // visible
        if (delay > sf::Time::Zero) {
            currentTime = sf::Time::Zero;
            delay = sf::Time::Zero;
            setTexture(0);
        }
    }

    // height
    float mult = 1.0f;
    if (positive) {
        position = driver->position;
    } else {
        position += sf::Vector2f(cosf(posAngle), sinf(posAngle)) *
                    speedForward * deltaTime.asSeconds();
        mult += 0.5f;
    }
    verticalSpeed += GRAVITY * deltaTime.asSeconds() * mult;
    relativeHeight += verticalSpeed * deltaTime.asSeconds();
    height = driver->height + relativeHeight;
    if (relativeHeight < 0.0f) {
        used = true;
    }

    // textures
    if (currentTime > TIME_BETWEEN_FRAMES) {
        currentFrame = (currentFrame + 1) % NUM_TEXTURES;
        currentTime = sf::Time::Zero;
        if (currentFrame >= NUM_TEXTURES) {
            used = true;
        } else {
            setTexture(currentFrame);
        }
    }

    if (once) {
        once = false;
        if (driver->controlType == DriverControlType::PLAYER) {
            Audio::play(SFX::CIRCUIT_COIN);
        }
    }
}

bool EffectCoin::solveCollision(CollisionData &, const sf::Vector2f &,
                                const sf::Vector2f &, const float, const float,
                                const bool, const float) {
    // this object doesnt collide with anything
    return false;
}
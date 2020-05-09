#include "effectdrown.h"

std::array<sf::Texture, EffectDrown::NUM_TEXTURES> EffectDrown::assetTextures;

const sf::Time EffectDrown::TIME_BETWEEN_FRAMES = sf::seconds(0.2f);

void EffectDrown::loadAssets(const std::string &assetName,
                             const sf::IntRect &roi0, const sf::IntRect &roi1,
                             const sf::IntRect &roi2, const sf::IntRect &roi3,
                             const sf::IntRect &roi4) {
    assetTextures[0].loadFromFile(assetName, roi0);
    assetTextures[1].loadFromFile(assetName, roi1);
    assetTextures[2].loadFromFile(assetName, roi2);
    assetTextures[3].loadFromFile(assetName, roi3);
    assetTextures[4].loadFromFile(assetName, roi4);
}

void EffectDrown::setTexture(const int frame) {
    sprite = sf::Sprite(assetTextures[frame]);
    sf::Vector2u spriteSize = assetTextures[frame].getSize();
    if (frame < 3) {
        sprite.setOrigin(spriteSize.x / 2.0f, spriteSize.y);
    } else {
        sprite.setOrigin(spriteSize.x / 2.0f, spriteSize.y / 2.0f);
    }
    sprite.setColor(tint);
}

// moves item (doesn't do collision)
void EffectDrown::update(const sf::Time &deltaTime) {
    currentTime += deltaTime;
    if (currentTime > TIME_BETWEEN_FRAMES) {
        currentFrame++;
        currentTime = sf::Time::Zero;
        if (currentFrame >= NUM_TEXTURES) {
            used = true;
        } else {
            setTexture(currentFrame);
        }
    }
}

bool EffectDrown::solveCollision(CollisionData &, const sf::Vector2f &,
                                 const sf::Vector2f &, const float, const float,
                                 const bool, const float) {
    // this object doesnt collide with anything
    return false;
}
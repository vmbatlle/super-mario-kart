#include "effectsparkles.h"

std::array<sf::Texture, EffectSparkles::NUM_TEXTURES>
    EffectSparkles::assetTextures;

const sf::Time EffectSparkles::TIME_BETWEEN_FRAMES = sf::seconds(0.04f);

void EffectSparkles::loadAssets(const std::string &assetName,
                                const sf::IntRect &roi0,
                                const sf::IntRect &roi1,
                                const sf::IntRect &roi2,
                                const sf::IntRect &roi3) {
    assetTextures[0].loadFromFile(assetName, roi0);
    assetTextures[1].loadFromFile(assetName, roi1);
    assetTextures[2].loadFromFile(assetName, roi2);
    assetTextures[3].loadFromFile(assetName, roi3);
}

void EffectSparkles::setTexture(const int frame) {
    sprite = sf::Sprite(assetTextures[frame]);
    sf::Vector2u spriteSize = assetTextures[frame].getSize();
    sprite.setOrigin(spriteSize.x / 2.0f, spriteSize.y / 1.5f);
}

// moves item (doesn't do collision)
void EffectSparkles::update(const sf::Time &deltaTime) {
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

bool EffectSparkles::solveCollision(CollisionData &, const sf::Vector2f &,
                                    const sf::Vector2f &, const float,
                                    const float, const bool, const float) {
    // this object doesnt collide with anything
    return false;
}
#include "floatingfish.h"

sf::Image FloatingFish::assetFishImage;
sf::IntRect FloatingFish::roi1L, FloatingFish::roi2L, FloatingFish::roi1R,
    FloatingFish::roi2R;

void FloatingFish::loadAssets(const std::string &assetName,
                              const sf::IntRect &_roi1L,
                              const sf::IntRect &_roi2L,
                              const sf::IntRect &_roi1R,
                              const sf::IntRect &_roi2R) {
    assetFishImage.loadFromFile(assetName);
    roi1L = _roi1L;
    roi2L = _roi2L;
    roi1R = _roi1R;
    roi2R = _roi2R;
}

FloatingFish::FloatingFish(const sf::Vector2f &position,
                           const sf::Vector2f &_speed, const bool _flipped)
    : Item(position, 0.0f, 0.0f, 0.0f), speed(_speed), flipped(_flipped) {
    if (_flipped) {
        assetFishImage.flipHorizontally();
        assetFishTextures[0].loadFromImage(assetFishImage, roi1R);
        assetFishTextures[1].loadFromImage(assetFishImage, roi2R);
        assetFishImage.flipHorizontally();
    } else {
        assetFishTextures[0].loadFromImage(assetFishImage, roi1L);
        assetFishTextures[1].loadFromImage(assetFishImage, roi2L);
    }
    sprite = sf::Sprite(assetFishTextures[0]);
    sf::Vector2u size = sprite.getTexture()->getSize();
    sprite.setOrigin(size.x / 2.0f, size.y);
    usingFirstTexture = true;
    totalTime = sf::Time::Zero;
}

void FloatingFish::update(const sf::Time &deltaTime) {
    totalTime += deltaTime;
    position += speed * deltaTime.asSeconds();
    height = sinf(totalTime.asSeconds()) * 6.0f;
    if (deltaTime > sf::seconds(0.4f)) {
        usingFirstTexture = !usingFirstTexture;
        sprite.setTexture(assetFishTextures[usingFirstTexture ? 0 : 1]);
        sf::Vector2u size = sprite.getTexture()->getSize();
        sprite.setOrigin(size.x / 2.0f, size.y);
    }
}

#include "podium.h"

sf::Texture Podium::assetPodium;

void Podium::loadAssets(const std::string &assetName, const sf::IntRect &roi) {
    assetPodium.loadFromFile(assetName, roi);
}

Podium::Podium(const sf::Vector2f &position)
    : Item(position, 0.0f, 0.0f, 0.0f) {
    sprite = sf::Sprite(assetPodium);
    sf::Vector2u size = sprite.getTexture()->getSize();
    sprite.setOrigin(size.x / 2.0f, size.y * 0.9f);
}

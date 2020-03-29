#include "pipe.h"

sf::Texture Pipe::assetGreenTexture;
sf::Texture Pipe::assetOrangeTexture;

void Pipe::loadAssets(const std::string &assetName, const sf::IntRect &roiGreen,
                      const sf::IntRect &roiOrange) {
    assetGreenTexture.loadFromFile(assetName, roiGreen);
    assetOrangeTexture.loadFromFile(assetName, roiOrange);
}

Pipe::Pipe(const sf::Vector2f &position, bool greenNotOrange)
    : WallObject(position, 2.5f, 0.0f, Map::ASSETS_WIDTH, Map::ASSETS_HEIGHT),
      sprite(greenNotOrange ? assetGreenTexture : assetOrangeTexture) {
    sf::Vector2u size = sprite.getTexture()->getSize();
    sprite.setOrigin(size.x / 2.0f, size.y);
}
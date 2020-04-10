#include "thwomp.h"

sf::Texture Thwomp::assetNormal;
sf::Texture Thwomp::assetSuper;

void Thwomp::loadAssets(const std::string &assetName,
                        const sf::IntRect &roiNormal,
                        const sf::IntRect &roiSuper) {
    assetNormal.loadFromFile(assetName, roiNormal);
    assetSuper.loadFromFile(assetName, roiSuper);
}

Thwomp::Thwomp(const sf::Vector2f &position, bool _isSuper)
    : WallObject(position, 2.5f, 3.0f, MAX_HEIGHT, MAP_ASSETS_WIDTH,
                 MAP_ASSETS_HEIGHT),
      sprite(_isSuper ? assetSuper : assetNormal),
      currentState(State::UP),
      currentTime(0.0f),
      isSuper(_isSuper) {
    sf::Vector2u size = sprite.getTexture()->getSize();
    sprite.setOrigin(size.x / 2.0f, size.y);
}

void Thwomp::update(const sf::Time &deltaTime) {
    currentTime += deltaTime.asSeconds();
    switch (currentState) {
        case State::UP:
            if (currentTime >= 0.0f) {
                currentState = State::GOING_DOWN;
                currentTime = 0.0f;
            }
            break;
        case State::GOING_DOWN:
            currentTime = fminf(currentTime, 0.25f);
            height = (0.25f - currentTime) * 4.0f * MAX_HEIGHT;
            if (currentTime == 0.25f) {
                currentState = State::DOWN;
                currentTime = 0.0f;
            }
            break;
        case State::DOWN:
            currentTime = fminf(currentTime, 5.0f);
            if (currentTime == 5.0f) {
                currentState = State::GOING_UP;
                currentTime = 0.0f;
            }
            break;
        case State::GOING_UP:
            currentTime = fminf(currentTime, 2.0f);
            height = currentTime * MAX_HEIGHT / 2.0f;
            if (currentTime == 2.0f) {
                currentState = State::UP;
                currentTime = ((rand() % 6) + 3) * -1;
                if (isSuper) {
                    currentTime *= 0.6f;
                }
            }
            break;
    }
}
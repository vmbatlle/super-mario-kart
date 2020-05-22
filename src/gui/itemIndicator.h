#pragma once

#include <SFML/Graphics.hpp>

#include "entities/enums.h"

class ItemIndicator {
   public:
    sf::Texture items[8];  // 0 = none
    sf::Sprite indicator;

    sf::Vector2u rightUpCorner;
    sf::Vector2u winSize;

    int selectedItem, selectedFinalItem;
    float timeAcc;
    bool spinning;

    sf::Vector2f scaleFactor;

    ItemIndicator();

    void setPosition(sf::Vector2u winSize, sf::Vector2f position);
    void setItem(PowerUps id);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);

    void reset();
};
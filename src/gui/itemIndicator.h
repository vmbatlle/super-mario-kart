#pragma once

#include <SFML/Graphics.hpp>

class ItemIndicator {
    public:
    sf::Texture items[8]; // 0 = none
    sf::Sprite indicator;

    sf::Vector2u rightUpCorner;
    sf::Vector2u winSize;

    int selectedItem, selectedFinalItem;
    float timeAcc;
    bool spinning;
    
    sf::Vector2f scaleFactor;

    ItemIndicator();

    void setPosition(sf::Vector2f position);
    void setItem(int id);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};
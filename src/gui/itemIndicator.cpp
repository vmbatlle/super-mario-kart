#include "itemIndicator.h"

#include "audio/audio.h"

ItemIndicator::ItemIndicator() {
    std::string spriteFile = "assets/gui/items.png";
    for (int i = 0; i < 8; i++)
        items[i].loadFromFile(spriteFile, sf::IntRect(0 + (i * 27), 0, 26, 18));

    scaleFactor = sf::Vector2f(2, 2);

    selectedItem = 0;
    selectedFinalItem = 0;
    timeAcc = 0;
    spinning = false;

    indicator.setTexture(items[selectedItem]);
    indicator.setPosition(0, 0);
    indicator.scale(scaleFactor);
    indicator.setOrigin(indicator.getLocalBounds().width / 2,
                        indicator.getLocalBounds().height / 2);
}

void ItemIndicator::setPosition(sf::Vector2u winSize, sf::Vector2f position) {
    float xFactor = winSize.x / BASIC_WIDTH;
    indicator.setScale(scaleFactor.x * xFactor, scaleFactor.y * xFactor);

    position = sf::Vector2f(position.x - indicator.getGlobalBounds().width / 2 -
                                5 * scaleFactor.y * xFactor,
                            position.y);

    indicator.setPosition(position);
}

void ItemIndicator::setItem(PowerUps id) {
    if (id != PowerUps::NONE) {
        spinning = true;
        selectedItem = 0;
        selectedFinalItem = (int)id % 8;
    } else {
        selectedItem = 0;
        selectedFinalItem = 0;
        indicator.setTexture(items[selectedFinalItem]);
    }
}

void ItemIndicator::update(const sf::Time &deltaTime) {
    if (spinning) {
        timeAcc += deltaTime.asSeconds();
        selectedItem = ((selectedItem + 1) % 7) + 1;
        indicator.setTexture(items[selectedItem]);

        if (timeAcc > 2) {
            spinning = false;
            Audio::play(SFX::CIRCUIT_ITEM_GET);
            indicator.setTexture(items[selectedFinalItem]);
            selectedFinalItem = 0;
            selectedItem = 0;
            timeAcc = 0;
        }
    }
}

void ItemIndicator::draw(sf::RenderTarget &window) { window.draw(indicator); }

void ItemIndicator::reset() {
    selectedFinalItem = 0;
    selectedItem = 0;
    indicator.setTexture(items[selectedFinalItem]);
}
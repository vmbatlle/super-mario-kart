#include "textutils.h"

TextUtils TextUtils::instance;

#include <iostream>

void TextUtils::loadAssets(const std::string &assetName,
                           const sf::Vector2i &startFlat,
                           const sf::Vector2i &startShadow) {
    static const sf::Vector2i size(8, 8);
    sf::Image raw;
    raw.loadFromFile(assetName);
    sf::Vector2i posFlat = startFlat;
    sf::Vector2i posShadow = startShadow;
    for (int i = 1; i < NUM_CHARS; i++) {
        instance.charactersFlat[i - 1].loadFromImage(raw,
                                                 sf::IntRect(posFlat, size));
        instance.charactersShadow[i - 1].loadFromImage(
            raw, sf::IntRect(posShadow, size));
        posFlat += sf::Vector2i(size.x + 1, 0);
        posShadow += sf::Vector2i(size.x + 1, 0);
        if (i % 13 == 0 && i < 27) {
            int movement = (size.y + 1) * i / 13;
            posFlat = startFlat + sf::Vector2i(0, movement);
            posShadow = startShadow + sf::Vector2i(0, movement);
        }
    }
}

const sf::Texture &TextUtils::getChar(const char c, const bool useFlatFont) {
    int i;
    if (c >= 'a' && c <= 'z') {
        i = c - 'a';
    } else if (c >= '0' && c <= '9') {
        i = 25 + c - '0';
    } else {
        switch (c) {
            case '.':
                i = 36;
                break;
            case '(':
                i = 37;
                break;
            case ')':
                i = 38;
                break;
            case '?':
                i = 39;
                break;
            case '!':
                i = 40;
                break;
            case '\'':
                i = 41;
                break;
            case '"':
                i = 42;
                break;
            case '-':
                i = 43;
                break;
            default:
                i = 39;  // ?
                break;
        }
    }
    return useFlatFont ? instance.charactersFlat[i]
                       : instance.charactersShadow[i];
}

void TextUtils::write(sf::RenderTarget &window, const std::string &text,
                      sf::Vector2f position, const float scale,
                      const sf::Color &color, const bool useFlatFont) {
    static const sf::Vector2f delta(
        instance.charactersFlat[0].getSize().x + 1.0f, 0.0f);
    for (const char c : text) {
        if (c != ' ') {
            sf::Sprite sprite(getChar(c, useFlatFont));
            sprite.setPosition(sf::Vector2f(position));
            sprite.setScale(scale, scale);
            sprite.setColor(color);
            window.draw(sprite);
        }
        position += delta * scale;
    }
}
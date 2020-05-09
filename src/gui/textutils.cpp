#include "textutils.h"

TextUtils TextUtils::instance;

#include <iostream>

void TextUtils::loadAssets(const std::string &assetName,
                           const std::string &alphaName,
                           const sf::Vector2i &startFlat,
                           const sf::Vector2i &startShadow) {
    static const sf::Vector2i size(8, 8);
    sf::Image raw, alpha;
    raw.loadFromFile(assetName);
    alpha.loadFromFile(alphaName);
    sf::Vector2i posFlat = startFlat;
    sf::Vector2i posShadow = startShadow;
    for (int i = 1; i <= NUM_CHARS; i++) {
        instance.charactersFlat[i - 1].loadFromImage(
            raw, sf::IntRect(posFlat, size));
        instance.charactersFlatAlpha[i - 1].loadFromImage(
            alpha, sf::IntRect(posFlat, size));
        instance.charactersShadow[i - 1].loadFromImage(
            raw, sf::IntRect(posShadow, size));
        instance.charactersShadowAlpha[i - 1].loadFromImage(
            alpha, sf::IntRect(posShadow, size));
        posFlat += sf::Vector2i(size.x + 1, 0);
        posShadow += sf::Vector2i(size.x + 1, 0);
        if (i % 13 == 0 && i < 27) {
            int movement = (size.y + 1) * i / 13;
            posFlat = startFlat + sf::Vector2i(0, movement);
            posShadow = startShadow + sf::Vector2i(0, movement);
        }
    }
}

int getCharIndex(char c) {
    int i;
    if (c >= 'a' && c <= 'z') {
        i = c - 'a';
    } else if (c >= '0' && c <= '9') {
        i = 26 + c - '0';
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
            case '>':
                i = 44;
                break;
            case '<':
                i = 45;
                break;
            default:
                i = 39;  // ?
                break;
        }
    }
    return i;
}
const sf::Texture &TextUtils::getChar(const char c, const bool useFlatFont) {
    int i = getCharIndex(c);
    return useFlatFont ? instance.charactersFlat[i]
                       : instance.charactersShadow[i];
}

const sf::Texture &TextUtils::getCharAlpha(const char c,
                                           const bool useFlatFont) {
    int i = getCharIndex(c);
    return useFlatFont ? instance.charactersFlatAlpha[i]
                       : instance.charactersShadowAlpha[i];
}

void TextUtils::write(sf::RenderTarget &window, const std::string &text,
                      sf::Vector2f position, const float scale,
                      const sf::Color &color, const bool useFlatFont,
                      const TextAlign align, const TextVerticalAlign alignV) {
    static const sf::Vector2f delta(
        instance.charactersFlat[0].getSize().x + 1.0f, 0.0f);

    float textWidth = delta.x * scale * text.size();
    float textHeight = instance.charactersFlat[0].getSize().y * scale;
    if (align == TextAlign::LEFT) {
        // nothing
    } else if (align == TextAlign::CENTER) {
        position.x -= textWidth / 2.0f;
    } else if (align == TextAlign::RIGHT) {
        position.x -= textWidth;
    }

    if (alignV == TextVerticalAlign::TOP) {
        // nothing
    } else if (alignV == TextVerticalAlign::MIDDLE) {
        position.y -= textHeight / 2.0f;
    } else if (alignV == TextVerticalAlign::BOTTOM) {
        position.y -= textHeight;
    }

    for (const char c : text) {
        if (c != ' ') {
            sf::Sprite sprite(getChar(c, useFlatFont));
            sprite.setPosition(sf::Vector2f(position));
            sprite.setScale(scale, scale);
            window.draw(sprite);
            sprite.setTexture(getCharAlpha(c, useFlatFont));
            sprite.setColor(color);
            window.draw(sprite);
        }
        position += delta * scale;
    }
}
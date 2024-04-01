#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <array>

namespace Color {
static const sf::Color Default(81, 142, 225);
static const sf::Color MenuPrimary(40, 71, 112);
static const sf::Color MenuPrimaryOnFocus(0, 105, 255);
static const sf::Color MenuSecondary = sf::Color(214, 0, 214);
static const sf::Color MenuActive = sf::Color::Green;
}  // namespace Color

class TextUtils {
   public:
    enum class TextAlign : int {
        CENTER,
        LEFT,
        RIGHT,
    };
    enum class TextVerticalAlign : int {
        TOP,
        MIDDLE,
        BOTTOM,
    };
    static constexpr const int CHAR_SIZE = 8;

   private:
    static constexpr const int NUM_CHARS = 46;
    std::array<sf::Texture, NUM_CHARS> charactersFlat, charactersShadow;
    std::array<sf::Texture, NUM_CHARS> charactersFlatAlpha,
        charactersShadowAlpha;
    static TextUtils instance;

    static const sf::Texture &getChar(const char c, const bool useFlatFont);
    static const sf::Texture &getCharAlpha(const char c,
                                           const bool useFlatFont);
    TextUtils() {}

   public:
    static void loadAssets(const std::string &assetName,
                           const std::string &alphaName,
                           const sf::Vector2i &startFlat,
                           const sf::Vector2i &startShadow);

    // Draw selected text in given position ( all lowercase please :-) )
    // allowed characters: a-z 0-9 . ( ) ? ! ' " - (spacebar)
    // - position in pixels
    // - scale to make the text adjust with screen size
    // - color just changes the font's hue, so color::white means blue font
    // - see letters.png file for useFlatFont
    static void write(sf::RenderTarget &window, const std::string &text,
                      sf::Vector2f position, const float scale,
                      const sf::Color &color = Color::Default,
                      const bool useFlatFont = true,
                      const TextAlign align = TextAlign::LEFT,
                      const TextVerticalAlign alignV = TextVerticalAlign::TOP);
};
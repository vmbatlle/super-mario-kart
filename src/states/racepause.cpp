#include "racepause.h"

#include "gui/textutils.h"

void StateRacePause::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::CONTINUE, event)) {
        if (currentState == MenuState::NO) {
            Audio::resumeMusic();
            Audio::resumeSFX();
            Audio::resumeEngines();
            game.popState();
        } else if (currentState == MenuState::YES) {
            // goto start
            game.popState();
            game.popState();
            game.popState();
            game.popState();
            game.popState();
        }
    }
    if (Input::pressed(Key::MENU_DOWN, event)) currentState = MenuState::NO;
    if (Input::pressed(Key::MENU_UP, event)) currentState = MenuState::YES;
}

void StateRacePause::fixedUpdate(const sf::Time& /*deltaTime*/) {
    // TODO igual no se necesita esto, solo handleEvent (?)
    // a no ser que queramos hacer que la opcion actual parpadee o algo
}

void StateRacePause::draw(sf::RenderTarget& window) {
    // TODO
    sf::Vector2u windowSize = window.getSize();
    window.clear(sf::Color::Black);
    float scale = windowSize.x / 256.0;

    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setOrigin(0.0f, backgroundTexture.getSize().y);
    backgroundSprite.scale(1.0f, -1.0f);
    window.draw(backgroundSprite);

    float bigFontScale = scale * 1.5;
    TextUtils::write(
        window, "give up?",
        sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y / 4.0f - TextUtils::CHAR_SIZE / 2.0f * bigFontScale
            - TextUtils::CHAR_SIZE * 2.0f * bigFontScale),
        bigFontScale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);

    std::string yesStr = currentState == MenuState::YES ? "> yes  " : "  yes  ";
    TextUtils::write(
        window, yesStr,
        sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y / 4.0f - TextUtils::CHAR_SIZE / 2.0f * scale
            - TextUtils::CHAR_SIZE * 0.75f * scale),
        scale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);
    
    std::string noStr = currentState == MenuState::NO ? "> no  " : "  no  ";
    TextUtils::write(
        window, noStr,
        sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y / 4.0f - TextUtils::CHAR_SIZE / 2.0f * scale
            + TextUtils::CHAR_SIZE * 1.0f * scale),
        scale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);
}
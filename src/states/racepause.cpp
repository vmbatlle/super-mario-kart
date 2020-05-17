#include "racepause.h"

#include "gui/textutils.h"

const sf::Time StateRacePause::FADE_TIME = sf::seconds(1.5f);

void StateRacePause::handleEvent(const sf::Event& event) {
    if (currentState == MenuState::FADE_OUT) {
        return;
    }
    if (Input::pressed(Key::ACCEPT, event) && !hasPopped) {
        if (currentState == MenuState::NO) {
            Audio::play(SFX::MENU_SELECTION_ACCEPT);
            hasPopped = true;
            Audio::resumeMusic();
            Audio::resumeSFX();
            Audio::resumeEngines();
            game.popState();
        } else if (currentState == MenuState::YES) {
            Audio::play(SFX::MENU_SELECTION_ACCEPT);
            currentState = MenuState::FADE_OUT;
            fadeTime = sf::Time::Zero;
        }
    }
    if (Input::pressed(Key::MENU_DOWN, event)) {
        Audio::play(SFX::MENU_SELECTION_MOVE);
        currentState = MenuState::NO;
    }
    if (Input::pressed(Key::MENU_UP, event)) {
        Audio::play(SFX::MENU_SELECTION_MOVE);
        currentState = MenuState::YES;
    }
}

bool StateRacePause::fixedUpdate(const sf::Time& deltaTime) {
    fadeTime += deltaTime;
    if (currentState == MenuState::FADE_OUT && fadeTime > FADE_TIME &&
        !hasPopped) {
        hasPopped = true;
        Audio::stopEngines();
        Audio::stopMusic();
        Audio::stopSFX();
        // goto start
        game.popStatesUntil(1);
    }
    if (once) {
        once = false;
        Audio::pauseMusic();
        Audio::pauseSFX();
        Audio::pauseEngines();
        Audio::play(SFX::MENU_SELECTION_MOVE);
    }
    return true;
}

void StateRacePause::draw(sf::RenderTarget& window) {
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
        sf::Vector2f(windowSize.x / 2.0f,
                     windowSize.y / 4.0f -
                         TextUtils::CHAR_SIZE / 2.0f * bigFontScale -
                         TextUtils::CHAR_SIZE * 2.0f * bigFontScale),
        bigFontScale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);

    std::string yesStr =
        currentState == MenuState::YES || currentState == MenuState::FADE_OUT
            ? "> yes  "
            : "  yes  ";
    TextUtils::write(
        window, yesStr,
        sf::Vector2f(windowSize.x / 2.0f,
                     windowSize.y / 4.0f - TextUtils::CHAR_SIZE / 2.0f * scale -
                         TextUtils::CHAR_SIZE * 0.75f * scale),
        scale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);

    std::string noStr = currentState == MenuState::NO ? "> no  " : "  no  ";
    TextUtils::write(
        window, noStr,
        sf::Vector2f(windowSize.x / 2.0f,
                     windowSize.y / 4.0f - TextUtils::CHAR_SIZE / 2.0f * scale +
                         TextUtils::CHAR_SIZE * 1.0f * scale),
        scale, sf::Color::Black, true, TextUtils::TextAlign::CENTER);

    if (currentState == MenuState::FADE_OUT) {
        float pct = fadeTime / FADE_TIME;
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Image black;
        black.create(windowSize.x, windowSize.y, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);

        // credits below
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}
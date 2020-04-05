#include "start.h"

sf::Texture StateStart::assetBackground, StateStart::assetLogo;

const sf::Vector2f StateStart::ABS_MENU =
    sf::Vector2f(73.0f / BACKGROUND_WIDTH, 170.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_LOGO =
    sf::Vector2f(128.0f / BACKGROUND_WIDTH, 26.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::MENU_SIZE =
    sf::Vector2f(110.0f / BACKGROUND_WIDTH, 54.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_MENU_CENTER = ABS_LOGO + MENU_SIZE / 2.0f;

void StateStart::loadBackgroundAssets(const std::string& assetName,
                                      const sf::IntRect& roiBackground,
                                      const sf::IntRect& roiLogo) {
    assetBackground.loadFromFile(assetName, roiBackground);
    assetBackground.setRepeated(true);
    assetLogo.loadFromFile(assetName, roiLogo);
}

void StateStart::init() {
    currentState = MenuState::NO_MENUS;
    backgroundPosition = 0.0f;
    timeSinceStateChange = sf::Time::Zero;
}

void StateStart::handleEvent(const sf::Event& event) {
    switch (currentState) {
        case MenuState::NO_MENUS:
            if (Input::pressed(Key::MENU_UP, event) ||
                Input::pressed(Key::MENU_DOWN, event) ||
                Input::pressed(Key::MENU_LEFT, event) ||
                Input::pressed(Key::MENU_RIGHT, event) ||
                Input::pressed(Key::ACCELERATE,
                               event)) {  // menu says "press X"
                currentState = MenuState::MENU_FADE_IN;
                timeSinceStateChange = sf::Time::Zero;
                game.popState(); // TODO temporal, remove
            }
            break;
        case MenuState::MENU:
            if (Input::pressed(Key::ACCEPT, event) ||
                Input::pressed(Key::ACCELERATE,
                               event)) {  // menu says "press X"
                // TODO menu logic handling
                currentState = MenuState::CONTROLS_FADE_IN;
                timeSinceStateChange = sf::Time::Zero;
            } else if (Input::pressed(Key::CANCEL, event)) {
                currentState = MenuState::MENU_FADE_OUT;
                timeSinceStateChange = sf::Time::Zero;
            }
            break;
        // TODO add other logic for controls menu
        default:
            break;
    }
}

void StateStart::fixedUpdate(const sf::Time& deltaTime) {
    timeSinceStateChange += deltaTime;
    if (currentState == MenuState::NO_MENUS) {
        backgroundPosition -= BACKGROUND_PPS * deltaTime.asSeconds();
    }
}

void StateStart::draw(sf::RenderTarget& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u backgroundSize = assetBackground.getSize();
    float scale = windowSize.y / (float)backgroundSize.y;

    sf::Sprite background(assetBackground);
    int pixelsLeft = backgroundPosition * scale;
    background.setTextureRect(
        {pixelsLeft, 0, (int)backgroundSize.x, (int)backgroundSize.y});
    background.setScale(scale, scale);
    background.setPosition(0.0f, 0.0f);
    window.draw(background);

    sf::Sprite logo(assetLogo);
    logo.setOrigin(assetLogo.getSize().x / 2.0f, 0.0f);
    logo.setScale(scale, scale);
    logo.setPosition(
        sf::Vector2f(ABS_LOGO.x * windowSize.x, ABS_LOGO.y * windowSize.y));
    window.draw(logo);
}
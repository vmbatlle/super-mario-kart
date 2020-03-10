#include "start.h"
#include <iostream>

void StateStart::init() {
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;

    background.loadFromFile("assets/background.png");
    backgroundDay.loadFromImage(background, sf::IntRect(246, 16, 511, 255));
    backgroundDay.setSmooth(false);
    backgroundDay.setRepeated(true);
    logo.loadFromImage(background, sf::IntRect(6, 16, 233, 92));

    backgroundAspectRatio =
        backgroundDay.getSize().x / backgroundDay.getSize().y;
    backgroundPos = 0.0f;
    backgroundSpeed = -100.0f;
}

void StateStart::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed &&
        event.type != sf::Event::KeyReleased) {
        return;
    }
    switch (event.key.code) {
        case sf::Keyboard::Up:
            upPressed = event.type == sf::Event::KeyPressed;
            break;
        case sf::Keyboard::Down:
            downPressed = event.type == sf::Event::KeyPressed;
            break;
        case sf::Keyboard::Left:
            leftPressed = event.type == sf::Event::KeyPressed;
            break;
        case sf::Keyboard::Right:
            rightPressed = event.type == sf::Event::KeyPressed;
            break;
        default:
            break;
    }
}

void StateStart::fixedUpdate(const sf::Time& deltaTime) {
    if (upPressed || rightPressed || downPressed || upPressed) {
        // skip screen
        game.popState();
    }
    if (backgroundPos > -(float)backgroundDay.getSize().x * 2) {
        backgroundPos += backgroundSpeed * deltaTime.asSeconds();
    } else {
        backgroundPos = 0.f;
    }
}

void StateStart::draw(sf::RenderTarget& window) {
    sf::RectangleShape backS, logoS;

    backS.setPosition(sf::Vector2f(backgroundPos, 0.f));
    backS.setSize(sf::Vector2f(backgroundDay.getSize().x * 2,
                               game.getWindow().getSize().y));
    backS.setTexture(&backgroundDay, false);
    backS.setTextureRect({0, 0, 511, 255});

    logoS.setTexture(&logo);
    logoS.setSize(sf::Vector2f(logo.getSize().x, logo.getSize().y));
    logoS.setOrigin(233 / 2, 92 / 2);
    logoS.setPosition(sf::Vector2f(game.getWindow().getSize().x / 2,
                                   game.getWindow().getSize().y / 4));

    window.draw(backS);
    window.draw(logoS);
}
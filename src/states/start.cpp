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

    background_aspect_ratio = backgroundDay.getSize().x / backgroundDay.getSize().y;
    background_pos = 0.f;
    background_speed = -100.f;
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
        //skip screen
        game.popState();
    }
    if (background_pos > -(float)backgroundDay.getSize().x * 2) {
        background_pos += background_speed * deltaTime.asSeconds();
    } else {
        background_pos = 0.f;
    }
}

void StateStart::draw(sf::RenderTarget& window) {

    sf::RectangleShape back_s, logo_s;

    
    
    back_s.setPosition(sf::Vector2f(background_pos, 0.f));
    back_s.setSize(sf::Vector2f(backgroundDay.getSize().x * 2, 
                                game.getWindow().getSize().y));
    back_s.setTexture(&backgroundDay, false);
    back_s.setTextureRect({ 0, 0, 511, 255 });
    
    

    logo_s.setTexture(&logo);
    logo_s.setSize(sf::Vector2f(logo.getSize().x,logo.getSize().y));
    logo_s.setOrigin(233/2, 92/2);
    logo_s.setPosition(sf::Vector2f(game.getWindow().getSize().x / 2, 
                                    game.getWindow().getSize().y / 4));


    window.draw(back_s);
    window.draw(logo_s);
}
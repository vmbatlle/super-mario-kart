#include "input.h"

Input Input::instance;

Input::Input() {
    // Driving actions
    set(Key::ACCELERATE, sf::Keyboard::X);
    set(Key::BRAKE, sf::Keyboard::Z);
    set(Key::TURN_LEFT, sf::Keyboard::Left);
    set(Key::TURN_RIGHT, sf::Keyboard::Right);
    set(Key::ITEM_FRONT, sf::Keyboard::Up);
    set(Key::ITEM_BACK, sf::Keyboard::Down);
    // Menu actions
    set(Key::PAUSE, sf::Keyboard::Escape);
    set(Key::CONTINUE, sf::Keyboard::Enter);
    set(Key::ACCEPT, sf::Keyboard::Enter);
    set(Key::CANCEL, sf::Keyboard::Escape);
    set(Key::MENU_UP, sf::Keyboard::Up);
    set(Key::MENU_DOWN, sf::Keyboard::Down);
    set(Key::MENU_LEFT, sf::Keyboard::Left);
    set(Key::MENU_RIGHT, sf::Keyboard::Right);
}
#include "input.h"

sf::Keyboard::Key Input::map[(int)Key::__COUNT] = {sf::Keyboard::Key::Up};
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

// Read/write the key map
inline void Input::set(const Key action, const sf::Keyboard::Key code) {
    map[(int)action] = code;
}
inline const sf::Keyboard::Key &Input::get(Key action) {
    return map[(int)action];
}

// Check for key press/release/hold events
inline bool Input::pressed(const Key action, const sf::Event &event) {
    return event.type == sf::Event::KeyPressed && event.key.code == get(action);
}
inline bool Input::released(const Key action, const sf::Event &event) {
    return event.type == sf::Event::KeyReleased &&
           event.key.code == get(action);
}
inline bool Input::held(const Key action) {
    return sf::Keyboard::isKeyPressed(get(action));
}
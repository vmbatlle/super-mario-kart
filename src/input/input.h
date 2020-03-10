#pragma once

#include <SFML/Graphics.hpp>

enum class Key : int {
    // Driving actions
    ACCELERATE,
    BRAKE,
    TURN_LEFT,
    TURN_RIGHT,
    ITEM_FRONT,
    ITEM_BACK,
    // Menu actions
    PAUSE,
    CONTINUE,
    ACCEPT,
    CANCEL,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    __COUNT
};

class Input {
   private:
    static sf::Keyboard::Key map[(int)Key::__COUNT];
    static Input instance;

    Input();

   public:
    // Read/write the key map
    static inline void set(const Key action, const sf::Keyboard::Key code) {
        map[(int)action] = code;
    }
    static inline const sf::Keyboard::Key &get(Key action) {
        return map[(int)action];
    }

    // Check for key press/release/hold events
    static inline bool pressed(const Key action, const sf::Event &event) {
        return event.type == sf::Event::KeyPressed &&
               event.key.code == get(action);
    }
    static inline bool released(const Key action, const sf::Event &event) {
        return event.type == sf::Event::KeyReleased &&
               event.key.code == get(action);
    }
    static inline bool held(const Key action) {
        return sf::Keyboard::isKeyPressed(get(action));
    }
};
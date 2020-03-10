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
    inline static void set(const Key action, const sf::Keyboard::Key code);
    inline static const sf::Keyboard::Key &get(Key action);

    // Check for key press/release/hold events
    inline static bool pressed(const Key action, const sf::Event &event);
    inline static bool released(const Key action, const sf::Event &event);
    inline static bool held(const Key action);
};
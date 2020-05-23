#include "input.h"

Input Input::instance;

Input::Input() {
    // Driving actions
    set(Key::ACCELERATE, sf::Keyboard::X);
    set(Key::BRAKE, sf::Keyboard::Z);
    set(Key::DRIFT, sf::Keyboard::C);
    set(Key::TURN_LEFT, sf::Keyboard::Left);
    set(Key::TURN_RIGHT, sf::Keyboard::Right);
    set(Key::ITEM_FRONT, sf::Keyboard::Up);
    set(Key::ITEM_BACK, sf::Keyboard::Down);
    // Menu actions
    set(Key::PAUSE, sf::Keyboard::Escape);
    // set(Key::CONTINUE, sf::Keyboard::Enter);  // [[ deprecated ]]
    set(Key::ACCEPT, sf::Keyboard::Enter);
    set(Key::CANCEL, sf::Keyboard::Escape);
    set(Key::MENU_UP, sf::Keyboard::Up);
    set(Key::MENU_DOWN, sf::Keyboard::Down);
    set(Key::MENU_LEFT, sf::Keyboard::Left);
    set(Key::MENU_RIGHT, sf::Keyboard::Right);
}

std::string Input::getActionName(const Key action) {
    std::string ret;
    switch (action) {
        // Driving actions
        case Key::ACCELERATE:
            ret = "accelerate";
            break;
        case Key::BRAKE:
            ret = "brake";
            break;
        case Key::DRIFT:
            ret = "drift";
            break;
        case Key::TURN_LEFT:
            ret = "turn left";
            break;
        case Key::TURN_RIGHT:
            ret = "turn right";
            break;
        case Key::ITEM_FRONT:
            ret = "item front";
            break;
        case Key::ITEM_BACK:
            ret = "item back";
            break;
        // Menu actions
        case Key::PAUSE:
            ret = "pause";
            break;
        // [[ deprecated ]]
        // case Key::CONTINUE:
        //     ret = "continue";
        //     break;
        case Key::ACCEPT:
            ret = "accept";
            break;
        case Key::CANCEL:
            ret = "cancel";
            break;
        case Key::MENU_UP:
            ret = "menu up";
            break;
        case Key::MENU_DOWN:
            ret = "menu down";
            break;
        case Key::MENU_LEFT:
            ret = "menu left";
            break;
        case Key::MENU_RIGHT:
            ret = "menu right";
            break;
        default:
            ret = "?";
            break;
    }
    return ret;
}

// code based on:
// https://en.sfml-dev.org/forums/index.php?topic=15226.0
std::string Input::getKeyCodeName(const sf::Keyboard::Key keycode) {
    std::string ret;
    switch (keycode) {
        case sf::Keyboard::A:
            ret = "a";
            break;
        case sf::Keyboard::B:
            ret = "b";
            break;
        case sf::Keyboard::C:
            ret = "c";
            break;
        case sf::Keyboard::D:
            ret = "d";
            break;
        case sf::Keyboard::E:
            ret = "e";
            break;
        case sf::Keyboard::F:
            ret = "f";
            break;
        case sf::Keyboard::G:
            ret = "g";
            break;
        case sf::Keyboard::H:
            ret = "h";
            break;
        case sf::Keyboard::I:
            ret = "i";
            break;
        case sf::Keyboard::J:
            ret = "j";
            break;
        case sf::Keyboard::K:
            ret = "k";
            break;
        case sf::Keyboard::L:
            ret = "l";
            break;
        case sf::Keyboard::M:
            ret = "m";
            break;
        case sf::Keyboard::N:
            ret = "n";
            break;
        case sf::Keyboard::O:
            ret = "o";
            break;
        case sf::Keyboard::P:
            ret = "p";
            break;
        case sf::Keyboard::Q:
            ret = "q";
            break;
        case sf::Keyboard::R:
            ret = "r";
            break;
        case sf::Keyboard::S:
            ret = "s";
            break;
        case sf::Keyboard::T:
            ret = "t";
            break;
        case sf::Keyboard::U:
            ret = "u";
            break;
        case sf::Keyboard::V:
            ret = "v";
            break;
        case sf::Keyboard::W:
            ret = "w";
            break;
        case sf::Keyboard::X:
            ret = "x";
            break;
        case sf::Keyboard::Y:
            ret = "y";
            break;
        case sf::Keyboard::Z:
            ret = "z";
            break;
        case sf::Keyboard::Num0:
            ret = "0";
            break;
        case sf::Keyboard::Num1:
            ret = "1";
            break;
        case sf::Keyboard::Num2:
            ret = "2";
            break;
        case sf::Keyboard::Num3:
            ret = "3";
            break;
        case sf::Keyboard::Num4:
            ret = "4";
            break;
        case sf::Keyboard::Num5:
            ret = "5";
            break;
        case sf::Keyboard::Num6:
            ret = "6";
            break;
        case sf::Keyboard::Num7:
            ret = "7";
            break;
        case sf::Keyboard::Num8:
            ret = "8";
            break;
        case sf::Keyboard::Num9:
            ret = "9";
            break;
        case sf::Keyboard::Escape:
            ret = "escape";
            break;
        case sf::Keyboard::LControl:
            ret = "l ctrl";
            break;
        case sf::Keyboard::LShift:
            ret = "l shift";
            break;
        case sf::Keyboard::LAlt:
            ret = "l alt";
            break;
        case sf::Keyboard::LSystem:
            ret = "l system";
            break;
        case sf::Keyboard::RControl:
            ret = "r ctrl";
            break;
        case sf::Keyboard::RShift:
            ret = "r shift";
            break;
        case sf::Keyboard::RAlt:
            ret = "r alt";
            break;
        case sf::Keyboard::RSystem:
            ret = "r system";
            break;
        case sf::Keyboard::Dash:
            ret = "dash";
            break;
        case sf::Keyboard::Space:
            ret = "space";
            break;
        case sf::Keyboard::Enter:
            ret = "enter";
            break;
        case sf::Keyboard::BackSpace:
            ret = "delete";
            break;
        case sf::Keyboard::Tab:
            ret = "tab";
            break;
        case sf::Keyboard::PageUp:
            ret = "pg up";
            break;
        case sf::Keyboard::PageDown:
            ret = "pg down";
            break;
        case sf::Keyboard::End:
            ret = "end";
            break;
        case sf::Keyboard::Home:
            ret = "home";
            break;
        case sf::Keyboard::Insert:
            ret = "insert";
            break;
        case sf::Keyboard::Delete:
            ret = "delete";
            break;
        case sf::Keyboard::Left:
            ret = "left";
            break;
        case sf::Keyboard::Right:
            ret = "right";
            break;
        case sf::Keyboard::Up:
            ret = "up";
            break;
        case sf::Keyboard::Down:
            ret = "down";
            break;
        case sf::Keyboard::Numpad0:
            ret = "numpad 0";
            break;
        case sf::Keyboard::Numpad1:
            ret = "numpad 1";
            break;
        case sf::Keyboard::Numpad2:
            ret = "numpad 2";
            break;
        case sf::Keyboard::Numpad3:
            ret = "numpad 3";
            break;
        case sf::Keyboard::Numpad4:
            ret = "numpad 4";
            break;
        case sf::Keyboard::Numpad5:
            ret = "numpad 5";
            break;
        case sf::Keyboard::Numpad6:
            ret = "numpad 6";
            break;
        case sf::Keyboard::Numpad7:
            ret = "numpad 7";
            break;
        case sf::Keyboard::Numpad8:
            ret = "numpad 8";
            break;
        case sf::Keyboard::Numpad9:
            ret = "numpad 9";
            break;
        case sf::Keyboard::F1:
            ret = "f1";
            break;
        case sf::Keyboard::F2:
            ret = "f2";
            break;
        case sf::Keyboard::F3:
            ret = "f3";
            break;
        case sf::Keyboard::F4:
            ret = "f4";
            break;
        case sf::Keyboard::F5:
            ret = "f5";
            break;
        case sf::Keyboard::F6:
            ret = "f6";
            break;
        case sf::Keyboard::F7:
            ret = "f7";
            break;
        case sf::Keyboard::F8:
            ret = "f8";
            break;
        case sf::Keyboard::F9:
            ret = "f9";
            break;
        case sf::Keyboard::F10:
            ret = "f10";
            break;
        case sf::Keyboard::F11:
            ret = "f11";
            break;
        case sf::Keyboard::F12:
            ret = "f12";
            break;
        default:
            ret = "?";
            break;
    }
    return ret;
}
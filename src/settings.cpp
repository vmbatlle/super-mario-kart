#include "settings.h"

// Just work with letters
sf::Keyboard::Key strToKey( std::string value) {
    return (sf::Keyboard::Key) (int)(std::atoi(value.c_str()));
}

void Settings::applySetting(const std::string key, std::string value) {
    if (key.compare(MUSIC_VOLUME) == 0) {
        Audio::setVolume(std::stof(value), Audio::getSfxVolume());
    } else if (key.compare(SFX_VOLUME) == 0) {
        Audio::setVolume(Audio::getMusicVolume(), std::stof(value));
    } else if (key.compare(RESOLUTION) == 0) {
        //*inst.scaleFactor = std::stof(value);
    } else {
        //Controls
        //std::cout << "Readed " << key << "(int = " << std::atoi(key.c_str()) << ")" << " value " << value << std::endl;
        switch(std::atoi(key.c_str())) {
            case (int)Key::ACCELERATE:
                    Input::set(Key::ACCELERATE, strToKey(value));
                break;
            case (int)Key::BRAKE:
                    Input::set(Key::BRAKE, strToKey(value));
                break;
            case (int)Key::DRIFT:
                    Input::set(Key::DRIFT, strToKey(value));
                break;
            case (int)Key::TURN_LEFT:
                    Input::set(Key::TURN_LEFT, strToKey(value));
                break;
            case (int)Key::TURN_RIGHT:
                    Input::set(Key::TURN_RIGHT, strToKey(value));
                break;
            case (int)Key::ITEM_FRONT:
                    Input::set(Key::ITEM_FRONT, strToKey(value));
                break;
            case (int)Key::ITEM_BACK:
                    Input::set(Key::ITEM_BACK, strToKey(value));
                break;

            // Menu actions
            case (int)Key::PAUSE:
                    Input::set(Key::PAUSE, strToKey(value));
                break;
            case (int)Key::CONTINUE:
                    Input::set(Key::CONTINUE, strToKey(value));
                break;
            case (int)Key::ACCEPT:
                    Input::set(Key::ACCEPT, strToKey(value));
                break;
            case (int)Key::CANCEL:
                    Input::set(Key::CANCEL, strToKey(value));
                break;
            case (int)Key::MENU_UP:
                    Input::set(Key::MENU_UP, strToKey(value));
                break;
            case (int)Key::MENU_DOWN:
                    Input::set(Key::MENU_DOWN, strToKey(value));
                break;
            case (int)Key::MENU_LEFT:
                    Input::set(Key::MENU_LEFT, strToKey(value));
                break;
            case (int)Key::MENU_RIGHT:
                    Input::set(Key::MENU_RIGHT, strToKey(value));
                break;
            default:
                break;
        }
    }
}


void Settings::loadSettings(uint &res) {
    std::string line;
    std::ifstream file(FILE_NAME, std::fstream::in);
    //inst.scaleFactor = &res;

    while(file.is_open() && std::getline(file, line)) {
        std::string key = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=')+1);
        applySetting(key, value);
    }

    file.close();
    
}

void Settings::saveSettings(uint res) {
    std::string line;
    std::ofstream file;
    file.open(FILE_NAME, std::fstream::out);

    file << MUSIC_VOLUME << "=" << Audio::getMusicVolume() << std::endl;
    file << SFX_VOLUME << "=" << Audio::getSfxVolume() << std::endl;
    file << RESOLUTION << "=" << res << std::endl;

    for (int i = 0; i < (int)Key::__COUNT; i++)
    file << (int)i << "=" << (int)Input::get((Key)i) << std::endl;
    
}
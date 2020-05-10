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
                    //std::cout << "ACELERATE " << std::atoi(key.c_str()) << " value " << value  << "tokey " << strToKey(value) << std::endl;
                    Input::set(Key::ACCELERATE, strToKey(value));
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
    file << (int)Key::ACCELERATE << "=" << (int)Input::get(Key::ACCELERATE) << std::endl;
    
}
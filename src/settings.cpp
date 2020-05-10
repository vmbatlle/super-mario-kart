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
        for (int i = 0; i < (int)Key::__COUNT; i++)
            if ((Key)std::atoi(key.c_str()) == (Key)i)
                Input::set((Key)i, strToKey(value));
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
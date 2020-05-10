#include "settings.h"

void applySetting(const std::string key, std::string value) {
    if (key.compare(MUSIC_VOLUME) == 0) {
        Audio::setVolume(std::stof(value), Audio::getSfxVolume());
    } else if (key.compare(SFX_VOLUME) == 0) {
        Audio::setVolume(Audio::getMusicVolume(), std::stof(value));
    }
}


void Settings::loadSettings() {
    std::string line;
    std::ifstream file(FILE_NAME, std::fstream::in);

    while(file.is_open() && std::getline(file, line)) {
        std::string key = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=')+1);
        applySetting(key, value);
    }

    file.close();
    
}

void Settings::saveSettings(sf::Vector2u resolution) {
    std::string line;
    std::ofstream file;
    file.open(FILE_NAME, std::fstream::out);

    file << MUSIC_VOLUME << "=" << Audio::getMusicVolume() << std::endl;
    file << SFX_VOLUME << "=" << Audio::getSfxVolume() << std::endl;
    file << RESOLUTION_X << "=" << resolution.x << std::endl;
    file << RESOLUTION_Y << "=" << resolution.y << std::endl;
    
}
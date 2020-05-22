#pragma once

class Settings;

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

#include "audio/audio.h"
#include "game.h"
#include "input/input.h"

class Settings {
   private:
    static const std::string FILE_NAME;
    static const std::string MUSIC_VOLUME;
    static const std::string SFX_VOLUME;
    static const std::string RESOLUTION;
    static const std::string KEY_TITLE;

    Settings() = default;
    static Settings instance;

    static const unsigned int DEFAULT_RESOLUTION_INDEX = 2;
    static const std::array<unsigned int, 7> ALLOWED_MULTIPLIERS;
    unsigned int resolutionIndex;
    unsigned int resolutionMultiplier;  // game resolution is BASIC_WIDTH *
                                        // multiplier, BASIC_HEIGHT *
                                        // multiplier; valid values are whole
                                        // numbers or numbers ended in .5

    static bool applySetting(const std::string &key, const std::string &value);
    static void writeDefaultSettings();

   public:
    static inline unsigned int getResolutionMultiplier() {
        return instance.resolutionMultiplier;
    }
    static inline bool incrementResolutionMultiplier() {
        if (instance.resolutionIndex == ALLOWED_MULTIPLIERS.size() - 1 ||
            sf::VideoMode::getDesktopMode().height <
                BASIC_HEIGHT *
                    ALLOWED_MULTIPLIERS[instance.resolutionIndex + 1] / 2.0f) {
            return false;
        } else {
            instance.resolutionIndex++;
            instance.resolutionMultiplier =
                ALLOWED_MULTIPLIERS[instance.resolutionIndex];
            return true;
        }
    }
    static inline bool decrementResolutionMultiplier() {
        if (instance.resolutionIndex == 0) {
            return false;
        } else {
            instance.resolutionIndex--;
            instance.resolutionMultiplier =
                ALLOWED_MULTIPLIERS[instance.resolutionIndex];
            return true;
        }
    }

    // Tries to read FILE_NAME, and if it doesn't exist, creates it
    static void tryLoadSettings();
    // Create FILE_NAME with current (default) settings
    static void saveSettings();
};
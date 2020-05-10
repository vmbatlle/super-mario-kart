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

    static const uint DEFAULT_RESOLUTION_INDEX = 2;
    static const std::array<uint, 4> ALLOWED_MULTIPLIERS;
    uint resolutionIndex;
    uint resolutionMultiplier;  // game resolution is BASIC_WIDTH *
                                // multiplier, BASIC_HEIGHT * multiplier;
                                // valid values are 1, 2, 4 (256x224,
                                // 512x448, and 1024x896)

    static bool applySetting(const std::string &key, const std::string &value);
    static void writeDefaultSettings();

   public:
    static inline uint getResolutionMultiplier() {
        return instance.resolutionMultiplier;
    }
    static inline bool incrementResolutionMultiplier() {
        if (instance.resolutionIndex == ALLOWED_MULTIPLIERS.size() - 1) {
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
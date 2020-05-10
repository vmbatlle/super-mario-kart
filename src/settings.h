#pragma once

class Settings;

#include <iostream>
#include <fstream>
#include "audio/audio.h"
#include "input/input.h"
#include <SFML/Graphics.hpp>
#include "game.h"

static const std::string FILE_NAME = "settings.txt";

static const std::string MUSIC_VOLUME = "MUSIC_VOLUME";
static const std::string SFX_VOLUME = "SFX_VOLUME";
static const std::string RESOLUTION = "RESOLUTION";

class Settings {
   private:
    Settings();
    static Settings inst;

   public:
   
    uint *scaleFactor;
    static void loadSettings(uint &res);
    static void saveSettings(uint res);
    static void applySetting(const std::string key, std::string value);

};
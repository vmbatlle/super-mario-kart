#include "audio.h"

Audio Audio::instance;

SFX Audio::loadDing() {
    SFX ding = SFX::MENU_INTRO_SCREEN_DING;
    instance.load(ding, "assets/sfx/nintendo_logo.ogg");
    return ding;
}

void Audio::loadAll() {
    // instance.load(Music::MENU_TITLE_SCREEN,
    //               "assets/music/menu_title_screen.ogg");
    // instance.load(Music::MENU_PLAYER_CIRCUIT,
    //               "assets/music/menu_player_circuit.ogg");
    // instance.load(Music::CIRCUIT_END_VICTORY,
    //               "assets/music/circuit_end_victory.ogg");
    // instance.load(Music::CIRCUIT_END_DEFEAT,
    //               "assets/music/circuit_end_defeat.ogg");
    // instance.load(Music::CONGRATULATIONS_SCREEN,
    //               "assets/music/congratulations_screen.ogg");

    // // TODO complete
    // // instance.load(SFX::MENU_INTRO_SCREEN_DING, "assets/sfx/TODO.ogg");
    // instance.load(SFX::MENU_SELECTION_ACCEPT, "assets/sfx/TODO.ogg");
    // instance.load(SFX::MENU_SELECTION_MOVE, "assets/sfx/TODO.ogg");
    // instance.load(SFX::CIRCUIT_ANIMATION_START, "assets/sfx/TODO.ogg");
}

void Audio::loadCircuit(const std::string &folder) {
    instance.load(Music::CIRCUIT_NORMAL, folder + "/music_normal.ogg");
    instance.load(Music::CIRCUIT_LAST_LAP, folder + "/music_last_lap.ogg");
}

void Audio::load(const Music music, const std::string &filename) {
    musicList[(int)music].openFromFile(filename);
}
void Audio::load(const SFX sfx, const std::string &filename) {
    sfxList[(int)sfx].loadFromFile(filename);
}

void Audio::play(const Music music) {
    instance.musicMutex.lock();
    for (auto &music : instance.musicList) {
        music.stop();
    }
    instance.musicList[(int)music].play();
    instance.musicMutex.unlock();
}

void Audio::play(const SFX sfx) {
    instance.sfxMutex.lock();
    int i = instance.currentSoundIndex++ % MAX_SOUNDS;
    instance.sfxMutex.unlock();
    instance.playingSounds[i].setBuffer(instance.sfxList[(int)sfx]);
    instance.playingSounds[i].play();
}
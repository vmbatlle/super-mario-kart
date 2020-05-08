#include "audio.h"

Audio Audio::instance;

SFX Audio::loadDing() {
    SFX ding = SFX::MENU_INTRO_SCREEN_DING;
    instance.load(ding, "assets/sfx/nintendo_logo.ogg");
    return ding;
}

void Audio::loadAll() {
    instance.load(Music::MENU_TITLE_SCREEN,
                  "assets/music/menu_title_screen.ogg");
    instance.load(Music::MENU_PLAYER_CIRCUIT,
                  "assets/music/menu_player_circuit.ogg");
    instance.load(Music::CIRCUIT_ANIMATION_START, "assets/music/circuit_opening.ogg");
    instance.load(Music::CIRCUIT_END_VICTORY, "assets/sfx/win.ogg");
    instance.load(Music::CIRCUIT_END_DEFEAT, "assets/sfx/lose.ogg");

    // // TODO complete
    // instance.load(SFX::MENU_INTRO_SCREEN_DING, "assets/sfx/TODO.ogg");
    // instance.load(SFX::MENU_SELECTION_ACCEPT, "assets/sfx/TODO.ogg");
    // instance.load(SFX::MENU_SELECTION_MOVE, "assets/sfx/TODO.ogg");
    instance.load(SFX::CIRCUIT_GOAL_END, "assets/sfx/star.ogg");
    instance.load(SFX::CIRCUIT_LAKITU_WARNING,
                  "assets/sfx/lakitu_warning_sfx.ogg");
    instance.load(SFX::CIRCUIT_ITEM_RANDOMIZING,
                  "assets/sfx/item_box_sfx_fixed.ogg");
    instance.load(SFX::CIRCUIT_LAKITU_SEMAPHORE,
                  "assets/sfx/race_start_sfx_fixed.ogg");
    instance.load(SFX::CIRCUIT_ITEM_THUNDER, "assets/sfx/lightning_sfx.ogg");
    instance.load(SFX::CIRCUIT_ITEM_STAR, "assets/sfx/star_10.ogg");
    instance.load(SFX::MENU_SELECTION_ACCEPT, "assets/sfx/grow_sfx.ogg");
    instance.load(SFX::MENU_SELECTION_MOVE, "assets/sfx/nintendo_logo.ogg");

    instance.load(SFX::CIRCUIT_GOAL_END, "assets/sfx/goal_sfx.ogg");
    instance.load(SFX::CIRCUIT_LAST_LAP_NOTICE, "assets/sfx/final_lap.ogg");
}

void Audio::loadCircuit(const std::string &folder) {
    instance.load(Music::CIRCUIT_NORMAL, folder + "/music.ogg");
    instance.load(Music::CIRCUIT_LAST_LAP, folder + "/final_lap.ogg");
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
    instance.musicList[(int)music].setLoop(true);
    instance.musicList[(int)music].setVolume(instance.musicVolumePct);
    instance.musicMutex.unlock();
}

void Audio::play(const SFX sfx, bool loop) {
    instance.sfxMutex.lock();
    int i = 0;
    for (int j = 0; j < MAX_SOUNDS; j++) {
        i = instance.currentSoundIndex++ % MAX_SOUNDS;
        if (instance.playingSounds[i].getStatus() ==
            sf::SoundSource::Status::Stopped) {
            break;
        }
    }
    instance.sfxMutex.unlock();
    instance.sfxLastIndex[(int)sfx] = i;
    instance.playingSounds[i].setBuffer(instance.sfxList[(int)sfx]);
    instance.playingSounds[i].play();
    instance.playingSounds[i].setLoop(loop);
    instance.playingSounds[i].setVolume(instance.sfxVolumePct);
}

void Audio::stopSFX() {
    for (int i = 0; i < MAX_SOUNDS; i++) instance.playingSounds[i].stop();
}

void Audio::stopMusic() {
    instance.musicMutex.lock();
    for (int i = 0; i < (int)Music::__COUNT; i++) {
        instance.musicList[i].stop();
    }
    instance.musicMutex.unlock();
}

// set volume as percent 0-1
void Audio::setVolume(const float musicVolumePct, const float sfxVolumePct) {
    instance.musicVolumePct = musicVolumePct * 100.0f;
    instance.sfxVolumePct = sfxVolumePct * 100.0f;
}

void Audio::setPitch(const SFX sfx, const float sfxPitch) {
    int i = instance.sfxLastIndex[(int)sfx];
    instance.playingSounds[i].setPitch(sfxPitch);
}

void Audio::playEngines() {
    for (auto &engine : instance.sfxEngines) {
        engine.openFromFile("assets/sfx/engine.ogg");
        engine.play();
        engine.setLoop(true);
        engine.setVolume(instance.sfxVolumePct);
        break; // TODO: remove line
    }
}

void Audio::updateEngine(unsigned int i, sf::Vector2f position, float height,
                         float speedForward) {
    instance.sfxEngines[i].setPosition(position.x, position.y, height);
    float maxLinearSpeed = 0.11232f;
    instance.sfxEngines[i].setPitch(1.0f + speedForward / maxLinearSpeed);
}

void Audio::updateListener(sf::Vector2f position, float height) {
    sf::Listener::setPosition(position.x, position.y, height);
}
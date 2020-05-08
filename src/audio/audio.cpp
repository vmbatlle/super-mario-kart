#include "audio.h"

#include <cmath>

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
    instance.load(Music::CIRCUIT_ANIMATION_START, "assets/music/TODO.ogg");
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

    instance.load(SFX::CIRCUIT_PLAYER_MOTOR, "assets/sfx/engine.ogg");
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

void Audio::playEngines(int playerIndex) {
    instance.playerIndex = playerIndex;
    std::string filename = "assets/sfx/engine.ogg";
    for (int i = 0; i < (int)MenuPlayer::__COUNT; i++) {
        if (i != playerIndex) {
            auto &engine = instance.sfxEngines[i];
            engine.openFromFile(filename);
            // engine.play();
            engine.setLoop(true);
            engine.setVolume(instance.sfxVolumePct / 2.0f);
        }
    }
    instance.sfxPlayerEngine.setBuffer(
        instance.sfxList[(int)SFX::CIRCUIT_PLAYER_MOTOR]);
    instance.sfxPlayerEngine.play();
    instance.sfxPlayerEngine.setLoop(true);
    instance.sfxPlayerEngine.setVolume(instance.sfxVolumePct / 2.0f);
    instance.sfxPlayerEngine.setRelativeToListener(true);
}

void Audio::updateEngine(unsigned int i, sf::Vector2f position, float height,
                         float speedForward, float speedTurn) {
    float maxLinearSpeed = 0.4992f;
    float maxSpeedTurn = 3.6f;
    float pitch = 1.0f;
    pitch += speedForward / maxLinearSpeed * 1.5;
    pitch -= fabs(speedTurn) / maxSpeedTurn * 0.65;
    if (height > 0.0f) pitch += 0.35f;
    pitch = fmin(pitch, 2.0f);
    if ((int)i != instance.playerIndex) {
        instance.sfxEngines[i].setPosition(position.x, position.y, height);
        instance.sfxEngines[i].setPitch(pitch);
    } else {
        instance.sfxPlayerEngine.setPitch(pitch);
    }
}

void Audio::updateEngine(sf::Vector2f position, float height,
                         float speedForward, float speedTurn) {
    updateEngine(instance.playerIndex, position, height, speedForward,
                 speedTurn);
}

void Audio::updateListener(sf::Vector2f position, float angle, float height) {
    sf::Listener::setPosition(position.x, position.y, height);
    sf::Listener::setDirection(cosf(angle), sinf(angle), 0.0f);
    sf::Listener::setUpVector(0.0f, 0.0f, 1.0f);
}

void Audio::stopEngines() {
    for (auto &engine : instance.sfxEngines) {
        engine.stop();
    }
    instance.sfxPlayerEngine.stop();
}
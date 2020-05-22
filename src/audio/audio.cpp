#include "audio.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "map/enums.h"

Audio Audio::instance;

SFX Audio::loadDing() {
    SFX ding = SFX::MENU_INTRO_SCREEN_DING;
    instance.load(ding, "assets/sfx/nintendologo.ogg");
    return ding;
}

void Audio::loadAll() {
    instance.load(Music::MENU_TITLE_SCREEN,
                  "assets/music/menu_title_screen.ogg");
    instance.load(Music::MENU_PLAYER_CIRCUIT,
                  "assets/music/menu_player_circuit.ogg");
    instance.load(Music::CIRCUIT_ANIMATION_START,
                  "assets/music/circuit_opening.ogg");
    instance.load(Music::CIRCUIT_PLAYER_WIN, "assets/music/tournament_win.ogg");
    instance.load(Music::CIRCUIT_PLAYER_LOSE,
                  "assets/music/tournament_lose.ogg");

    instance.load(SFX::MENU_SELECTION_ACCEPT, "assets/sfx/menuselect.ogg");
    instance.load(SFX::MENU_SELECTION_CANCEL, "assets/sfx/menuback.ogg");
    instance.load(SFX::MENU_SELECTION_MOVE, "assets/sfx/menumove.ogg");

    instance.load(SFX::CIRCUIT_LAKITU_SEMAPHORE, "assets/sfx/racestart.ogg");
    instance.load(SFX::CIRCUIT_LAKITU_WARNING, "assets/sfx/lakituwarning.ogg");

    instance.load(SFX::CIRCUIT_COLLISION, "assets/sfx/thud.ogg");
    instance.load(SFX::CIRCUIT_COLLISION_PIPE, "assets/sfx/thudpipe.ogg");

    instance.load(SFX::CIRCUIT_LAST_LAP_NOTICE, "assets/sfx/finallap.ogg");
    instance.load(SFX::CIRCUIT_GOAL_END, "assets/sfx/goal.ogg");
    instance.load(SFX::CIRCUIT_END_VICTORY, "assets/sfx/win.ogg");
    instance.load(SFX::CIRCUIT_END_DEFEAT, "assets/sfx/lose.ogg");

    instance.load(SFX::CIRCUIT_PLAYER_MOTOR, "assets/sfx/engine.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_BRAKE, "assets/sfx/brake.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_DRIFT, "assets/sfx/skid.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_JUMP, "assets/sfx/jump.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_LANDING, "assets/sfx/landing.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_FALL, "assets/sfx/fall.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_FALL_WATER, "assets/sfx/water.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_FALL_LAVA, "assets/sfx/lava.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_HIT, "assets/sfx/spinout.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_SMASH, "assets/sfx/hit.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_GROW, "assets/sfx/grow.ogg");
    instance.load(SFX::CIRCUIT_PLAYER_SHRINK, "assets/sfx/shrink.ogg");

    instance.load(SFX::CIRCUIT_COIN, "assets/sfx/coin.ogg");

    instance.load(SFX::CIRCUIT_ITEM_RANDOMIZING, "assets/sfx/itemreel.ogg");
    instance.load(SFX::CIRCUIT_ITEM_GET, "assets/sfx/item.ogg");
    instance.load(SFX::CIRCUIT_ITEM_USE_LAUNCH, "assets/sfx/throw.ogg");
    instance.load(SFX::CIRCUIT_ITEM_USE_UP, "assets/sfx/itemdestroy.ogg");
    instance.load(SFX::CIRCUIT_ITEM_USE_DOWN, "assets/sfx/itemdestroy.ogg");
    instance.load(SFX::CIRCUIT_ITEM_STAR, "assets/sfx/star.ogg");
    instance.load(SFX::CIRCUIT_ITEM_MUSHROOM, "assets/sfx/boost.ogg");
    instance.load(SFX::CIRCUIT_ITEM_THUNDER, "assets/sfx/lightning.ogg");
    instance.load(SFX::CIRCUIT_ITEM_RED_SHELL, "assets/sfx/redshell.ogg");
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

void Audio::play(const Music music, bool loop) {
    instance.musicMutex.lock();
    for (auto &music : instance.musicList) {
        music.stop();
    }
    instance.musicList[(int)music].play();
    instance.musicList[(int)music].setLoop(loop);
    instance.musicList[(int)music].setVolume(instance.musicVolumePct);
    instance.musicMutex.unlock();
}

void Audio::play(const SFX sfx, bool loop) {
    if (loop) {
        stop(sfx);
    }
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
    instance.sfxLastIndex[(int)sfx] = loop ? i : -1;
    instance.playingSounds[i].setBuffer(instance.sfxList[(int)sfx]);
    instance.playingSounds[i].play();
    instance.playingSounds[i].setLoop(loop);
    instance.playingSounds[i].setRelativeToListener(true);
    instance.playingSounds[i].setVolume(instance.sfxVolumePct);
}

bool Audio::isPlaying(const SFX sfx) {
    bool playing;
    instance.sfxMutex.lock();
    int i = instance.sfxLastIndex[(int)sfx];
    if (i >= 0) {
        playing = instance.playingSounds[i].getStatus() ==
                  sf::SoundSource::Status::Playing;
    } else {
        playing = false;
    }
    instance.sfxMutex.unlock();
    return playing;
}

void Audio::stop(const SFX sfx) {
    instance.sfxMutex.lock();
    int i = instance.sfxLastIndex[(int)sfx];
    if (i >= 0) instance.playingSounds[i].stop();
    instance.sfxMutex.unlock();
}

void Audio::fadeOut(const Music music, const sf::Time &deltaTime,
                    const sf::Time &time) {
    float volume =
        instance.musicList[(int)music].getVolume() -
        ((instance.sfxVolumePct / time.asSeconds()) * deltaTime.asSeconds());
    volume = fmax(0.0f, volume);
    instance.musicList[(int)music].setVolume(volume);
}

void Audio::pauseMusic() {
    instance.musicMutex.lock();
    for (int i = 0; i < (int)Music::__COUNT; i++) {
        if (instance.musicList[i].getStatus() ==
            sf::SoundSource::Status::Playing) {
            instance.musicList[i].pause();
        }
    }
    instance.musicMutex.unlock();
}

void Audio::pauseSFX() {
    instance.sfxMutex.lock();
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (instance.playingSounds[i].getStatus() ==
            sf::SoundSource::Status::Playing) {
            instance.playingSounds[i].pause();
        }
    }
    instance.sfxMutex.unlock();
}

void Audio::resumeMusic() {
    instance.musicMutex.lock();
    for (int i = 0; i < (int)Music::__COUNT; i++) {
        if (instance.musicList[i].getStatus() ==
            sf::SoundSource::Status::Paused) {
            instance.musicList[i].play();
        }
    }
    instance.musicMutex.unlock();
}

void Audio::resumeSFX() {
    instance.sfxMutex.lock();
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (instance.playingSounds[i].getStatus() ==
            sf::SoundSource::Status::Paused) {
            instance.playingSounds[i].play();
        }
    }
    instance.sfxMutex.unlock();
}

void Audio::stopSFX() {
    instance.sfxMutex.lock();
    for (int i = 0; i < MAX_SOUNDS; i++) instance.playingSounds[i].stop();
    instance.sfxMutex.unlock();
}

void Audio::stopMusic() {
    instance.musicMutex.lock();
    for (int i = 0; i < (int)Music::__COUNT; i++) {
        instance.musicList[i].stop();
    }
    instance.musicMutex.unlock();
}

float Audio::logFunc(const float value) {
    float ret = -log10f(powf(1 - value * 0.9f, VOLUME_LOG_EXP));
    if (ret > 1.0f) {
        ret = 1.0f;
    }
    return ret;
}

// set volume as percent 0-1
void Audio::setVolume(const float musicVolumePct, const float sfxVolumePct) {
    instance.getMusicValue = musicVolumePct;
    instance.getSFXValue = sfxVolumePct;
    instance.musicVolumePct =
        logFunc(musicVolumePct) * 100.0f * VOLUME_MULTIPLIER;
    instance.sfxVolumePct = logFunc(sfxVolumePct) * 100.0f * VOLUME_MULTIPLIER;
    instance.musicMutex.lock();
    for (int i = 0; i < (int)Music::__COUNT; i++) {
        instance.musicList[i].setVolume(instance.musicVolumePct);
    }
    instance.musicMutex.unlock();
}

void Audio::setPitch(const SFX sfx, const float sfxPitch) {
    int i = instance.sfxLastIndex[(int)sfx];
    instance.playingSounds[i].setPitch(sfxPitch);
}

void Audio::playEngines(unsigned int playerIndex, bool raceMode) {
    instance.playerIndex = playerIndex;
    instance.raceMode = raceMode;
    std::string filename = "assets/sfx/engine.ogg";
    for (unsigned int i = 0; i < (unsigned int)MenuPlayer::__COUNT; i++) {
        auto &engine = instance.sfxEngines[i];
        if (instance.enginesPlaying && i != playerIndex) {
            continue;
        }
        if (!instance.enginesPlaying) {
            engine.openFromFile(filename);
            engine.play();
            engine.setLoop(true);
            setEngineVolume(i);
        }
        if (raceMode || i == playerIndex) {
            engine.setAttenuation(0.60f);
            engine.setMinDistance(1.0f / MAP_TILES_WIDTH * 3.0f);
        } else {
            engine.setAttenuation(0.15f);
            engine.setMinDistance(1.0f / MAP_TILES_WIDTH * 0.15f);
        }
    }
    instance.sfxEngines[playerIndex].setRelativeToListener(raceMode);
    if (raceMode) {
        instance.sfxEngines[playerIndex].setPosition(0.0f, 0.0f, 0.0f);
    }
    instance.enginesPlaying = true;
}

void Audio::playEngines(bool playerOnly) {
    playEngines(instance.playerIndex, playerOnly);
}

void Audio::setEngineVolume(unsigned int i, float volume) {
    auto &engine = instance.sfxEngines[i];
    if (i == instance.playerIndex) {
        engine.setVolume(instance.sfxVolumePct * 0.75f * volume / 100.0f);
    } else {
        engine.setVolume(instance.sfxVolumePct / 1.75f * volume / 100.0f);
    }
}

void Audio::setEnginesVolume(float volume) {
    for (int i = 0; i < (int)MenuPlayer::__COUNT; i++) {
        setEngineVolume(i, volume);
    }
}

void Audio::updateEngine(unsigned int i, sf::Vector2f position, float height,
                         float speedForward, float speedTurn) {
    float maxLinearSpeed = 0.4992f / 2.0f;
    float maxSpeedTurn = 3.6f;
    float pitch = 1.0f;
    pitch += speedForward / maxLinearSpeed;
    pitch -= fabs(speedTurn) / maxSpeedTurn * 0.65;
    if (height > 0.0f) pitch += 0.35f;
    pitch = fmin(pitch, 2.0f);
    if ((unsigned int)i != instance.playerIndex || !instance.raceMode) {
        instance.sfxEngines[i].setPosition(position.x, position.y,
                                           height / 80.0f);
    }
    instance.sfxEngines[i].setPitch(pitch);
}

void Audio::updateEngine(sf::Vector2f position, float height,
                         float speedForward, float speedTurn) {
    updateEngine(instance.playerIndex, position, height, speedForward,
                 speedTurn);
}

void Audio::updateListener(sf::Vector2f position, float angle, float height) {
    sf::Listener::setPosition(position.x, position.y, height / 80.0f);
    sf::Listener::setDirection(-cosf(angle), -sinf(angle), 0.0f);
    sf::Listener::setUpVector(0.0f, 0.0f, 1.0f);
}

void Audio::pauseEngines() {
    for (int i = 0; i < (int)MenuPlayer::__COUNT; i++) {
        instance.sfxEngines[i].pause();
    }
}

void Audio::resumeEngines() {
    for (int i = 0; i < (int)MenuPlayer::__COUNT; i++) {
        instance.sfxEngines[i].play();
    }
}

void Audio::stopEngines() {
    for (auto &engine : instance.sfxEngines) {
        engine.stop();
    }
    instance.enginesPlaying = false;
}
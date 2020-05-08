#pragma once

#include <SFML/Audio.hpp>
#include <mutex>

#include "../entities/enums.h"

// all music annotations are based on this video:
// https://www.youtube.com/watch?v=AlAmXXNz5ac

enum class Music : int {
    MENU_TITLE_SCREEN,        // mario kart main menu
    MENU_PLAYER_CIRCUIT,      // player & circuit/gp selection
    CIRCUIT_ANIMATION_START,  // race start jingle (before semaphore)
    CIRCUIT_NORMAL,           // circuit's theme for laps 1-4
    CIRCUIT_LAST_LAP,         // circuit's theme for lap 5
    CIRCUIT_END_VICTORY,  // finished lap 5, small driver animation & standings
    CIRCUIT_END_DEFEAT,   // finished lap 5 ( but in bad position :-( )
    CONGRATULATIONS_SCREEN,  // you won the grand prix
    __COUNT,
};

enum class SFX : int {
    MENU_INTRO_SCREEN_DING,    // nintendo "ding"
    MENU_SELECTION_ACCEPT,     // title screen (all presses) and player/circuit
                               // (only 'select'/'confirm' actions)
    MENU_SELECTION_MOVE,       // player/circuit (cursor move)
    CIRCUIT_LAKITU_SEMAPHORE,  // 3-2-1 semaphore
    CIRCUIT_LAKITU_WARNING,    // player going backwards
    CIRCUIT_COLLISION,         // player hits an object
    CIRCUIT_PASS_MOTOR,        // played when a player passes you (doppler?)
    CIRCUIT_OVERTAKE_UP,       // played when you overtake a player (ui update)
    CIRCUIT_OVERTAKE_DOWN,     // played when a player overtakes you (ui update)
    CIRCUIT_GOAL_END,          // finished all 5 laps
    // TODO hay sonidos especiales para cada tipo de terreno
    // no se cuantos sonidos diferentes hay, por ahora dejo estos
    CIRCUIT_PLAYER_MOTOR,  // your background motor noise (depends on speed)
    CIRCUIT_PLAYER_MOTOR_SPOOK,  // special motor noise for ghost valley
    CIRCUIT_PLAYER_DRIFT,        // drift noise
    CIRCUIT_PLAYER_DRIFT_SPOOK,  // special drift noise for ghost valley
    CIRCUIT_MATERIAL_GRASS,      // moving over grass terrain
    CIRCUIT_MATERIAL_WOOD,       // moving over wood terrain (normal)
    CIRCUIT_MATERIAL_SPOOK,      // moving over wood terrain (ghost valley)
    // ----------------
    CIRCUIT_PLAYER_JUMP,  // noise before drifting/using jump pad
    CIRCUIT_PLAYER_HIT,   // banana hit
    CIRCUIT_COIN,         // floor coin get
                          // OR player hits you and you lose a coin
    // TODO añadir mas SFX de objetos
    CIRCUIT_ITEM_RANDOMIZING,  // jingle while your object is randomized
    CIRCUIT_ITEM_GET,          // ding ding ding after pressing a question panel
    CIRCUIT_ITEM_USE_LAUNCH,   // use item and launch it forward (ex. banana)
    CIRCUIT_ITEM_USE_UP,       // use item forward (don't launch, ex. shell)
    CIRCUIT_ITEM_USE_DOWN,     // use item and launch it behind you
    CIRCUIT_ITEM_COIN,         // use coin item
    CIRCUIT_ITEM_STAR,         // use star item
    CIRCUIT_ITEM_MUSHROOM,     // use mushroom item
    CIRCUIT_ITEM_THUNDER,      // use mushroom item
    // ------------
    CIRCUIT_LAST_LAP_NOTICE,
    RESULTS_POINTS_UPDATE,  // leaderboard point update animation
    __COUNT,
};

class Audio {
   private:
    std::array<sf::Music, (int)Music::__COUNT> musicList;
    std::array<sf::SoundBuffer, (int)SFX::__COUNT> sfxList;
    std::array<int, (int)SFX::__COUNT> sfxLastIndex;

    std::array<sf::Music, (int)MenuPlayer::__COUNT> sfxEngines;

    std::mutex musicMutex, sfxMutex;
    static const int MAX_SOUNDS = 32;
    std::array<sf::Sound, MAX_SOUNDS> playingSounds;
    int currentSoundIndex = 0;
    int lastDurableSFXIndex = 0;

    static Audio instance;
    float musicVolumePct, sfxVolumePct;

    Audio() {
        musicVolumePct = 50.0f;
        sfxVolumePct = 50.0f;
    }
    static SFX loadDing();  // small sound before everything starts loading :-)
    static void loadAll();  // load rest of the assets meanwhile

    void load(const Music music, const std::string &filename);
    void load(const SFX sfx, const std::string &filename);

    friend class StateInitLoad;

   public:
    static void loadCircuit(const std::string &folder);
    static void play(const Music music);
    static void play(const SFX sfx, bool loop = false);

    static void stopSFX();
    static void stopMusic();

    // set volume as percent 0-1
    static void setVolume(const float musicVolumePct, const float sfxVolumePct);
    static float getMusicVolume() { return instance.musicVolumePct / 100.0f; }
    static float getSfxVolume() { return instance.sfxVolumePct / 100.0f; }

    static void setPitch(const SFX sfx, const float sfxPitch);

    static void playEngines();
    static void updateEngine(unsigned int i, sf::Vector2f position,
                             float height, float speedForward);
    static void updateListener(sf::Vector2f position, float height);
};
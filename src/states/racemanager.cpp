#include "racemanager.h"

// #define NO_ANIMATIONS  // remove race begin/end animations from the game

void StateRaceManager::updatePositions() {
    for (uint i = 0; i < positions.size(); i++) {
        sf::Vector2f pos = Map::getPlayerInitialPosition(i + 1);
        positions[i]->setPositionAndReset(
            sf::Vector2f(pos.x / MAP_ASSETS_WIDTH, pos.y / MAP_ASSETS_HEIGHT));
    }
}

void StateRaceManager::setPlayer() {
    constexpr int count = (int)MenuPlayer::__COUNT;
    // apply player character multiplier to player vehicle
    drivers[(int)selectedPlayer]->vehicle =
        &drivers[(int)selectedPlayer]->vehicle->makePlayer();
    for (int i = 0; i < count; i++) {
#ifdef NO_ANIMATIONS
        drivers[i]->controlType = DriverControlType::DISABLED;
#else
        drivers[i]->controlType = DriverControlType::AI_GRADIENT;
#endif
    }
    // move selected player to last position
    std::swap(positions[(int)selectedPlayer], positions[count - 1]);
    std::random_shuffle(positions.begin(), positions.begin() + (count - 1));
}

void StateRaceManager::init(const float _speedMultiplier,
                            const float _playerCharacterMultiplier,
                            const RaceCircuit _circuit) {
    VehicleProperties::setScaleFactor(_speedMultiplier,
                                      _playerCharacterMultiplier);
    currentCircuit = _circuit;
    for (uint i = 0; i < (uint)MenuPlayer::__COUNT; i++) {
        DriverPtr driver(new Driver(
            DRIVER_ASSET_NAMES[i].c_str(), sf::Vector2f(0.0f, 0.0f),
            M_PI_2 * -1.0f, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
            DriverControlType::DISABLED, *DRIVER_PROPERTIES[i], MenuPlayer(i)));
        drivers[i] = driver;
        positions[i] = driver.get();
        grandPrixRanking[i] = std::make_pair(driver.get(), 0);
    };
    currentState = RaceState::NO_PLAYER;
}

void StateRaceManager::update(const sf::Time &) {
    switch (currentState) {
        case RaceState::NO_PLAYER:
            Audio::play(Music::MENU_PLAYER_CIRCUIT);
            game.pushState(
                StatePtr(new StatePlayerSelection(game, selectedPlayer)));
            currentState = RaceState::YES_PLAYER;
            break;
        case RaceState::YES_PLAYER:
            setPlayer();
            currentState = RaceState::RACING;
            break;
        case RaceState::RACING: {
            int i = (int)currentCircuit;
            currentCircuit = RaceCircuit(i + 1);
            drivers[(uint)selectedPlayer]->controlType =
                DriverControlType::PLAYER;
            Map::loadCourse(CIRCUIT_ASSET_NAMES[i]);
            Audio::loadCircuit(CIRCUIT_ASSET_NAMES[i]);
            updatePositions();
            uint currentPlayerPosition = 0;
            for (uint i = 0; i < positions.size(); i++) {
                if (positions[i]->getPj() == selectedPlayer) {
                    currentPlayerPosition = i;
                    break;
                }
            }
#ifndef NO_ANIMATIONS
            game.pushState(
                StatePtr(new StateRaceEnd(game, drivers[(uint)selectedPlayer],
                                          drivers, selectedPlayer, positions)));
#endif
            game.pushState(StatePtr(new StateRace(
                game, drivers[(uint)selectedPlayer], drivers, positions)));
#ifndef NO_ANIMATIONS
            Audio::play(Music::CIRCUIT_ANIMATION_START);
            Audio::playEngines((int)selectedPlayer);
            game.pushState(StatePtr(new StateRaceStart(
                game, drivers[(uint)selectedPlayer], drivers,
                Map::getPlayerInitialPosition(currentPlayerPosition + 1))));
#endif
            currentState = RaceState::STANDINGS;
        } break;
        case RaceState::STANDINGS: {
            Audio::stopEngines();
            RaceCircuit lastCircuit = RaceCircuit((uint)currentCircuit - 1);
            if ((mode == RaceMode::GRAND_PRIX_1 &&
                 currentCircuit == RaceCircuit::__COUNT) ||
                mode == RaceMode::VERSUS) {
                if (mode == RaceMode::VERSUS) {
                    // "grand prix ranking" equals the race ranking
                    for (uint i = 0; i < grandPrixRanking.size(); i++) {
                        grandPrixRanking[i].first = positions[i];
                    }
                }
                game.pushState(StatePtr(new StateCongratulations(
                    game, lastCircuit, selectedPlayer, grandPrixRanking)));
                currentState = RaceState::DONE;
            }
            if (mode == RaceMode::GRAND_PRIX_1) {
                game.pushState(StatePtr(
                    new StateGPStandings(game, positions, grandPrixRanking,
                                         lastCircuit, selectedPlayer)));
                if (currentCircuit != RaceCircuit::__COUNT) {
                    currentState = RaceState::RACING;
                }
            }
        } break;
        case RaceState::DONE:
            // pop player selection & racemanager
            game.popState();
            game.popState();
            break;
    }
}
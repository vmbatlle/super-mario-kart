#include "racemanager.h"

void StateRaceManager::updatePositions() {
    for (uint i = 0; i < positions.size(); i++) {
        sf::Vector2f pos = Map::getPlayerInitialPosition(i + 1);
        drivers[(int)positions[i]]->position =
            sf::Vector2f(pos.x / MAP_ASSETS_WIDTH, pos.y / MAP_ASSETS_HEIGHT);
    }
}

void StateRaceManager::setPlayer() {
    constexpr int count = (int)MenuPlayer::__COUNT;
    for (int i = 0; i < count; i++) {
        positions[i] = MenuPlayer(i);
        drivers[i]->controlType = DriverControlType::AI_GRADIENT;
    }
    // move selected player to last position
    std::swap(positions[(int)selectedPlayer], positions[count - 1]);
    std::random_shuffle(positions.begin(), positions.begin() + (count - 1));
}

void StateRaceManager::init() {
    currentCircuit = 0;
    currentPlayerPosition = 7;  // last place
    for (uint i = 0; i < (uint)MenuPlayer::__COUNT; i++) {
        DriverPtr driver(new Driver(
            DRIVER_ASSET_NAMES[i].c_str(), sf::Vector2f(0.0f, 0.0f),
            M_PI_2 * -1.0f, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
            DriverControlType::DISABLED, *DRIVER_PROPERTIES[i], MenuPlayer(i)));
        drivers.push_back(driver);
    };
    currentState = RaceState::NO_PLAYER;
}

void StateRaceManager::update(const sf::Time &) {
    switch (currentState) {
        case RaceState::NO_PLAYER:
            game.pushState(
                StatePtr(new StatePlayerSelection(game, selectedPlayer)));
            currentState = RaceState::YES_PLAYER;
            break;
        case RaceState::YES_PLAYER:
            setPlayer();
            currentState = RaceState::RACING;
            break;
        case RaceState::RACING:
            int i = currentCircuit++;
            drivers[(uint)selectedPlayer]->controlType =
                DriverControlType::PLAYER;
            Map::loadCourse(CIRCUIT_NAMES[i]);
            updatePositions();
            game.pushState(
                StatePtr(new StateRaceEnd(game, drivers[(uint)selectedPlayer],
                                          drivers, selectedPlayer, positions)));
            game.pushState(StatePtr(new StateRace(
                game, drivers[(uint)selectedPlayer], drivers, positions)));
            game.pushState(StatePtr(new StateRaceStart(
                game, drivers,
                Map::getPlayerInitialPosition(currentPlayerPosition + 1))));
            break;
    }
}
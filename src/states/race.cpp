#include "race.h"

void StateRace::init() {
    playerCps = std::vector<bool>(Map::numCheckpoints());
    for (bool cp : playerCps) cp = false;
}

void StateRace::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        // TODO this is example code
        // we can handle item usage by creating an item entity
        // and removing the player's held item
    }
}

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    player->update(deltaTime);

    checkpointUpdate();
    
    //Meta condition
    if (playerPassedCps >= Map::numCheckpoints() && Map::inMeta(player.get()->position)) {
        player->rounds++;
        playerPassedCps = 0;
        for (bool cp : playerCps) cp = false;
        lakitu.onScreen = true;
        lakitu.update(deltaTime);
    }

}

void StateRace::draw(sf::RenderTarget& window) {
    Map::drawMap(player->position, player->posAngle, window);
    player->draw(window);
    if (lakitu.onScreen) {
        lakitu.draw(window);
    }
}

void StateRace::checkpointUpdate() {
    sf::Vector2f ppos = player->position;

    int i = 0;
    for (sf::FloatRect cp : Map::getCheckpoints()) {
        if (!playerCps[i] && cp.contains(ppos))  {
            playerCps[i] = true;
            playerPassedCps++;
        }
    }

}
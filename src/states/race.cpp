#include "race.h"

void StateRace::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        // TODO this is example code
        // we can handle item usage by creating an item entity
        // and removing the player's held item
    }
}

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    player->update(deltaTime);
    
    //Meta condition
    if (Map::inMeta(player.get()->position)) {
        player->rounds++;
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
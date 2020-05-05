#include "racepause.h"

void StateRacePause::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::CONTINUE, event)) {
        game.popState();
    }
}

void StateRacePause::fixedUpdate(const sf::Time& /*deltaTime*/) {
    // TODO igual no se necesita esto, solo handleEvent (?)
    // a no ser que queramos hacer que la opcion actual parpadee o algo
}

void StateRacePause::draw(sf::RenderTarget& window) {
    // TODO
    window.clear(sf::Color::Black);

    static int i = 0;
    if ((i++ / 10) % 2 == 0) {
        sf::Sprite backgroundSprite(backgroundTexture);
        backgroundSprite.setOrigin(0.0f, backgroundTexture.getSize().y);
        backgroundSprite.scale(1.0f, -1.0f);
        window.draw(backgroundSprite);
    }
}
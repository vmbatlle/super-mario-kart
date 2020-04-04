#include "gui.h"

Gui Gui::instance;

Gui::Gui() {

    winSize = sf::Vector2u(0,0);

}

void Gui::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
    instance.timer.setWindowSize(s);
    instance.itemInd.setPosition(instance.timer.getItemPos());
    instance.others.setWindowSize(s);
}

void Gui::setPowerUp(PowerUps power) {
    instance.itemInd.setItem(power);
}

void Gui::addCoin() {
    instance.others.addCoin();
}

void Gui::update(const sf::Time &deltaTime) {
    instance.timer.update(deltaTime);
    instance.itemInd.update(deltaTime);
    instance.others.update(deltaTime);
}

void Gui::draw(sf::RenderTarget &window) {
    instance.timer.draw(window);
    instance.itemInd.draw(window);
    instance.others.draw(window);
}
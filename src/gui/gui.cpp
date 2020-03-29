#include "gui.h"

Gui::Gui() {

    winSize = sf::Vector2u(0,0);

}

void Gui::setWindowSize(sf::Vector2u s) {
    winSize = s;
    timer.setWindowSize(s);
    itemInd.setPosition(timer.getItemPos());
}


void Gui::update(const sf::Time &deltaTime) {
    timer.update(deltaTime);
    itemInd.update(deltaTime);
}

void Gui::draw(sf::RenderTarget &window) {
    timer.draw(window);
    itemInd.draw(window);
}
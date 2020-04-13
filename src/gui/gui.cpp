#include "gui.h"

Gui Gui::instance;

Gui::Gui() {

    winSize = sf::Vector2u(0,0);
    instance.whiteScreen.setFillColor(sf::Color(255,255,255,175));
    instance.thunderTime = 0;
    instance.drawThunder = false;

}

void Gui::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
    instance.timer.setWindowSize(s);
    instance.itemInd.setPosition(instance.timer.getItemPos());
    instance.others.setWindowSize(s);
    instance.whiteScreen.setSize(sf::Vector2f(s));
}

void Gui::setPowerUp(PowerUps power) {
    instance.itemInd.setItem(power);
}

void Gui::addCoin(int ammount) {
    instance.others.addCoin(ammount);
}

void Gui::setRanking(int r) {
    instance.others.setRanking(r);
}

void Gui::thunder() {
    instance.thunderTime = 0.2;
}

void Gui::update(const sf::Time &deltaTime) {
    instance.timer.update(deltaTime);
    instance.itemInd.update(deltaTime);
    instance.others.update(deltaTime);
    if (instance.thunderTime > 0) {
        instance.drawThunder = !instance.drawThunder;
        instance.thunderTime -= deltaTime.asSeconds();
    } else {
        instance.drawThunder = false;
    }
}

void Gui::draw(sf::RenderTarget &window) {
    if (instance.drawThunder) {
        window.draw(instance.whiteScreen);
    }
    instance.timer.draw(window);
    instance.itemInd.draw(window);
    instance.others.draw(window);
}
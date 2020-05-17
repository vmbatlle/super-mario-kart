#include "gui.h"

Gui Gui::instance;

Gui::Gui() { winSize = sf::Vector2u(0, 0); }

void Gui::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
    instance.timer.setWindowSize(s);
    instance.itemInd.setPosition(s, instance.timer.getItemPos());
    instance.others.setWindowSize(s);
    instance.effects.setWindowSize(s);
}

void Gui::setPowerUp(PowerUps power) { instance.itemInd.setItem(power); }

void Gui::addCoin(int ammount) { instance.others.addCoin(ammount); }

void Gui::setRanking(int r) { instance.others.setRanking(r); }

void Gui::thunder() { instance.effects.thunder(0.2); }

void Gui::speed(float time) { instance.effects.speed(time); }

void Gui::fade(float time, bool fromBlack) {
    instance.effects.blackFade(time, fromBlack);
}

bool Gui::canUseItem() { return !instance.itemInd.spinning; }

bool Gui::isBlackScreen(bool total) {
    if (total)
        return instance.effects.blackScreen.getFillColor().a > 252;
    else
        return instance.effects.blackScreen.getFillColor().a > 1;
}

void Gui::update(const sf::Time &deltaTime) {
    instance.timer.update(deltaTime);
    instance.itemInd.update(deltaTime);
    instance.others.update(deltaTime);
    instance.effects.update(deltaTime);
}

void Gui::draw(sf::RenderTarget &window, const sf::Color &timerColor) {
    instance.effects.draw(window);
    instance.timer.draw(window, timerColor);
    instance.itemInd.draw(window);
    instance.others.draw(window);
}

void Gui::endRace() { instance.others.setRanking(instance.others.rank, true); }

void Gui::reset(bool rankReset) {
    instance.timer.reset();
    instance.others.reset(rankReset);
    instance.itemInd.reset();
    instance.effects.reset();
}

void Gui::stopEffects() { instance.effects.stop(); }
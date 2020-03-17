#include "lakitu.h"

Lakitu::Lakitu() {
    std::string spriteFile = "assets/misc/lakitu.png";
    for (int i = 0; i < 3; i++)
        finish[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 3; i < 5; i++)
        wrongDir[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 0; i < 4; i++)
       laps[i].loadFromFile(spriteFile, sf::IntRect(43 + (i * 25), 76, 24, 16));
    for (int i = 0; i < 4; i++)
        lights[i].loadFromFile(spriteFile, sf::IntRect(149 + (i * 9), 69, 8, 24));
    lakituLaps.loadFromFile(spriteFile, sf::IntRect(11, 68, 22, 31));
    lakituCatchPlayer.loadFromFile(spriteFile, sf::IntRect(75, 35, 32, 32));

    sprite.setTexture(finish[0]);
    sprite.setPosition(0,0);
    sprite.setScale(2,2);

    state = LakituState::SLEEP;
    screenTime = 0;
    lap = 0;

}

void Lakitu::showLap(int numLap) {
    state = LakituState::LAP;
    sprite.setPosition(0,0);
    lap = numLap;
}

void Lakitu::showFinish() {
    state = LakituState::FINISH;
    sprite.setPosition(0,0);
}

void Lakitu::showUntil(float seconds, const sf::Time &deltaTime) {
    screenTime += deltaTime.asSeconds();
    if (screenTime > seconds) { 
        state = LakituState::SLEEP;
        screenTime = 0;
    }
}


void Lakitu::update(const sf::Time&) {
    // switch(state) {
    //     case LakituState::START:

    //         showUntil(5, deltaTime);
    //         break;

    //     case LakituState::FINISH:
            
    //         showUntil(5, deltaTime);
    //         break;

    //     case LakituState::WORNG_DIR:

    //         showUntil(5, deltaTime);
    //         break;
        
    //     case LakituState::LAP: 
    //         {
    //             sprite.setTexture(lakituLaps);
    //             object.setTexture(laps[lap-2]);

    //             sprite.move(1,0);

    //             sf::Vector2f lakiPos = sprite.getPosition();
    //             object.setPosition(lakiPos.x, lakiPos.y);

    //             showUntil(5, deltaTime);
    //         }
    //         break;
        
    //     case LakituState::SLEEP:
            
    //         break;

    //     default:
    //         break;
            
    // }
}

void Lakitu::draw(sf::RenderTarget &window) {
    if ( state != LakituState::SLEEP ) {
        sprite.setPosition(window.getSize().x/5.f, window.getSize().y/6.f);
        window.draw(sprite);
        if (state == LakituState::LAP || state == LakituState::START) {
           window.draw(object); 
        }
    }
}
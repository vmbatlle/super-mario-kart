#include "lakitu.h"

Lakitu::Lakitu() {
    std::string spriteFile = "assets/misc/lakitu.png";
    for (int i = 0; i < 3; i++)
        finish[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 3; i < 5; i++)
        wrongDir[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 0; i < 3; i++)
        laps[i].loadFromFile(spriteFile, sf::IntRect(43 + (i * 25), 76, 24, 16));
    laps[3].loadFromFile(spriteFile, sf::IntRect(118, 76, 30, 16));
    for (int i = 0; i < 2; i++)
        start[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 42), 34, 32, 32));
    for (int i = 0; i < 4; i++)
        lights[i].loadFromFile(spriteFile, sf::IntRect(149 + (i * 9), 69, 8, 24));
    lakituLaps.loadFromFile(spriteFile, sf::IntRect(10, 68, 24, 32));
    lakituCatchPlayer.loadFromFile(spriteFile, sf::IntRect(75, 35, 32, 32));

    sprite.setTexture(start[0]);
    sprite.setPosition(winSize.x/4, -10);
    sprite.setScale(2,2);

    state = LakituState::SLEEP;

    screenTime = 0;

    nextFrameTime = 0.5;
    frameTime = 0;

    lap = 2;
    light = 0;

    textIndex = 0;

    winSize = sf::Vector2u(0,0);

}

void Lakitu::setWindowSize(sf::Vector2u s) {
    winSize = s;
}

void Lakitu::showStart() {
    state = LakituState::START;
    sprite.setTexture(start[0]);

    sprite.setOrigin(start[0].getSize().x/2, start[0].getSize().y/2);

    sprite.scale(-1,1);
    sprite.setPosition(winSize.x/4, -10);

    light = 0;
    lightSprite.setTexture(lights[light]);
    lightSprite.setOrigin(lights[light].getSize().x/2, 0);
    lightSprite.setScale(2,2);

    nextFrameTime = 1;
}

void Lakitu::showLap(int numLap) {
    state = LakituState::LAP;
    sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2);
    signSprite.setTexture(laps[numLap-2]);
    signSprite.setOrigin(signSprite.getLocalBounds().width/2, signSprite.getLocalBounds().height/2);
    signSprite.setScale(2,2);

    lap = numLap;
    nextFrameTime = 0.5;
}

void Lakitu::showFinish() {
    state = LakituState::FINISH;
    sprite.setOrigin(finish[0].getSize().x/2, finish[0].getSize().y/2);
    sprite.setPosition(0,0);
    nextFrameTime = 0.5;
}

void Lakitu::showUntil(float seconds, const sf::Time &deltaTime) {
    screenTime += deltaTime.asSeconds();
    if (screenTime > seconds) { 
        state = LakituState::SLEEP;
        screenTime = 0;
        textIndex = 0;
    }
}


void Lakitu::update(const sf::Time &deltaTime) {
    switch(state) {
        case LakituState::START: 
            {
                frameTime += deltaTime.asSeconds();
                if (frameTime >= nextFrameTime) {
                    textIndex++;
                    frameTime = 0;
                }
                
                if (textIndex < 1)
                    sprite.move(0,1);

                if (textIndex > 1 && textIndex < 5) {
                    lightSprite.setTexture(lights[textIndex-1]);
                }
                if (textIndex >= 4) 
                    sprite.setTexture(start[1]);
                if (textIndex >= 5)
                    sprite.move(0,-1);

                sf::Vector2f lakiPos = sprite.getPosition();
                if (textIndex >= 4)
                    lightSprite.setPosition(lakiPos.x+31, lakiPos.y);
                else
                    lightSprite.setPosition(lakiPos.x+29, lakiPos.y);

                showUntil(10, deltaTime);
            }
            break;

        case LakituState::FINISH:
            {
                sprite.setTexture(finish[finishAnim[textIndex % 4]]);
                frameTime += deltaTime.asSeconds();
                if (frameTime >= nextFrameTime) {
                    textIndex++;
                    frameTime = 0;
                }

                // x^2/4 + 0.1
                float x = screenTime/ 5;
                float y = (-(x * x)/4) - 0.1;
                sprite.setPosition(x * winSize.x, y * winSize.y + (winSize.y * 2)/3);
                showUntil(5, deltaTime);
            }
            break;

        case LakituState::WORNG_DIR:
            {
                sprite.setTexture(wrongDir[textIndex % 2]);
                frameTime += deltaTime.asSeconds();
                if (frameTime >= nextFrameTime) {
                    textIndex++;
                    frameTime = 0;
                }

                // x^2/4 + 0.1
                float x = screenTime/ 5;
                float y = (-(x * x)/4) - 0.2;
                sprite.setPosition(x * winSize.x, y * winSize.y + winSize.y/2);
                showUntil(5, deltaTime);
            }

            showUntil(5, deltaTime);
            break;
        
        case LakituState::LAP: 
            {
                sprite.setTexture(lakituLaps);

                // x^2/4 + 0.1
                float x = screenTime/ 5;
                float y = (-(x * x)/4) - 0.1;
                sprite.setPosition(x * winSize.x, y * winSize.y + winSize.y/3);

                sf::Vector2f lakiPos = sprite.getPosition();
                signSprite.setPosition(lakiPos.x + 5 , lakiPos.y-17);

                showUntil(5, deltaTime);
            }
            break;
        
        case LakituState::SLEEP:
            
            break;

        default:
            break;
            
    }
}

void Lakitu::draw(sf::RenderTarget &window) {
    if ( state != LakituState::SLEEP ) {
        window.draw(sprite);
        if (state == LakituState::START)
           window.draw(lightSprite); 
        else if (state == LakituState::LAP)
            window.draw(signSprite); 
    }
}
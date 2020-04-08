#include "lakitu.h"

Lakitu Lakitu::instance;

Lakitu::Lakitu() {
    std::string spriteFile = "assets/misc/lakitu.png";
    for (int i = 0; i < 3; i++)
        instance.finish[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 3; i < 5; i++)
        instance.wrongDir[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 0; i < 3; i++)
        instance.laps[i].loadFromFile(spriteFile, sf::IntRect(43 + (i * 25), 76, 24, 16));
    instance.laps[3].loadFromFile(spriteFile, sf::IntRect(118, 76, 30, 16));
    for (int i = 0; i < 2; i++)
        instance.start[i].loadFromFile(spriteFile, sf::IntRect(1 + (i * 42), 34, 32, 32));
    for (int i = 0; i < 4; i++)
        instance.lights[i].loadFromFile(spriteFile, sf::IntRect(149 + (i * 9), 69, 8, 24));
    instance.lakituLaps.loadFromFile(spriteFile, sf::IntRect(10, 68, 24, 32));
    instance.lakituCatchPlayer.loadFromFile(spriteFile, sf::IntRect(75, 35, 32, 32));

    instance.sprite.setTexture(start[0]);
    instance.sprite.setPosition(winSize.x/4, -20);
    instance.sprite.setScale(2,2);

    instance.lightSprite.setPosition(-20, -20);
    instance.signSprite.setPosition(-20, -20);

    instance.state = LakituState::SLEEP;

    instance.screenTime = 0;

    instance.nextFrameTime = 0.5;
    instance.frameTime = 0;

    instance.lap = 2;
    instance.light = 0;
    instance.started = false;

    instance.textIndex = 0;

    instance.winSize = sf::Vector2u(0,0);

}

void Lakitu::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
}

void Lakitu::showStart() {
    instance.state = LakituState::START;
    instance.sprite.setTexture(instance.start[0]);

    instance.sprite.setOrigin(instance.start[0].getSize().x/2, instance.start[0].getSize().y/2);

    instance.sprite.scale(-1,1);
    instance.sprite.setPosition(instance.winSize.x/4, -20);

    instance.light = 0;
    instance.lightSprite.setTexture(instance.lights[instance.light]);
    instance.lightSprite.setOrigin(instance.lights[instance.light].getSize().x/2, 0);
    instance.lightSprite.setScale(2,2);

    instance.nextFrameTime = 1;
}

void Lakitu::showLap(int numLap) {
    instance.state = LakituState::LAP;
    instance.sprite.setOrigin(instance.sprite.getLocalBounds().width/2, instance.sprite.getLocalBounds().height/2);
    instance.signSprite.setTexture(instance.laps[numLap-2]);
    instance.signSprite.setOrigin(instance.signSprite.getLocalBounds().width/2, instance.signSprite.getLocalBounds().height/2);
    instance.signSprite.setScale(2,2);

    instance.lap = numLap;
    instance.nextFrameTime = 0.5;
}

void Lakitu::showFinish() {
    instance.state = LakituState::FINISH;
    instance.sprite.setOrigin(instance.finish[0].getSize().x/2, instance.finish[0].getSize().y/2);
    instance.sprite.setPosition(0,0);
    instance.nextFrameTime = 0.5;
}

bool Lakitu::hasStarted(){
    return instance.started;
}

bool Lakitu::isSleeping(){
    return instance.state == Lakitu::LakituState::SLEEP;
}

void Lakitu::showUntil(float seconds, const sf::Time &deltaTime) {
    instance.screenTime += deltaTime.asSeconds();
    if (instance.screenTime > seconds) { 
        instance.state = LakituState::SLEEP;
        instance.screenTime = 0;
        instance.textIndex = 0;
    }
}


void Lakitu::update(const sf::Time &deltaTime) {
    switch(instance.state) {
        case LakituState::START: 
            {
                instance.frameTime += deltaTime.asSeconds();
                if (instance.frameTime >= instance.nextFrameTime) {
                    instance.textIndex++;
                    instance.frameTime = 0;
                }
                
                if (instance.textIndex < 1)
                    instance.sprite.move(0,1);

                if (instance.textIndex > 1 && instance.textIndex < 5) {
                    instance.lightSprite.setTexture(instance.lights[instance.textIndex-1]);
                }
                if (instance.textIndex >= 4) {
                    instance.sprite.setTexture(instance.start[1]);
                    instance.started = true;
                }
                if (instance.textIndex >= 5)
                    instance.sprite.move(0,-1);

                sf::Vector2f lakiPos = instance.sprite.getPosition();
                if (instance.textIndex >= 4)
                    instance.lightSprite.setPosition(lakiPos.x+31, lakiPos.y);
                else
                    instance.lightSprite.setPosition(lakiPos.x+29, lakiPos.y);

                instance.showUntil(10, deltaTime);
            }
            break;

        case LakituState::FINISH:
            {
                instance.sprite.setTexture(instance.finish[instance.finishAnim[instance.textIndex % 4]]);
                instance.frameTime += deltaTime.asSeconds();
                if (instance.frameTime >= instance.nextFrameTime) {
                    instance.textIndex++;
                    instance.frameTime = 0;
                }

                // x^2/4 + 0.1
                float x = instance.screenTime/ 5;
                float y = (-(x * x)/4) - 0.1;
                instance.sprite.setPosition(x * instance.winSize.x, y * instance.winSize.y + (instance.winSize.y * 2)/3);
                instance.showUntil(5, deltaTime);
            }
            break;

        case LakituState::WORNG_DIR:
            {
                instance.sprite.setTexture(instance.wrongDir[instance.textIndex % 2]);
                instance.frameTime += deltaTime.asSeconds();
                if (instance.frameTime >= instance.nextFrameTime) {
                    instance.textIndex++;
                    instance.frameTime = 0;
                }

                // x^2/4 + 0.1
                float x = instance.screenTime/ 5;
                float y = (-(x * x)/4) - 0.2;
                instance.sprite.setPosition(x * instance.winSize.x, y * instance.winSize.y + instance.winSize.y/2);
                instance.showUntil(5, deltaTime);
            }

            instance.showUntil(5, deltaTime);
            break;
        
        case LakituState::LAP: 
            {
                instance.sprite.setTexture(instance.lakituLaps);

                // x^2/4 + 0.1
                float x = instance.screenTime/ 5;
                float y = (-(x * x)/4) - 0.1;
                instance.sprite.setPosition(x * instance.winSize.x, y * instance.winSize.y + instance.winSize.y/3);

                sf::Vector2f lakiPos = instance.sprite.getPosition();
                instance.signSprite.setPosition(lakiPos.x + 5 , lakiPos.y-17);

                instance.showUntil(5, deltaTime);
            }
            break;
        
        case LakituState::SLEEP:
            
            break;

        default:
            break;
            
    }
}

void Lakitu::draw(sf::RenderTarget &window) {
    if ( instance.state != LakituState::SLEEP ) {
        window.draw(instance.sprite);
        if (instance.state == LakituState::START)
           window.draw(instance.lightSprite); 
        else if (instance.state == LakituState::LAP)
            window.draw(instance.signSprite); 
    }
}
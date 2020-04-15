#include "lakitu.h"

Lakitu Lakitu::instance;

Lakitu::Lakitu() {
    std::string spriteFile = "assets/misc/lakitu.png";
    for (int i = 0; i < 3; i++)
        instance.finish[i].loadFromFile(spriteFile,
                                        sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 3; i < 5; i++)
        instance.wrongDir[i - 3].loadFromFile(
            spriteFile, sf::IntRect(1 + (i * 41), 1, 40, 32));
    for (int i = 0; i < 5; i++)
        instance.laps[i].loadFromFile(spriteFile,
                                      sf::IntRect(43 + (i * 31), 76, 30, 16));
    for (int i = 0; i < 2; i++)
        instance.start[i].loadFromFile(spriteFile,
                                       sf::IntRect(1 + (i * 42), 34, 40, 32));
    for (int i = 0; i < 4; i++)
        instance.lights[i].loadFromFile(spriteFile,
                                        sf::IntRect(168 + (i * 9), 69, 8, 24));
    instance.lakituLaps.loadFromFile(spriteFile, sf::IntRect(3, 68, 40, 32));
    instance.lakituCatchPlayer.loadFromFile(spriteFile,
                                            sf::IntRect(89, 34, 40, 32));

    instance.sprite.setTexture(start[0]);
    instance.sprite.setPosition(winSize.x / 4, -20);
    instance.sprite.setScale(2, 2);

    instance.lightSprite.setPosition(-20, -20);
    instance.signSprite.setPosition(-20, -20);

    instance.state = LakituState::SLEEP;

    instance.screenTime = 0;

    instance.nextFrameTime = 0.5;
    instance.frameTime = 0;

    instance.lap = 2;
    instance.light = 0;
    instance.started = false;

    instance.inAnimation = false;

    instance.textIndex = 0;

    instance.winSize = sf::Vector2u(0, 0);
}

void Lakitu::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
    instance.sprite.setPosition(instance.winSize.x / 4, -20);
}

void Lakitu::showStart() {
    instance.frameTime = 0;
    instance.screenTime = 0;
    instance.textIndex = 0;
    instance.started = false;
    instance.state = LakituState::START;
    instance.sprite.setTexture(instance.start[0]);

    instance.sprite.setOrigin(instance.start[0].getSize().x / 2,
                                instance.start[0].getSize().y / 2);

    instance.sprite.setScale(-2, 2);
    instance.sprite.setPosition(instance.winSize.x / 4, -20);

    instance.light = 0;
    instance.lightSprite.setTexture(instance.lights[instance.light]);
    instance.lightSprite.setOrigin(
        instance.lights[instance.light].getSize().x / 2, 0);
    instance.lightSprite.setScale(2, 2);

    instance.nextFrameTime = 1;
    instance.inAnimation = true;
}

void Lakitu::showLap(int numLap) {
    if (numLap <= 5 && numLap >= 2) {
        instance.frameTime = 0;
        instance.screenTime = 0;
        instance.textIndex = 0;
        instance.state = LakituState::LAP;
        instance.sprite.setOrigin(instance.sprite.getLocalBounds().width / 2,
                                  instance.sprite.getLocalBounds().height / 2);
        instance.signSprite.setTexture(instance.laps[numLap - 2]);
        instance.signSprite.setOrigin(
            instance.signSprite.getLocalBounds().width / 2,
            instance.signSprite.getLocalBounds().height / 2);
        instance.signSprite.setScale(2, 2);

        instance.lap = numLap;
        instance.nextFrameTime = 0.5;
        instance.inAnimation = true;
    }
}

void Lakitu::showFinish() {
    instance.frameTime = 0;
    instance.screenTime = 0;
    instance.textIndex = 0;
    instance.started = false;
    instance.state = LakituState::FINISH;
    instance.sprite.setOrigin(instance.finish[0].getSize().x / 2,
                                instance.finish[0].getSize().y / 2);
    instance.sprite.setPosition(0, 0);
    instance.nextFrameTime = 0.5;
    instance.inAnimation = true;
}

void Lakitu::setWrongDir(bool wrongDir) {
    if (wrongDir && !instance.inAnimation) {
        if (instance.state != LakituState::WRONG_DIR) {
            instance.state = LakituState::WRONG_DIR;
            instance.sprite.setOrigin(instance.wrongDir[0].getSize().x / 2,
                                      instance.wrongDir[0].getSize().y / 2);
            instance.sprite.setPosition(0, 0);
            instance.nextFrameTime = 0.5;
        }
    } else {
        sleep();
    }
}

bool Lakitu::hasStarted() { return instance.started; }

bool Lakitu::isSleeping() {
    return instance.state == Lakitu::LakituState::SLEEP;
}

void Lakitu::sleep() {
    if (!instance.inAnimation) {
        instance.state = LakituState::SLEEP;
        instance.sprite.setPosition(-20, -20);
        instance.screenTime = 0;
        instance.textIndex = 0;
    }
}

void Lakitu::showUntil(float seconds, const sf::Time &) {
    if (instance.screenTime > seconds) {
        instance.state = LakituState::SLEEP;
        instance.screenTime = 0;
        instance.textIndex = 0;
        instance.inAnimation = false;
    }
}

void Lakitu::update(const sf::Time &deltaTime) {
    switch (instance.state) {
        case LakituState::START: {
            instance.frameTime += deltaTime.asSeconds();
            if (instance.frameTime >= instance.nextFrameTime) {
                instance.textIndex++;
                instance.frameTime = 0;
            }

            if (instance.textIndex < 1) instance.sprite.move(0, 1);

            if (instance.textIndex > 1 && instance.textIndex < 5) {
                instance.lightSprite.setTexture(
                    instance.lights[instance.textIndex - 1]);
            }
            if (instance.textIndex >= 4) {
                instance.sprite.setTexture(instance.start[1]);
                instance.started = true;
            }
            if (instance.textIndex >= 5) instance.sprite.move(0, -1);

            sf::Vector2f lakiPos = instance.sprite.getPosition();
            if (instance.textIndex >= 4)
                instance.lightSprite.setPosition(lakiPos.x + 39, lakiPos.y);
            else
                instance.lightSprite.setPosition(lakiPos.x + 37, lakiPos.y);

            instance.screenTime += deltaTime.asSeconds();
            instance.showUntil(8, deltaTime);
        } break;

        case LakituState::FINISH: {
            instance.sprite.setTexture(
                instance.finish[instance.finishAnim[instance.textIndex % 4]]);
            instance.frameTime += deltaTime.asSeconds();
            if (instance.frameTime >= instance.nextFrameTime) {
                instance.textIndex++;
                instance.frameTime = 0;
            }

            // x^2/4 + 0.1
            float x = instance.screenTime / 6;
            float y = (-(x * x) / 4) - 0.1;
            instance.sprite.setPosition(
                x * instance.winSize.x,
                y * instance.winSize.y + instance.winSize.y / 3);

            instance.screenTime += deltaTime.asSeconds();
            instance.showUntil(6, deltaTime);
        } break;

        case LakituState::WRONG_DIR: {
            instance.sprite.setTexture(
                instance.wrongDir[instance.textIndex % 2]);
            instance.frameTime += deltaTime.asSeconds();
            if (instance.frameTime >= instance.nextFrameTime) {
                instance.textIndex++;
                instance.frameTime = 0;
            }

            float time = instance.screenTime;
            if (instance.screenTime > 5)
                time = instance.screenTime - (instance.screenTime - 5);
            else if (instance.screenTime > 10)
                time = 0;
            float x = time / 5;
            float y = instance.winSize.y * 0.2;

            instance.screenTime += deltaTime.asSeconds();
            instance.sprite.setPosition(
                x * instance.winSize.x / 3 + instance.winSize.x / 3, y);
        }

        break;

        case LakituState::LAP: {
            instance.sprite.setTexture(instance.lakituLaps);

            // x^2/4 + 0.1
            float x = instance.screenTime / 5;
            float y = (-(x * x) / 4) - 0.1;
            instance.sprite.setPosition(
                x * instance.winSize.x,
                y * instance.winSize.y + instance.winSize.y / 3);

            sf::Vector2f lakiPos = instance.sprite.getPosition();
            instance.signSprite.setPosition(lakiPos.x + 5, lakiPos.y - 17);

            instance.screenTime += deltaTime.asSeconds();
            instance.showUntil(5, deltaTime);
        } break;

        case LakituState::SLEEP:

            break;

        default:
            break;
    }
}

void Lakitu::draw(sf::RenderTarget &window) {
    if (instance.state != LakituState::SLEEP) {
        window.draw(instance.sprite);
        if (instance.state == LakituState::START)
            window.draw(instance.lightSprite);
        else if (instance.state == LakituState::LAP)
            window.draw(instance.signSprite);
    }
}
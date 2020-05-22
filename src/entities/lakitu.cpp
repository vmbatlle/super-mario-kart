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

    // load its own shadow
    assetShadow.loadFromFile("assets/misc/shadow.png");
    spriteShadow.setTexture(assetShadow);
    sf::Vector2u shadowSize = assetShadow.getSize();
    spriteShadow.setOrigin(shadowSize.x / 2.0f, shadowSize.y);

    instance.sScale = 2;

    instance.sprite.setTexture(start[0]);
    instance.sprite.setPosition(winSize.x / 4, -20);
    instance.sprite.setScale(instance.sScale, instance.sScale);

    instance.lightSprite.setPosition(-20, -20);
    instance.signSprite.setPosition(-20, -20);

    instance.state = LakituState::SLEEP;

    instance.screenTime = 0;

    instance.nextFrameTime = 0.5;
    instance.frameTime = 0;

    instance.lap = 2;
    instance.light = 0;
    instance.started = false;

    instance.drawShadows = true;
    instance.soundStarted = false;

    instance.currentAnimationPriority = 0;

    // Animation Priorities
    animationPriorities[(int)LakituState::START] = 8;
    animationPriorities[(int)LakituState::WRONG_DIR] = 0;
    animationPriorities[(int)LakituState::PICKUP] = 9;
    animationPriorities[(int)LakituState::LAP] = 5;
    animationPriorities[(int)LakituState::FINISH] = 10;
    animationPriorities[(int)LakituState::SLEEP] = 0;

    instance.textIndex = 0;

    instance.winSize = sf::Vector2u(0, 0);
}

void Lakitu::setWindowSize(sf::Vector2u s) {
    instance.winSize = s;
    instance.sprite.setPosition(instance.winSize.x / 4, -20);

    float xFactor = s.x / BASIC_WIDTH;
    instance.sScale = 2 * xFactor;  // 2 for a good lakitu size
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

    instance.sprite.setScale(-instance.sScale, instance.sScale);
    instance.sprite.setPosition(instance.winSize.x / 4, -20);

    instance.light = 0;
    instance.lightSprite.setTexture(instance.lights[instance.light]);
    instance.lightSprite.setOrigin(
        instance.lights[instance.light].getSize().x / 2, 0);
    instance.lightSprite.setScale(instance.sScale, instance.sScale);

    instance.nextFrameTime = 1;
    instance.currentAnimationPriority =
        instance.animationPriorities[(int)LakituState::START];
}

void Lakitu::showLap(int numLap) {
    if (numLap <= 5 && numLap >= 2 &&
        instance.animationPriorities[(int)LakituState::LAP]) {
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
        instance.signSprite.setScale(instance.sScale, instance.sScale);

        instance.lap = numLap;
        instance.nextFrameTime = 0.5;
        instance.currentAnimationPriority =
            instance.animationPriorities[(int)LakituState::LAP];
    }
}

void Lakitu::showFinish() {
    instance.frameTime = 0;
    instance.screenTime = 0;
    instance.textIndex = 0;
    instance.started = false;
    instance.state = LakituState::FINISH;
    instance.sprite.setOrigin(instance.finish[0].getSize().x / 1.5,
                              instance.finish[0].getSize().y / 2);
    instance.sprite.setPosition(0, 0);
    instance.nextFrameTime = 0.5;
    instance.currentAnimationPriority =
        instance.animationPriorities[(int)LakituState::FINISH];
}

void Lakitu::setWrongDir(bool wrongDir) {
    if (wrongDir &&
        instance.currentAnimationPriority <=
            instance.animationPriorities[(int)LakituState::WRONG_DIR]) {
        if (instance.state != LakituState::WRONG_DIR) {
            instance.currentAnimationPriority =
                instance.animationPriorities[(int)LakituState::WRONG_DIR];

            Audio::play(SFX::CIRCUIT_LAKITU_WARNING, true);
            instance.state = LakituState::WRONG_DIR;
            instance.sprite.setOrigin(instance.wrongDir[0].getSize().x / 1.5,
                                      instance.wrongDir[0].getSize().y / 2);
            instance.sprite.setPosition(0, 0);
            instance.nextFrameTime = 0.5;
        }
    } else {
        if (instance.state == LakituState::WRONG_DIR) {
            Audio::stop(SFX::CIRCUIT_LAKITU_WARNING);
            sleep();
        }
    }
}

void Lakitu::pickUpDriver(Driver *driver) {
    if (instance.currentAnimationPriority <=
        instance.animationPriorities[(int)LakituState::PICKUP]) {
        sleep();
        instance.frameTime = 0;
        instance.screenTime = 0;
        instance.textIndex = 0;
        instance.state = LakituState::PICKUP;
        instance.sprite.setTexture(instance.lakituCatchPlayer);

        instance.sprite.setPosition(
            instance.winSize.x / 2 -
                instance.sprite.getGlobalBounds().width / 2.25,
            -40);

        instance.ptrDriver = driver;

        instance.ptrDriver->onLakitu = true;

        instance.nextFrameTime = 0.5;
        instance.currentAnimationPriority =
            instance.animationPriorities[(int)LakituState::PICKUP];
    }
}

bool Lakitu::hasStarted() { return instance.started; }

bool Lakitu::isSleeping() {
    return instance.state == Lakitu::LakituState::SLEEP;
}

void Lakitu::sleep() {
    Audio::stop(SFX::CIRCUIT_LAKITU_WARNING);
    instance.sprite.setOrigin(instance.start[0].getSize().x / 2,
                              instance.start[0].getSize().y / 2);
    instance.state = LakituState::SLEEP;
    instance.sprite.setPosition(-20, -20);
    instance.screenTime = 0;
    instance.textIndex = 0;
    instance.started = false;
}

void Lakitu::showUntil(float seconds, const sf::Time &) {
    if (instance.screenTime > seconds) {
        instance.state = LakituState::SLEEP;
        instance.screenTime = 0;
        instance.textIndex = 0;
        instance.currentAnimationPriority = 0;
        instance.started = false;
    }
}

void Lakitu::update(const sf::Time &deltaTime) {
    switch (instance.state) {
        case LakituState::START: {
            // Initial move
            if (instance.sprite.getPosition().y < instance.winSize.y / 2 / 3 &&
                !instance.started) {
                instance.textIndex = 0;
                instance.sprite.move(0, 2);
            } else {
                if (!instance.soundStarted) {
                    Audio::play(SFX::CIRCUIT_LAKITU_SEMAPHORE);
                    instance.soundStarted = true;
                }

                // Normal animation
                instance.frameTime += deltaTime.asSeconds();
                if (instance.frameTime >= instance.nextFrameTime) {
                    instance.textIndex++;
                    instance.frameTime = 0;
                }

                // Lights
                if (instance.textIndex > 1 && instance.textIndex < 5) {
                    instance.lightSprite.setTexture(
                        instance.lights[instance.textIndex - 1]);
                }

                // Started
                if (instance.textIndex >= 4) {
                    instance.sprite.setTexture(instance.start[1]);
                    instance.started = true;
                }

                instance.screenTime += deltaTime.asSeconds();
                instance.showUntil(10, deltaTime);
            }

            // Final move
            if (instance.started && instance.textIndex >= 5) {
                instance.sprite.move(0, -2);
            }

            // Light position
            sf::Vector2f lakiPos = instance.sprite.getPosition();
            sf::FloatRect lakiSize = instance.sprite.getGlobalBounds();
            if (instance.textIndex >= 4)
                instance.lightSprite.setPosition(
                    lakiPos.x + lakiSize.width / 2 * 1, lakiPos.y);
            else
                instance.lightSprite.setPosition(
                    lakiPos.x + lakiSize.width / 2 * 0.95, lakiPos.y);

        } break;

        case LakituState::PICKUP: {
            // Initial move
            if (instance.sprite.getPosition().y < instance.winSize.y / 2 / 5 &&
                instance.ptrDriver->onLakitu) {
                instance.textIndex = 0;
                instance.sprite.move(0, 2);
            } else {
                // Normal animation
                instance.frameTime += deltaTime.asSeconds();
                if (instance.frameTime >= instance.nextFrameTime) {
                    instance.textIndex++;
                    instance.frameTime = 0;
                }
                if (instance.textIndex == 1) {
                    // Throw driver
                    instance.textIndex++;
                    instance.ptrDriver->onLakitu = false;
                    instance.ptrDriver->addCoin(-2);
                    instance.ptrDriver->popStateEnd(DriverState::STOPPED);
                }

                instance.screenTime += deltaTime.asSeconds();
                instance.showUntil(8, deltaTime);
            }

            // Final move
            if (!instance.ptrDriver->onLakitu) {
                instance.sprite.move(0, -1);
            }

            // Player position
            sf::Vector2f lakiPos = instance.sprite.getPosition();
            sf::FloatRect lakiSize = instance.sprite.getGlobalBounds();
            if (instance.ptrDriver->onLakitu) {
                // On fishing rod
                float scaleFactor = ((float)instance.winSize.x / BASIC_WIDTH);
                float driverY = instance.ptrDriver->animator.sprite
                                    .getLocalBounds()
                                    .height *
                                1.6 * scaleFactor;
                instance.ptrDriver->height =
                    ((instance.winSize.y / 2 - lakiPos.y - lakiSize.height -
                      driverY) /
                     8.0f) /
                    scaleFactor;
            }

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

            float time = instance.wrongTime;
            if (instance.wrongTime > 5 && instance.wrongTime < 10)
                time = 5 - (instance.wrongTime - 5);
            else if (instance.wrongTime >= 10)
                instance.wrongTime = 0;

            float x = time / 5;
            float y = instance.winSize.y * 0.2;

            instance.screenTime += deltaTime.asSeconds();
            instance.wrongTime += deltaTime.asSeconds();
            if (x > 1) {
                x = 0;
            }
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

void Lakitu::drawShadow(sf::RenderTarget &window) {
    sf::Vector2f lakiPos = instance.sprite.getPosition();
    if (instance.drawShadows && lakiPos.y > 0) {
        sf::Vector2f lakiPos = instance.sprite.getPosition();
        float y = instance.winSize.y * 0.45;
        int alpha = 255 * (lakiPos.y / y);
        sf::Color color(255, 255, 255, alpha);
        instance.spriteShadow.setColor(color);
        instance.spriteShadow.setPosition(lakiPos.x, y);
        instance.spriteShadow.setScale(instance.sScale / 1.5,
                                       instance.sScale / 1.5);
        window.draw(instance.spriteShadow);
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

void Lakitu::reset() {
    setWrongDir(false);
    sleep();
    instance.frameTime = 0;
    instance.lap = 2;
    instance.light = 0;
    instance.started = false;
    instance.soundStarted = false;
    instance.currentAnimationPriority = 0;
}
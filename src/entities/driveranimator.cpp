#include "driveranimator.h"

DriverAnimator::DriverAnimator(const char* spriteFile) {
    for (int i = 0; i < 12; i++)
        driving[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 32, 32, 32));
    for (int i = 0; i < 5; i++)
        others[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 0, 32, 32));

    sprite.setTexture(driving[0]);

    state = PlayerState::GO_FORWARD;
    sprite.setOrigin(driving[0].getSize().x / 2, driving[0].getSize().y / 2);
    sprite.scale(sScale, sScale);
}

void DriverAnimator::goForward() {
    if (canDrive()) state = PlayerState::GO_FORWARD;
}

void DriverAnimator::goRight() {
    if (canDrive()) state = PlayerState::GO_RIGHT;
}

void DriverAnimator::goLeft() {
    if (canDrive()) state = PlayerState::GO_LEFT;
}

void DriverAnimator::hit() { state = PlayerState::HIT; }

void DriverAnimator::fall() { state = PlayerState::FALLING; }

void DriverAnimator::update(float speedTurn) {
    switch (state) {
        case PlayerState::GO_FORWARD:
            sprite.setTexture(driving[0]);
            sprite.setScale(sScale, sScale);
            break;

        case PlayerState::GO_RIGHT:
            if (speedTurn < 1.0f * (1.f / 4))
                sprite.setTexture(driving[1]);
            else if (speedTurn < 1.0f * (1.f / 2))
                sprite.setTexture(driving[2]);
            else if (speedTurn < 1.0f * (3.f / 4))
                sprite.setTexture(driving[3]);
            else
                sprite.setTexture(driving[4]);
            sprite.setScale(sScale, sScale);
            break;

        case PlayerState::GO_LEFT:
            if (speedTurn > -1.0f * (1.f / 4))
                sprite.setTexture(driving[1]);
            else if (speedTurn > -1.0f * (1.f / 2))
                sprite.setTexture(driving[2]);
            else if (speedTurn > -1.0f * (3.f / 4))
                sprite.setTexture(driving[3]);
            else
                sprite.setTexture(driving[4]);
            sprite.setScale(-sScale, sScale);
            break;

        case PlayerState::GO_BACK:
            /* code */
            break;

        case PlayerState::HIT:
            hitPos = (hitPos + 1) % 22;
            sprite.setTexture(driving[hitTextuIdx[hitPos]]);
            if (hitPos > 11)
                sprite.setScale(-sScale, sScale);
            else
                sprite.setScale(sScale, sScale);
            break;

        case PlayerState::FALLING:  // NO va
        {
            sf::Vector2f s = sprite.getScale();
            if (s.x > 0) {
                // TODO this shouldnt be here
                sprite.scale(0.9f, 0.9f);
            }
            break;
        }

        case PlayerState::CRASH:
            /* code */
            break;

        default:
            sprite.setTexture(driving[0]);
            break;
    }
}

bool DriverAnimator::canDrive() const {
    return state != PlayerState::HIT || state != PlayerState::FALLING;
}

sf::Sprite DriverAnimator::getMinimapSprite(float angle) const {
    sf::Sprite minimapSprite(sprite);   // copy sprite (important for scale)
    angle = fmodf(angle, 2.0f * M_PI);  // 0-2pi range

    // TODO hacer setTexture con la textura que corresponda
    // 1) Si esta en estado HIT/FALLING, aparecera en el minimapa girando
    //    asi que habra que ignorar el parametro angle y poner lo que toque
    // 2) Si no, elegir la textura que toque dado el angle
    //    mirando hacia la derecha:   angle = 0
    //                     abajo:     angle = pi / 2
    //                     izquierda: angle = pi
    //                     arriba:    angle = pi * 3 / 4
    minimapSprite.setTexture(driving[0]);

    return minimapSprite;
}

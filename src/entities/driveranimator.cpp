#include "driveranimator.h"

DriverAnimator::DriverAnimator(const char* spriteFile) {
    for (int i = 0; i < 12; i++)
        driving[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 32, 32, 32));
    for (int i = 0; i < 5; i++)
        others[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 0, 32, 32));

    sprite.setTexture(driving[0]);

    state = PlayerState::GO_FORWARD;
    sprite.setOrigin(driving[0].getSize().x / 2, sprite.getGlobalBounds().height);
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
    sf::Sprite minimapSprite(sprite);  // copy sprite (important for scale)
    minimapSprite.setScale(sScale, sScale);
    angle += M_PI / 2;                  // adjust
    angle = fmodf(angle, 2.0f * M_PI);  // 0-2pi range

    if (angle < 0)  // 0-2pi range
        angle += 2.0f * M_PI;

    if (state != PlayerState::HIT || state != PlayerState::FALLING) {
        for (int i = 1; i <= 22; i++) {
            if (angle <= ((i * 2.0f * M_PI) / 22.f)) {
                minimapSprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (i > 11) minimapSprite.scale(-1, 1);
                break;
            }
        }
    }

    return minimapSprite;
}

void DriverAnimator::setViewSprite(float viwerAngle, float driverAngle) {

    float diff = viwerAngle - driverAngle;

    diff = fmodf(diff, 2.0f * M_PI); 
    if (diff < 0)  // 0-2pi range
        diff += 2.0f * M_PI;

    std::cout << "ViewAngle: " << viwerAngle << std::endl;
    std::cout << "DriverAngle: " << driverAngle << std::endl;
    std::cout << "ResultAngle: " << diff << std::endl;

    //sprite.setScale(abs(sprite.getScale().x), sprite.getScale().y);
    sprite.setScale(0.4107142985F, 0.4107142985F);

    if (state != PlayerState::HIT || state != PlayerState::FALLING) {
        for (int i = 1; i <= 22; i++) {
            if (diff <= ((i * 2.0f * M_PI) / 22.f)) {
                sprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (diff < M_PI) {
                    sprite.scale(-1, 1);
                }
                break;
            }
        }
    }
}

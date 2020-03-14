#include "driver.h"

void Driver::update(const sf::Time &deltaTime) {
    // Friction
    speedForward = std::fmaxf(speedForward - 0.005f, 0.0f);
    speedTurn /= 1.2f;
    // Speed control
    animator.goForward();
    if (Input::held(Key::ACCELERATE)) {
        speedForward = std::fminf(speedForward + 0.008f, 0.1f);
    }
    if (Input::held(Key::BRAKE)) {
        // dont make brakes too high as friction still applies
        speedForward = std::fmaxf(speedForward - 0.008f, 0.0f);
        // debug:
        animator.hit();
    }
    if (Input::held(Key::TURN_LEFT)) {
        speedTurn = std::fmaxf(speedTurn - 0.15f, -2.0f);
        animator.goLeft();
    }
    if (Input::held(Key::TURN_RIGHT)) {
        speedTurn = std::fminf(speedTurn + 0.15f, 2.0f);
        animator.goRight();
    }

    // Speed & rotation changes
    float deltaAngle = speedTurn * deltaTime.asSeconds();
    sf::Vector2f deltaPosition = sf::Vector2f(cosf(posAngle), sinf(posAngle)) *
                                 speedForward * deltaTime.asSeconds();

    switch (Map::getLand(position + deltaPosition)) {
        case Map::Land::BLOCK:
            deltaPosition = sf::Vector2f(0.0f, 0.0f);
            break;
        case Map::Land::SLOW:
            deltaPosition /= 2.0f;
            break;
        case Map::Land::OUTER:
            animator.fall();
        default:
            break;
    }

    position += deltaPosition;
    posAngle += deltaAngle;

    // std::cerr << int(posX * 128) << " " << int(posY * 128)
    //     << ": " << int(assetLand[int(posY * 128)][int(posX * 128)]) <<
    //     std::endl;

    // int landOriginX = int(lastPosX * 128);
    // int landOriginY = int(lastPosY * 128);
    // int landDestinyX = int(posX * 128);
    // int landDestinyY = int(posY * 128);
    // int distanceX = (landOriginX <= landDestinyX) ?
    //     landDestinyX - landOriginX :
    //     landOriginX - landDestinyX;
    // int distanceY = (landOriginY <= landDestinyY) ?
    //     landDestinyY - landOriginY :
    //     landOriginY - landDestinyY;
    // int landMaxDistance = std::max(distanceX, distanceY);
    // float stepX = (posX - lastPosX) / float(landMaxDistance);
    // float stepY = (posX - lastPosY) / float(landMaxDistance);

    // float landX = landOriginX;
    // float landY = landOriginY;
    // while(landX < landDestinyX || landY < landDestinyY) {
    //     if (assetLand[int(landY)][int(landX)] == Land::BLOCK) {
    //         std::cerr << "OUT" << std::endl;
    //         posX = landX / 128.0f;
    //         posY = landY / 128.0f;
    //         break;
    //     }
    //     landX += stepX;
    //     landY += stepY;
    // }
    // std::cerr << landOriginX << " " << landOriginY << std::endl;
    // std::cerr << posX << " " << posY << std::endl;
    animator.update(speedTurn);
}

void Driver::draw(sf::RenderTarget &window) {
    sf::Sprite playerSprite = animator.sprite;
    // possible player moving/rotation/etc
    float width = window.getSize().x;
    float halfHeight = window.getSize().y / 2.0f;
    playerSprite.setPosition(width / 2, (halfHeight * 3) / 4 - 15);
    window.draw(playerSprite);
}
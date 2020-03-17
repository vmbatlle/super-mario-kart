#include "driver.h"

// needed to overcome circular dependency errors
#include "map/map.h"

void Driver::update(const sf::Time &deltaTime) {
    constexpr float MAX_LINEAR_SPEED = 0.1f;
    // Physics variables
    float acelerationLinear = 0.0;
    // Friction
    constexpr float FRICTION_LINEAR_ACELERATION = -0.03f;
    acelerationLinear += FRICTION_LINEAR_ACELERATION;
    speedTurn /= 1.2f;
    // Speed control
    animator.goForward();

    if (Input::held(Key::ACCELERATE)) {
        constexpr float MOTOR_ACELERATION = 0.1f;
        acelerationLinear += MOTOR_ACELERATION;
    }
    if (Input::held(Key::BRAKE)) {
        // dont make brakes too high as friction still applies
        constexpr float BREAK_ACELERATION = -0.1f;
        acelerationLinear += BREAK_ACELERATION;
        // debug:
        // animator.hit();
    }
    if (Input::held(Key::TURN_LEFT)) {
        speedTurn = std::fmaxf(speedTurn - 0.15f, -2.0f);
        animator.goLeft();
    }
    if (Input::held(Key::TURN_RIGHT)) {
        speedTurn = std::fminf(speedTurn + 0.15f, 2.0f);
        animator.goRight();
    }

    if (Map::getLand(position) == Map::Land::SLOW) {
        constexpr float SLOW_LAND_MAX_LINEAR_SPEED = MAX_LINEAR_SPEED / 2.0;
        constexpr float SLOW_LAND_LINEAR_ACELERATION = -0.15f;
        if (speedForward > SLOW_LAND_MAX_LINEAR_SPEED) {
            acelerationLinear += SLOW_LAND_LINEAR_ACELERATION;
        }
    }

    // Speed & rotation changes
    // Calculate space traveled
    float deltaAngle = speedTurn * deltaTime.asSeconds();
    float deltaSpace = speedForward * deltaTime.asSeconds() +
                       acelerationLinear *
                           (deltaTime.asSeconds() * deltaTime.asSeconds()) /
                           2.0;
    deltaSpace =
        std::fminf(deltaSpace, MAX_LINEAR_SPEED * deltaTime.asSeconds());
    deltaSpace = std::fmaxf(deltaSpace, 0.0f);
    // Update speed
    speedForward += acelerationLinear * deltaTime.asSeconds();
    speedForward = std::fminf(speedForward, MAX_LINEAR_SPEED);
    speedForward = std::fmaxf(speedForward, 0.0f);

    sf::Vector2f deltaPosition =
        sf::Vector2f(cosf(posAngle), sinf(posAngle)) * deltaSpace;

    switch (Map::getLand(position + deltaPosition)) {
        case Map::Land::BLOCK:
            deltaPosition = sf::Vector2f(0.0f, 0.0f);
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
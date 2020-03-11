#include "mode7test.h"
#include <fstream>
#include <iostream>

void StateMode7Test::init() {
    playerX = START_POS_X;
    playerY = START_POS_Y;
    posX = 0.0f;
    posY = 0.0f;
    posAngle = START_POS_ANGLE;
    fovHalf = 0.4f;
    clipNear = 0.0005f;
    clipFar = 0.045f;

    speedForward = 0.0f;
    speedTurn = 0.0f;

    float width = game.getWindow().getSize().x;
    float halfHeight = game.getWindow().getSize().y / 2.0f;

    assetImageBottom.loadFromFile("assets/donut_plains_1.png");
    assetImageTop.loadFromFile("assets/sky.jpeg");

    player.init("assets/drivers/yoshi.png");
    player.sprite.setPosition(width / 2, halfHeight + (halfHeight * 3) / 4 - 30);
    std::ifstream assetLandFile("assets/donut_plains_1.txt");

    for (int y = 0; y < TILES_HEIGHT; y++) {
        char c;
        for (int x = 0; x < TILES_WIDTH; x++) {
            assetLandFile >> c;
            assetLand[y][x] = Land(c - '0');
        }
        // assetLandFile >> c;
    }
}

void StateMode7Test::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        // TODO this is example code
        // we can handle item usage by creating an item entity
        // and removing the player's held item
    }
}

void StateMode7Test::fixedUpdate(const sf::Time& deltaTime) {
    // Friction
    speedForward = std::fmaxf(speedForward - 0.005f, 0.0f);
    speedTurn /= 1.2f;
    // Speed control
    player.goForward();
    if (Input::held(Key::ACCELERATE)) {
        speedForward = std::fminf(speedForward + 0.008f, 0.1f);
    }
    if (Input::held(Key::BRAKE)) {
        // dont make brakes too high as friction still applies
        speedForward = std::fmaxf(speedForward - 0.008f, 0.0f);
        //debug:
        player.hited(deltaTime);
    }
    if (Input::held(Key::TURN_LEFT)) {
        speedTurn = std::fmaxf(speedTurn - 0.1f, -1.0f);
        player.goLeft(speedTurn);
    }
    if (Input::held(Key::TURN_RIGHT)) {
        speedTurn = std::fminf(speedTurn + 0.1f, 1.0f);
        player.goRight(speedTurn);
    }

    // Speed & rotation changes
    posAngle += speedTurn * deltaTime.asSeconds();
    float incX = cosf(posAngle) * speedForward * deltaTime.asSeconds();
    float incY = sinf(posAngle) * speedForward * deltaTime.asSeconds();

    switch (getLand(playerX + incX, playerY + incY)) {
        case Land::BLOCK:
            incX = 0.0f;
            incY = 0.0f;
            break;
        case Land::SLOW:
            incX /= 2.0f;
            incY /= 2.0f;
            break;
        default:
            break;
    }

    playerX += incX;
    playerY += incY;
    posX = playerX - cosf(posAngle) * (CAM_2_PLAYER_DST / ASSETS_WIDTH);
    posY = playerY - sinf(posAngle) * (CAM_2_PLAYER_DST / ASSETS_HEIGHT);

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
}

void StateMode7Test::draw(sf::RenderTarget& window) {
    float farX1 = posX + cosf(posAngle - fovHalf) * clipFar;
    float farY1 = posY + sinf(posAngle - fovHalf) * clipFar;
    float nearX1 = posX + cosf(posAngle - fovHalf) * clipNear;
    float nearY1 = posY + sinf(posAngle - fovHalf) * clipNear;

    float farX2 = posX + cosf(posAngle + fovHalf) * clipFar;
    float farY2 = posY + sinf(posAngle + fovHalf) * clipFar;
    float nearX2 = posX + cosf(posAngle + fovHalf) * clipNear;
    float nearY2 = posY + sinf(posAngle + fovHalf) * clipNear;

    float width = game.getWindow().getSize().x;
    float halfHeight = game.getWindow().getSize().y / 2.0f;
    sf::Image bottomImage, topImage;
    bottomImage.create(width, halfHeight);
    topImage.create(width, halfHeight);
    for (int y = 1; y < halfHeight; y++) {
        float sampleDepth = y / halfHeight;

        float startX = (farX1 - nearX1) / sampleDepth + nearX1;
        float startY = (farY1 - nearY1) / sampleDepth + nearY1;
        float endX = (farX2 - nearX2) / sampleDepth + nearX2;
        float endY = (farY2 - nearY2) / sampleDepth + nearY2;

        for (int x = 0; x < width; x++) {
            float sampleWidth = x / width;
            float sampleX = (endX - startX) * sampleWidth + startX;
            float sampleY = (endY - startY) * sampleWidth + startY;

            sf::Color sampleBottom = sf::Color::Black;
            sf::Color sampleTop = sf::Color::Black;

            if (!(sampleX < 0 || sampleX > 1.0f || sampleY < 0 ||
                  sampleY > 1.0f)) {
                sampleX = fmodf(sampleX, 1.0f) * assetImageBottom.getSize().x;
                sampleY = fmodf(sampleY, 1.0f) * assetImageBottom.getSize().y;

                sampleBottom = assetImageBottom.getPixel(sampleX, sampleY);
                sampleTop = assetImageTop.getPixel(sampleX, sampleY);
            }
            bottomImage.setPixel(x, y, sampleBottom);
            topImage.setPixel(x, halfHeight - y, sampleTop);
        }
    }

    sf::Texture bottomTexture, topTexture;
    bottomTexture.loadFromImage(bottomImage);
    topTexture.loadFromImage(topImage);
    sf::Sprite bottomSprite(bottomTexture), topSprite(topTexture);
    bottomSprite.setPosition(sf::Vector2f(0.0f, halfHeight));
    window.draw(bottomSprite);
    window.draw(topSprite);

    window.draw(player.sprite);
}
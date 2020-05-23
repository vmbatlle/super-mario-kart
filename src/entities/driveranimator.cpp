#include "driveranimator.h"

#include "map/map.h"

#define COLOR_CHANGE_RATE 6

// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val) {
    hue %= 360;
    while (hue < 0) hue += 360;

    if (sat < 0.f) sat = 0.f;
    if (sat > 1.f) sat = 1.f;

    if (val < 0.f) val = 0.f;
    if (val > 1.f) val = 1.f;

    int h = hue / 60;
    float f = float(hue) / 60 - h;
    float p = val * (1.f - sat);
    float q = val * (1.f - sat * f);
    float t = val * (1.f - sat * (1 - f));

    switch (h) {
        default:
        case 0:
        case 6:
            return sf::Color(val * 255, t * 255, p * 255);
        case 1:
            return sf::Color(q * 255, val * 255, p * 255);
        case 2:
            return sf::Color(p * 255, val * 255, t * 255);
        case 3:
            return sf::Color(p * 255, q * 255, val * 255);
        case 4:
            return sf::Color(t * 255, p * 255, val * 255);
        case 5:
            return sf::Color(val * 255, p * 255, q * 255);
    }
}

DriverAnimator::DriverAnimator(const char *spriteFile,
                               const DriverControlType &control)
    : controlType(control) {
    for (int i = 0; i < 12; i++)
        driving[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 32, 32, 32));
    for (int i = 0; i < 5; i++)
        others[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 0, 32, 32));

    std::string particleFile = "assets/misc/particles.png";
    for (int i = 0; i < 3; i++)
        driftTxtParticles[i].loadFromFile(particleFile,
                                          sf::IntRect(1 + (26 * i), 1, 16, 16));
    for (int i = 0; i < 2; i++)
        grassTxtParticles[i].loadFromFile(particleFile,
                                          sf::IntRect(1, 26 + (9 * i), 16, 8));

    sprite.setTexture(driving[0]);

    int groundType = 0;
    for (int i = 0; i < 5; i++) {
        // driftParticles[i].setScale(0.1,0.1);
        driftParticles[i].setTexture(driftTxtParticles[groundType]);
        driftParticles[i].setColor(sf::Color(255, 255, 255, 190));
        driftParticles[i].setOrigin(
            0, driftParticles[i].getLocalBounds().height * 1.3);
    }
    grassParticle.setTexture(grassTxtParticles[0]);
    grassParticle.setOrigin(grassParticle.getLocalBounds().width / 2,
                            grassParticle.getLocalBounds().height / 2);
    grassParticle.scale(1.5, 1.5);

    smashTime = sf::seconds(0);
    starTime = sf::seconds(0);
    starColor = 0;
    sScale = 2;

    state = PlayerState::GO_FORWARD;
    sprite.setOrigin(sprite.getGlobalBounds().width / 2,
                     sprite.getGlobalBounds().height);
    sprite.scale(sScale, sScale);
}

void DriverAnimator::goForward() {
    drifting = false;
    state = PlayerState::GO_FORWARD;
}

void DriverAnimator::goRight(bool drift) {
    drifting = drift;
    state = PlayerState::GO_RIGHT;
}

void DriverAnimator::goLeft(bool drift) {
    drifting = drift;
    state = PlayerState::GO_LEFT;
}

void DriverAnimator::hit() {
    drifting = false;
    state = PlayerState::HIT;
}

void DriverAnimator::fall() {
    drifting = false;
    state = PlayerState::FALLING;
}

void DriverAnimator::update(const float speedForward, const float speedTurn,
                            const float height, const sf::Time &deltaTime) {
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
            else if (drifting)
                sprite.setTexture(driving[4]);
            else
                sprite.setTexture(driving[3]);
            sprite.setScale(sScale, sScale);
            break;

        case PlayerState::GO_LEFT:
            if (speedTurn > -1.0f * (1.f / 4))
                sprite.setTexture(driving[1]);
            else if (speedTurn > -1.0f * (1.f / 2))
                sprite.setTexture(driving[2]);
            else if (drifting)
                sprite.setTexture(driving[4]);
            else
                sprite.setTexture(driving[3]);
            sprite.setScale(-sScale, sScale);
            break;

        case PlayerState::GO_BACK:
            /* Nothing */
            break;

        case PlayerState::FALLING:
            // if (abs(sprite.getScale().x) > 0)
            fallScale = fallScale * 0.9;
            sprite.setScale(sScale * fallScale, sScale * fallScale);
            break;

        case PlayerState::HIT:
            hitPos = (hitPos + 2) % NUM_HIT_TEXTURES;
            sprite.setTexture(driving[hitTextuIdx[hitPos]]);
            if (hitPos > NUM_HIT_TEXTURES / 2) {
                sprite.setScale(-sScale, sScale);
            } else
                sprite.setScale(sScale, sScale);
            break;

        default:
            sprite.setTexture(driving[0]);
            sprite.setScale(sScale, sScale);
            break;
    }

    if (drifting) {
        driftIndex = (driftIndex + 1) % 5;

        if (driftParticles[driftIndex].getScale().x > 2)
            driftParticles[driftIndex].setScale(0.1, 0.1);
        else
            driftParticles[driftIndex].scale(1.3, 1.3);

    } else {
        driftIndex = 0;
    }

    if (smallTime > sf::seconds(0)) {
        smallTime -= deltaTime;
        sprite.scale(1 / 2.0f, 1 / 2.0f);
        if (controlType == DriverControlType::PLAYER &&
            smallTime <= sf::seconds(0)) {
            Audio::play(SFX::CIRCUIT_PLAYER_GROW);
        }
    }

    if (smashTime > sf::seconds(0)) {
        smashTime -= deltaTime;
        sprite.scale(1, 1 / 2.0f);
    }

    if (starTime > sf::seconds(0)) {
        starTime -= deltaTime;
        sprite.setColor(hsv(starColor, 1.0f, 1.0f));
        starColor += 7;
        if (controlType == DriverControlType::PLAYER &&
            starTime <= sf::seconds(0)) {
            Audio::stop(SFX::CIRCUIT_ITEM_STAR);
        }
    } else {
        sprite.setColor(sf::Color::White);
        starColor = 0;
    }

    // Player sprite moves up/down and left/right when running/drifting
    // respectively
    if (height == 0.0f && drifting) {
        spriteMovementDriftTime =
            fmodf(spriteMovementDriftTime + 1.0f, MOVEMENT_DRIFT_PERIOD);
    } else {
        spriteMovementDriftTime /= 1.5f;
    }
    if (height == 0.0f && speedForward > 0.0f) {
        spriteMovementSpeedTime = fmodf(
            spriteMovementSpeedTime + speedForward * deltaTime.asSeconds(),
            MOVEMENT_SPEED_PERIOD);
    } else {
        spriteMovementSpeedTime /= 1.5f;
    }
    spriteMovementDrift =
        sinf(spriteMovementDriftTime * 2.0f * M_PI / MOVEMENT_DRIFT_PERIOD) *
        MOVEMENT_DRIFT_AMPLITUDE;
    spriteMovementSpeed =
        sinf(spriteMovementSpeedTime * 2.0f * M_PI / MOVEMENT_SPEED_PERIOD) *
        MOVEMENT_SPEED_AMPLITUDE;
}

bool DriverAnimator::canDrive() const { return state != PlayerState::HIT; }

void DriverAnimator::small(sf::Time duration) { smallTime = duration; }

void DriverAnimator::smash(sf::Time duration) { smashTime = duration; }

void DriverAnimator::star(sf::Time duration) { starTime = duration; }

sf::Sprite DriverAnimator::getMinimapSprite(float angle,
                                            const float screenScale) const {
    sf::Sprite minimapSprite(sprite);  // copy sprite (important for scale)
    minimapSprite.setScale(sScale * screenScale, sScale * screenScale);

    angle += M_PI / 2;                  // adjust
    angle = fmodf(angle, 2.0f * M_PI);  // 0-2pi range

    if (angle < 0)  // 0-2pi range
        angle += 2.0f * M_PI;

    if (state != PlayerState::HIT) {
        for (int i = 1; i <= NUM_HIT_TEXTURES + 1; i++) {
            if (angle <= ((i - 0.5f) * 2.0f * M_PI) / (float)NUM_HIT_TEXTURES) {
                minimapSprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (angle > M_PI) {  // careful: > sign (see below)
                    minimapSprite.scale(-1, 1);
                }
                break;
            }
        }
    } else {
        if (hitPos > NUM_HIT_TEXTURES / 2) {
            minimapSprite.scale(-1, 1);
        }
    }

    if (smallTime > sf::seconds(0)) {
        minimapSprite.scale(0.5, 0.5);
    }
    if (smashTime > sf::seconds(0)) {
        minimapSprite.scale(1, 0.5);
    }

    return minimapSprite;
}

void DriverAnimator::setViewSprite(float angle) {
    angle = fmodf(angle, 2.0f * M_PI);
    if (angle < 0)  // 0-2pi range
        angle += 2.0f * M_PI;

    sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);

    if (state != PlayerState::HIT) {
        for (int i = 1; i <= NUM_HIT_TEXTURES + 1; i++) {
            if (angle <= ((i - 0.5f) * 2.0f * M_PI) / (float)NUM_HIT_TEXTURES) {
                sprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (angle < M_PI) {  // careful: < sign (see above)
                    sprite.scale(-1, 1);
                }
                break;
            }
        }
    } else {
        if (hitPos > NUM_HIT_TEXTURES / 2) {
            sprite.scale(-1, 1);
        }
    }

    if (smallTime > sf::seconds(0)) {
        sprite.scale(0.5, 0.5);
    }
    if (smashTime > sf::seconds(0)) {
        sprite.scale(1, 0.5);
    }
}

void DriverAnimator::drawParticles(sf::RenderTarget &window, sf::Sprite &driver,
                                   bool small, sf::Vector2f mapPos) {
    sf::Vector2f driverPos = driver.getPosition();
    sf::Vector2f middlePosition = sf::Vector2f(driverPos.x, driverPos.y);
    sf::FloatRect driverSize = driver.getGlobalBounds();
    LandMaterial groundType = Map::getMaterial(mapPos);

    float factor = window.getSize().x / BASIC_WIDTH;

    // Grass
    if (groundType == LandMaterial::GRASS && PlayerState::GO_FORWARD == state) {
        if (grassIndex > 5)
            grassParticle.setTexture(grassTxtParticles[1]);
        else
            grassParticle.setTexture(grassTxtParticles[0]);

        grassIndex = (grassIndex + 1) % 10;

        sf::Sprite gpr(grassParticle);

        if (small) {
            gpr.scale(0.5, 0.5);
        }

        gpr.scale(factor, factor);

        sf::Sprite gpl(gpr);

        float posOffset = 0;

        gpr.setPosition(middlePosition.x + driverSize.width * 0.3 + posOffset,
                        middlePosition.y - driverSize.height * 0.1);
        gpl.setPosition(middlePosition.x - driverSize.width * 0.3 + posOffset,
                        middlePosition.y - driverSize.height * 0.1);

        window.draw(gpr);
        window.draw(gpl);
    }

    // Drifting
    if (drifting) {
        if (colorFrec > COLOR_CHANGE_RATE) {
            updateColor = true;
            colorFrec = 0;
        } else
            colorFrec++;

        int type = 0;
        sf::Color color = lastColor;
        float scale = 1;
        switch (groundType) {
            case LandMaterial::DIRT:
                type = 0;
                break;
            case LandMaterial::STONE:
                type = 1;
                break;
            case LandMaterial::SPOOKY_WOOD:
                type = 2;
                color = sf::Color(89, 54, 39, 190);
                scale = 0.7;
                break;
            case LandMaterial::RAINBOW:
                type = 2;
                if (updateColor) {
                    color = Map::sampleMapColor(mapPos);
                    updateColor = false;
                }
                scale = 0.7;
                break;
            case LandMaterial::GRASS:
                type = 2;
                color = sf::Color(69, 217, 24, 190);
                scale = 0.7;
                break;
            default:
                type = 0;
                break;
        }

        lastColor = color;

        for (auto pr : driftParticles) {
            pr.setTexture(driftTxtParticles[type]);
            if (type == 2) pr.setColor(color);
            pr.scale(scale, scale);
            if (small) {
                pr.scale(0.5, 0.5);
            }

            pr.scale(factor, factor);

            float posOffset = 0;
            if (PlayerState::GO_RIGHT == state) {
                pr.scale(-1, 1);

                posOffset = -driverSize.width * 0.25;
            } else if (PlayerState::GO_LEFT == state) {
                posOffset = +driverSize.width * 0.25;
            }

            sf::Sprite pl(pr);

            pr.setPosition(
                middlePosition.x + driverSize.width * 0.2 + posOffset,
                middlePosition.y);
            pl.setPosition(
                middlePosition.x - driverSize.width * 0.2 + posOffset,
                middlePosition.y);

            window.draw(pr);
            window.draw(pl);
        }
    }
}

void DriverAnimator::reset() {
    sprite.setTexture(driving[0]);

    smashTime = sf::seconds(0);
    starTime = sf::seconds(0);
    smallTime = sf::seconds(0);
    starColor = 0;
    sScale = 2;
    fallScale = 1;

    hitPos = 0;
    driftIndex = 0;
    drifting = false;

    state = PlayerState::GO_FORWARD;
    sprite.setScale(sScale, sScale);
    sprite.setColor(sf::Color::White);
}

#define _USE_MATH_DEFINES
#include "driver.h"

// needed to overcome circular dependency errors
#include "ai/gradientdescent.h"
#include "entities/lakitu.h"
#include "map/map.h"
#include "states/race.h"

sf::Time StateRace::currentTime;

const sf::Time Driver::SPEED_UP_DURATION = sf::seconds(1.5f);
const sf::Time Driver::SPEED_DOWN_DURATION = sf::seconds(10.0f);
const sf::Time Driver::STAR_DURATION = sf::seconds(40.0f);
const sf::Time Driver::UNCONTROLLED_DURATION = sf::seconds(1.0f);

// Try to simulate graph from:
// https://www.mariowiki.com/Super_Mario_Kart#Acceleration
void simulateSpeedGraph(Driver *self, float &accelerationLinear) {
    if (self->vehicle.convex) {
        float speedPercentage =
            self->speedForward / self->vehicle.maxNormalLinearSpeed;
        if (speedPercentage < 0.25f) {
            accelerationLinear += self->vehicle.motorAcceleration / 2.0f;
        } else if (speedPercentage < 0.45f) {
            accelerationLinear +=
                self->vehicle.motorAcceleration * (speedPercentage + 0.075f);
        } else if (speedPercentage < 0.95f) {
            accelerationLinear += self->vehicle.motorAcceleration / 2.0f;
        } else {
            accelerationLinear +=
                (0.05f * self->vehicle.maxNormalLinearSpeed) / 4.0f;
        }
    } else {
        float speedPercentage =
            self->speedForward / self->vehicle.maxNormalLinearSpeed;
        if (speedPercentage < 0.45f) {
            accelerationLinear += self->vehicle.motorAcceleration / 2.0f;
        } else if (speedPercentage < 0.95f) {
            accelerationLinear +=
                self->vehicle.motorAcceleration * (1.0f - speedPercentage);
        } else {
            accelerationLinear +=
                (0.05f * self->vehicle.maxNormalLinearSpeed) / 4.0f;
        }
    }
}

void incrisingAngularAceleration(Driver *self, float &accelerationAngular) {
    if (self->pressedToDrift) {
        if (Input::held(Key::TURN_RIGHT)) {
            self->speedTurn = self->vehicle.maxTurningAngularSpeed * 0.30f;
        } else if (Input::held(Key::TURN_LEFT)) {
            self->speedTurn =
                -1.0 * self->vehicle.maxTurningAngularSpeed * 0.30f;
        }
        self->pressedToDrift = false;
    }
    if (std::fabs(self->speedTurn) >
            (self->vehicle.maxTurningAngularSpeed * 0.4f) &&
        std::fabs(self->speedForward) >
            (self->vehicle.maxNormalLinearSpeed * 0.4f)) {
        accelerationAngular = self->vehicle.turningAcceleration * 1.0f;
    } else {
        accelerationAngular = self->vehicle.turningAcceleration * 0.15f;
    }
}

void reduceLinearSpeedWhileTurning(Driver *self, float &accelerationLinear,
                                   float &speedTurn) {
    float speedTurnPercentage =
        std::fabs(speedTurn / self->vehicle.maxTurningAngularSpeed);

    if (self->speedForward > self->vehicle.maxNormalLinearSpeed * 0.9f) {
        accelerationLinear =
            -1.0 * self->vehicle.motorAcceleration * speedTurnPercentage;
    }
}

// update using input service
void Driver::usePlayerControls(float &accelerationLinear) {
    // Speed control
    if (Input::held(Key::ACCELERATE)) {
        simulateSpeedGraph(this, accelerationLinear);
    }
    if (Input::held(Key::BRAKE)) {
        // dont make brakes too high as friction still applies
        accelerationLinear += VehicleProperties::BREAK_ACELERATION;
    }

    if (Input::held(Key::TURN_LEFT)) {
        float accelerationAngular = 0.0;
        incrisingAngularAceleration(this, accelerationAngular);
        speedTurn = std::fmaxf(speedTurn - accelerationAngular,
                               vehicle.maxTurningAngularSpeed * -1.0f);
        reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        animator.goLeft();
    } else if (Input::held(Key::TURN_RIGHT)) {
        float accelerationAngular = 0.0;
        incrisingAngularAceleration(this, accelerationAngular);
        speedTurn = std::fminf(speedTurn + accelerationAngular,
                               vehicle.maxTurningAngularSpeed);
        reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        animator.goRight();
    }
}

// update based on gradient AI
void Driver::useGradientControls(float &accelerationLinear) {
    // just accelerate if the ai is jumping
    if (height > 0.0f) {
        simulateSpeedGraph(this, accelerationLinear);
        return;
    }
    sf::Vector2f dirSum(0.0f, 0.0f);
    // if it's going too slow its probably stuck to a wall
    // reduce its vision so it knows how to exit the wall
    int tilesForward = speedForward < vehicle.maxNormalLinearSpeed / 4.0f
                           ? 1
                           : Map::getCurrentMapAIFarVision();
    for (int i = 0; i < tilesForward; i++) {
        dirSum += AIGradientDescent::getNextDirection(position + dirSum);
    }
    float targetAngle = std::atan2(dirSum.y, dirSum.x);
    float diff = targetAngle - posAngle;
    diff = fmodf(diff, 2.0f * M_PI);
    if (diff < 0.0f) diff += 2.0f * M_PI;
    if (fabsf(M_PI - diff) > 0.7f * M_PI) {
        // accelerate if it's not a sharp turn
        simulateSpeedGraph(this, accelerationLinear);
    }
    if (diff >= 0.05f * M_PI && diff <= 1.95f * M_PI) {
        float accelerationAngular = vehicle.turningAcceleration;
        if (diff > M_PI) {
            // left turn
            speedTurn = std::fmaxf(speedTurn - accelerationAngular,
                                   vehicle.maxTurningAngularSpeed * -1.0f);
            reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        } else {
            // right turn
            speedTurn = std::fminf(speedTurn + accelerationAngular,
                                   vehicle.maxTurningAngularSpeed);
            reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        }
    }
}

void Driver::updateGradientPosition() {
    static constexpr const int CONSECUTIVE_INCREMENTS_FOR_BACKWARDS = 5;
    int gradient = AIGradientDescent::getPositionValue(
        position.x * MAP_TILES_WIDTH, position.y * MAP_TILES_HEIGHT);
    // either player is on a bad tile (wall?) or gradient didnt change
    if (gradient == -1 || gradient == lastGradient) {
        return;
    }
    if (lastGradient == -1) {  // lastGradient wasn't initialized
        lastGradient = gradient;
        return;
    }
    if (gradient > lastGradient) {
        consecutiveGradientIncrements =
            std::min(consecutiveGradientIncrements + 1,
                     CONSECUTIVE_INCREMENTS_FOR_BACKWARDS);
        if (consecutiveGradientIncrements ==
            CONSECUTIVE_INCREMENTS_FOR_BACKWARDS) {
            goingForwards = false;
        }
    } else {
        consecutiveGradientIncrements =
            std::max(consecutiveGradientIncrements - 1,
                     CONSECUTIVE_INCREMENTS_FOR_BACKWARDS * -1);
        if (consecutiveGradientIncrements ==
            CONSECUTIVE_INCREMENTS_FOR_BACKWARDS * -1) {
            goingForwards = true;
        }
    }
    int diff = gradient - lastGradient;
    if (diff > GRADIENT_LAP_CHECK) {
        laps++;
        if (controlType == DriverControlType::PLAYER && laps < 6) {
            Map::reactivateQuestionPanels();
            Lakitu::showLap(laps);
        }
    } else if (diff < GRADIENT_LAP_CHECK * -1 && laps > 0) {
        laps--;
    }
    lastGradient = gradient;
    if (controlType == DriverControlType::PLAYER) {
        Lakitu::setWrongDir(isGoingBackwards());
    }
}

void Driver::applyMushroom() {
    if (controlType == DriverControlType::PLAYER)
        Gui::speed(SPEED_UP_DURATION.asSeconds());
    pushStateEnd(DriverState::SPEED_UP,
                 StateRace::currentTime + SPEED_UP_DURATION);
}

void Driver::applyStar() {
    if (controlType == DriverControlType::PLAYER)
        Audio::play(SFX::CIRCUIT_ITEM_STAR);
    pushStateEnd(DriverState::STAR, StateRace::currentTime + STAR_DURATION);
    animator.star(STAR_DURATION);
}

void Driver::applyThunder() {
    pushStateEnd(DriverState::UNCONTROLLED,
                 StateRace::currentTime + UNCONTROLLED_DURATION);
    animator.small(SPEED_DOWN_DURATION + UNCONTROLLED_DURATION);
    pushStateEnd(DriverState::SPEED_DOWN,
                 StateRace::currentTime + SPEED_DOWN_DURATION);
};

void Driver::shortJump() {
    if (height == 0.0f) {
        height = 8.0f;
    }
}

void Driver::applyHit() {
    pushStateEnd(DriverState::UNCONTROLLED,
                 StateRace::currentTime + UNCONTROLLED_DURATION);
}

void Driver::applySmash() {
    animator.smash(SPEED_DOWN_DURATION + UNCONTROLLED_DURATION);
    pushStateEnd(DriverState::UNCONTROLLED,
                 StateRace::currentTime + UNCONTROLLED_DURATION);
}

void handlerHitBlock(Driver *self, const sf::Vector2f &nextPosition) {
    sf::Vector2f moveWidth = sf::Vector2f(1.0 / MAP_TILES_WIDTH, 0.0);
    sf::Vector2f moveHeight = sf::Vector2f(0.0, 1.0 / MAP_TILES_HEIGHT);

    int widthSize = 0;
    for (int j = -1; j <= 1; j += 2) {
        for (int i = 1; i <= 4; i++) {
            if (Map::getLand(nextPosition + float(i * j) * moveWidth) ==
                MapLand::BLOCK) {
                widthSize++;
            } else {
                break;
            }
        }
    }
    int heightSize = 0;
    for (int j = -1; j <= 1; j += 2) {
        for (int i = 1; i <= 4; i++) {
            if (Map::getLand(nextPosition + float(i * j) * moveHeight) ==
                MapLand::BLOCK) {
                heightSize++;
            } else {
                break;
            }
        }
    }

    sf::Vector2f momentum =
        sf::Vector2f(cosf(self->posAngle), sinf(self->posAngle)) *
        float(std::fmax(self->speedForward,
                        self->vehicle.maxNormalLinearSpeed * 0.5));
    if (widthSize > 4 && heightSize < 4) {
        self->vectorialSpeed = sf::Vector2f(momentum.x, -momentum.y);
    } else if (widthSize < 4 && heightSize > 4) {
        self->vectorialSpeed = sf::Vector2f(-momentum.x, momentum.y);
    } else {
        self->vectorialSpeed = sf::Vector2f(-momentum.x, -momentum.y);
    }
}

void Driver::addCoin(int amount) {
    coints += amount;
    if (controlType == DriverControlType::PLAYER) {
        Gui::addCoin(amount);
    }
}

void Driver::pickUpPowerUp(PowerUps power) {
    powerUp = power;
    if (controlType == DriverControlType::PLAYER) {
        Gui::setPowerUp(power);
    }
}

void Driver::setPositionAndReset(const sf::Vector2f &newPosition) {
    position = newPosition;
    laps = 0;
    powerUp = PowerUps::NONE;
    coints = 0;
    goingForwards = true;
    lastGradient = -1;
    // TODO IMPORTANT clear all states / speeds
    // speed, momentum, etc.
}

void improvedCheckOfMapLands(Driver *self, const sf::Vector2f &position,
                             sf::Vector2f &deltaPosition) {
    sf::Vector2f nextPosition = position + deltaPosition;
    // TODO: Adjust size when character is set with the correct scale
    float halfTileWidthInMapCoord =
        float(MAP_TILE_SIZE) / MAP_ASSETS_WIDTH / 2.5f;
    float halfTileHeightInMapCoord =
        float(MAP_TILE_SIZE) / MAP_ASSETS_HEIGHT / 2.5f;

    float deltaAngle[5] = {0, M_PI_2, -M_PI_2, M_PI_4, -M_PI_4};

    for (int i = 0; i < 5; i++) {
        sf::Vector2f shifting = sf::Vector2f(
            cosf(self->posAngle + deltaAngle[i]) * halfTileWidthInMapCoord,
            sinf(self->posAngle + deltaAngle[i]) * halfTileHeightInMapCoord);
        switch (Map::getLand(nextPosition + shifting)) {
            case MapLand::BLOCK:
                handlerHitBlock(self, nextPosition + shifting);
                self->speedForward = 0.0f;
                self->collisionMomentum = sf::Vector2f(0.0f, 0.0f);
                deltaPosition = sf::Vector2f(0.0f, 0.0f);
                return;
            case MapLand::OUTER:
                if (self->height == 0.0f) {
                    self->animator.fall();
                }
                return;
            default:
                break;
        }
    }
}

void Driver::update(const sf::Time &deltaTime) {
    // Physics variables
    float accelerationLinear = 0.0f;
    // Friction
    accelerationLinear += VehicleProperties::FRICTION_LINEAR_ACELERATION;
    if ((!Input::held(Key::TURN_LEFT) && !Input::held(Key::TURN_RIGHT)) ||
        (Input::held(Key::TURN_LEFT) && speedTurn > 0.0f) ||
        (Input::held(Key::TURN_RIGHT) && speedTurn < 0.0f)) {
        speedTurn /= 1.2f;
    }

    // remove expired states
    popStateEnd(StateRace::currentTime);

    if ((state & (int)DriverState::UNCONTROLLED)) {
        animator.hit();
    } else {
        animator.goForward();
        switch (controlType) {
            case DriverControlType::PLAYER:
                usePlayerControls(accelerationLinear);
                break;
            case DriverControlType::AI_GRADIENT:
                useGradientControls(accelerationLinear);
                break;
            default:
                break;
        }
    }

    MapLand land = Map::getLand(position);
    if (land == MapLand::SLOW && (~state & (int)DriverState::STAR)) {
        if (speedForward > vehicle.slowLandMaxLinearSpeed) {
            accelerationLinear +=
                VehicleProperties::SLOW_LAND_LINEAR_ACELERATION;
        }
    } else if (land == MapLand::OIL_SLICK &&
               (~state & (int)DriverState::STAR)) {
        speedTurn = 0.0f;
        speedForward =
            std::fmin(speedForward, vehicle.maxNormalLinearSpeed * 0.6f);
        pushStateEnd(DriverState::UNCONTROLLED,
                     StateRace::currentTime + UNCONTROLLED_DURATION);
    } else if (land == MapLand::RAMP || land == MapLand::RAMP_HORIZONTAL ||
               land == MapLand::RAMP_VERTICAL) {
        if (speedUpwards == 0.0f) {
            const float RAMP_INCLINATION = 45.0f / 90.0f;
            // 384 = 20.0 / (MAX(MAX_LINEAR_SPEED[i]) / 2.0)
            speedUpwards = RAMP_INCLINATION * speedForward * 384.0;
            speedUpwards = std::fmax(speedUpwards, 10.0);
            speedForward = (1.0 - RAMP_INCLINATION) * speedForward;
            // 0.05 = MIN(MAX_LINEAR_SPEED[i]) / 2.0
            speedForward = std::fmax(speedForward, 0.05);
        }
    } else if (land == MapLand::ZIPPER) {
        pushStateEnd(DriverState::SPEED_UP,
                     StateRace::currentTime + SPEED_UP_DURATION);
        speedForward = vehicle.maxSpeedUpLinearSpeed;
    } else if (land == MapLand::OTHER) {
        // set a custom destructor to avoid deletion of the object itself
        Map::collideWithSpecialFloorObject(DriverPtr(this, [](Driver *) {}));
    }

    // Gravity
    if (height > 0.0f || speedUpwards > 0.0f) {
        // -9.8 * 5.0 MANUAL ADJUST
        const float gravityAceleration = -9.8 * 5.0;
        height = height + speedUpwards * deltaTime.asSeconds() +
                 0.5 * gravityAceleration * deltaTime.asSeconds() *
                     deltaTime.asSeconds();
        height = std::fmax(height, 0.0f);
        speedUpwards =
            speedUpwards + gravityAceleration * deltaTime.asSeconds();
        if (height == 0.0f) {
            speedUpwards = 0.0f;
        }
    }

    float maxLinearSpeed;
    if (state & (int)DriverState::SPEED_UP || state & (int)DriverState::STAR) {
        maxLinearSpeed = vehicle.maxSpeedUpLinearSpeed;
    } else if (state & (int)DriverState::SPEED_DOWN) {
        maxLinearSpeed = vehicle.maxSpeedDownLinearSpeed;
    } else {
        maxLinearSpeed = vehicle.maxNormalLinearSpeed;
    }

    // Speed & rotation changes
    // Calculate space traveled
    float deltaAngle = speedTurn * deltaTime.asSeconds();
    float deltaSpace = speedForward * deltaTime.asSeconds() +
                       accelerationLinear *
                           (deltaTime.asSeconds() * deltaTime.asSeconds()) /
                           2.0;
    deltaSpace = std::fminf(deltaSpace, maxLinearSpeed * deltaTime.asSeconds());
    deltaSpace = std::fmaxf(deltaSpace, 0.0f);
    // Update speed
    speedForward += accelerationLinear * deltaTime.asSeconds();
    speedForward = std::fminf(speedForward, maxLinearSpeed);
    speedForward = std::fmaxf(speedForward, 0.0f);

    sf::Vector2f deltaPosition =
        sf::Vector2f(cosf(posAngle), sinf(posAngle)) * deltaSpace;

    // collision momentum
    deltaPosition += collisionMomentum;
    collisionMomentum /= 1.3f;
    deltaPosition += vectorialSpeed * deltaTime.asSeconds();
    vectorialSpeed /= 1.3f;

    improvedCheckOfMapLands(this, position, deltaPosition);

    // normal driving
    position += deltaPosition;
    posAngle += deltaAngle;
    posAngle = fmodf(posAngle, 2.0f * M_PI);

    updateGradientPosition();
    animator.update(speedTurn, deltaTime);

    // TODO unused code below - remove?

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

sf::Sprite &Driver::getSprite() { return animator.sprite; }

std::pair<float, sf::Sprite *> Driver::getDrawable(
    const sf::RenderTarget &window) {
    // possible player moving/rotation/etc
    float width = window.getSize().x;
    float halfHeight = window.getSize().y / 2.0f;
    float y =
        (halfHeight * 3) / 4 + animator.sprite.getGlobalBounds().height / 2;
    // height is substracted for jump effect
    animator.sprite.setPosition(width / 2, y - height);
    float z = Map::CAM_2_PLAYER_DST / MAP_ASSETS_WIDTH;
    return std::make_pair(z, &animator.sprite);
}

void Driver::pushStateEnd(DriverState state, const sf::Time &endTime) {
    this->state |= (int)state;
    stateEnd[(int)log2((int)state)] = endTime;
}

int Driver::popStateEnd(const sf::Time &currentTime) {
    int finishedEstates = 0;
    if (this->state != 0) {
        int state = 1;
        for (int i = 0; i < (int)DriverState::_COUNT; i++) {
            if (this->state & state) {
                if (stateEnd[i] < currentTime) {
                    finishedEstates |= state;
                    this->state &= ~state;
                }
            }
            state *= 2;
        }
    }
    return finishedEstates;
}

bool Driver::solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                            const sf::Vector2f &otherPos,
                            const float otherWeight, const float distance2) {
    sf::Vector2f speed =
        speedForward * sf::Vector2f(cosf(posAngle), sinf(posAngle));
    float weight = vehicle.weight;
    sf::Vector2f quantity = speed * weight + otherSpeed * otherWeight;
    quantity /= (weight + otherWeight) * weight;
    sf::Vector2f dir = (otherPos - position) / sqrtf(distance2 + 1e-2f);
    float mod =
        sqrtf(quantity.x * quantity.x + quantity.y * quantity.y + 1e-2f);
    data = CollisionData(dir * mod, 1.0f);
    return true;
}
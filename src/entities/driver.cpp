#define _USE_MATH_DEFINES
#include "driver.h"

// needed to overcome circular dependency errors
#include "ai/gradientdescent.h"
#include "entities/lakitu.h"
#include "map/map.h"
#include "states/race.h"

sf::Time StateRace::currentTime;

const sf::Time Driver::SPEED_UP_DURATION = sf::seconds(1.5f);
const sf::Time Driver::MORE_SPEED_UP_DURATION = sf::seconds(0.75f);
const sf::Time Driver::SPEED_DOWN_DURATION = sf::seconds(10.0f);
const sf::Time Driver::STAR_DURATION = sf::seconds(10.0f);
const sf::Time Driver::UNCONTROLLED_DURATION = sf::seconds(1.0f);
const sf::Time Driver::FOLLOWED_PATH_UPDATE_INTERVAL = sf::seconds(0.25f);
const int Driver::STEPS_BACK_FOR_RELOCATION = 4;
const int Driver::STEPS_STILL_FOR_RELOCATION = 10;

const float Driver::COIN_SPEED = 0.007;

// Try to simulate graph from:
// https://www.mariowiki.com/Super_Mario_Kart#Acceleration
void simulateSpeedGraph(Driver *self, float &accelerationLinear) {
    if (self->vehicle->convex) {
        float speedPercentage =
            self->speedForward / self->vehicle->maxNormalLinearSpeed;
        if (speedPercentage < 0.25f) {
            accelerationLinear += self->vehicle->motorAcceleration / 2.0f;
        } else if (speedPercentage < 0.45f) {
            accelerationLinear +=
                self->vehicle->motorAcceleration * (speedPercentage + 0.075f);
        } else if (speedPercentage < 0.95f) {
            accelerationLinear += self->vehicle->motorAcceleration / 2.0f;
        } else {
            accelerationLinear +=
                (0.05f * self->vehicle->maxNormalLinearSpeed) / 4.0f;
        }
    } else {
        float speedPercentage =
            self->speedForward / self->vehicle->maxNormalLinearSpeed;
        if (speedPercentage < 0.45f) {
            accelerationLinear += self->vehicle->motorAcceleration / 2.0f;
        } else if (speedPercentage < 0.95f) {
            accelerationLinear +=
                self->vehicle->motorAcceleration * (1.0f - speedPercentage);
        } else {
            accelerationLinear +=
                (0.05f * self->vehicle->maxNormalLinearSpeed) / 4.0f;
        }
    }
}

// return true if its drifting
bool incrisingAngularAceleration(Driver *self, float &accelerationAngular) {
    bool drifting = false;
    if (self->pressedToDrift) {
        if (Input::held(Key::TURN_RIGHT)) {
            self->speedTurn = self->vehicle->maxTurningAngularSpeed * 0.41f;
        } else if (Input::held(Key::TURN_LEFT)) {
            self->speedTurn =
                -1.0 * self->vehicle->maxTurningAngularSpeed * 0.41f;
        }
        self->pressedToDrift = false;
    }
    if (std::fabs(self->speedTurn) >
            (self->vehicle->maxTurningAngularSpeed * 0.4f) &&
        std::fabs(self->speedForward) >
            (self->vehicle->maxNormalLinearSpeed * 0.4f)) {
        accelerationAngular = self->vehicle->turningAcceleration * 1.0f;
        drifting = true;
    } else {
        accelerationAngular = self->vehicle->turningAcceleration * 0.15f;
    }
    return drifting;
}

void reduceLinearSpeedWhileTurning(Driver *self, float &accelerationLinear,
                                   float &speedTurn) {
    float speedTurnPercentage =
        std::fabs(speedTurn / self->vehicle->maxTurningAngularSpeed);

    if (self->speedForward > self->vehicle->maxNormalLinearSpeed * 0.9f) {
        accelerationLinear =
            -1.0 * self->vehicle->motorAcceleration * speedTurnPercentage;
    }
}

// update using input service
void Driver::usePlayerControls(float &accelerationLinear) {
    // Speed control
    if (height == 0.0f) {
        if (Input::held(Key::ACCELERATE)) {
            simulateSpeedGraph(this, accelerationLinear);
        }
        if (Input::held(Key::BRAKE)) {
            // dont make brakes too high as friction still applies
            accelerationLinear += VehicleProperties::BREAK_ACELERATION;
        }
    }

    bool drift = false;
    if (Input::held(Key::TURN_LEFT) && !Input::held(Key::TURN_RIGHT)) {
        float accelerationAngular = 0.0;
        drift = incrisingAngularAceleration(this, accelerationAngular);
        speedTurn = std::fmaxf(speedTurn - accelerationAngular,
                               vehicle->maxTurningAngularSpeed * -1.0f);
        reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        animator.goLeft(drift);
    } else if (Input::held(Key::TURN_RIGHT) && !Input::held(Key::TURN_LEFT)) {
        float accelerationAngular = 0.0;
        drift = incrisingAngularAceleration(this, accelerationAngular);
        speedTurn = std::fminf(speedTurn + accelerationAngular,
                               vehicle->maxTurningAngularSpeed);
        reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        animator.goRight(drift);
    }
}

// update based on gradient AI
void Driver::useGradientControls(float &accelerationLinear) {
    sf::Vector2f dirSum(0.0f, 0.0f);
    // if it's going too slow its probably stuck to a wall
    // reduce its vision so it knows how to exit the wall
    int tilesForward = speedForward < vehicle->maxNormalLinearSpeed / 4.0f
                           ? 1
                           : Map::getCurrentMapAIFarVision();
    for (int i = 0; i < tilesForward; i++) {
        dirSum += AIGradientDescent::getNextDirection(position + dirSum);
    }
    float targetAngle = std::atan2(dirSum.y, dirSum.x);
    float diff = targetAngle - posAngle - speedTurn * 0.15f;
    diff = fmodf(diff, 2.0f * M_PI);
    if (diff < 0.0f) diff += 2.0f * M_PI;
    if (height == 0.0f && fabsf(M_PI - diff) > 0.85f * M_PI) {
        // accelerate if it's not a sharp turn
        simulateSpeedGraph(this, accelerationLinear);
    }
    if (diff >= 0.05f * M_PI && diff <= 1.95f * M_PI) {
        float accelerationAngular = vehicle->turningAcceleration;
        if (diff > M_PI) {
            // left turn
            speedTurn = std::fmaxf(speedTurn - accelerationAngular * 3.5f,
                                   vehicle->maxTurningAngularSpeed * -1.5f);
            reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        } else {
            // right turn
            speedTurn = std::fminf(speedTurn + accelerationAngular * 3.5f,
                                   vehicle->maxTurningAngularSpeed * 1.5f);
            reduceLinearSpeedWhileTurning(this, accelerationLinear, speedTurn);
        }
    }
}

void Driver::updateGradientPosition() {
    static constexpr const int CONSECUTIVE_INCREMENTS_FOR_BACKWARDS = 5;
    // check bounds
    if (position.x < 1e-4f || position.x > 1.0f - 1e-4f || position.y < 1e-4f ||
        position.y > 1.0f - 1e-4f) {
        return;
    }
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
    if (diff > AIGradientDescent::GRADIENT_LAP_CHECK) {
        laps = laps + 1;
        if (laps == 5 && maxLapSoFar == 4 &&
            controlType == DriverControlType::PLAYER) {
            Audio::stopMusic();
            Audio::play(SFX::CIRCUIT_LAST_LAP_NOTICE);
            Audio::play(Music::CIRCUIT_LAST_LAP);
        }
        if (laps > maxLapSoFar) {
            if (controlType == DriverControlType::PLAYER && laps < 6) {
                Map::reactivateQuestionPanels();
                Lakitu::showLap(laps);
            }
            if (laps > 1) {
                indexOfLap.push_back(followedPath.size() - 1);
            }
            maxLapSoFar = laps;
        }

    } else if (diff < AIGradientDescent::GRADIENT_LAP_CHECK * -1 && laps > 0) {
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
    speedForward = vehicle->maxSpeedUpLinearSpeed;
    pushStateEnd(DriverState::SPEED_UP,
                 StateRace::currentTime + SPEED_UP_DURATION);
}

void Driver::applyStar() {
    if (controlType == DriverControlType::PLAYER) {
        Audio::stopSFX();
        Audio::play(SFX::CIRCUIT_ITEM_STAR);
    }
    speedForward = speedForward * 2.0f;
    pushStateEnd(DriverState::STAR, StateRace::currentTime + STAR_DURATION);
    animator.star(STAR_DURATION);
}

void Driver::applyThunder(sf::Time duration) {
    speedTurn = 0.0f;
    speedForward =
        std::fmin(speedForward, vehicle->maxNormalLinearSpeed * 0.6f);
    pushStateEnd(DriverState::UNCONTROLLED,
                 StateRace::currentTime + UNCONTROLLED_DURATION);
    animator.small(duration);
    pushStateEnd(DriverState::SPEED_DOWN, StateRace::currentTime + duration);
}

void Driver::shortJump() {
    if (height == 0.0f) {
        flightAngle = posAngle;
        height = 3.0f;
    }
}

void Driver::applyHit() {
    if (~state & (int)DriverState::STAR) {
        addCoin(-1);
        pushStateEnd(DriverState::UNCONTROLLED,
                     StateRace::currentTime + UNCONTROLLED_DURATION);
    }
}

void Driver::applySmash() {
    addCoin(-2);
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

    float factor;
    if (self->isImmune()) {
        factor = std::fmax(self->speedForward,
                           self->vehicle->maxNormalLinearSpeed * 0.75);
    } else {
        factor = std::fmax(self->speedForward,
                           self->vehicle->maxNormalLinearSpeed * 0.5);
    }

    sf::Vector2f momentum =
        sf::Vector2f(cosf(self->posAngle), sinf(self->posAngle)) * factor;

    if (widthSize > 4 && heightSize < 4) {
        self->vectorialSpeed = sf::Vector2f(momentum.x, -momentum.y);
    } else if (widthSize < 4 && heightSize > 4) {
        self->vectorialSpeed = sf::Vector2f(-momentum.x, momentum.y);
    } else {
        self->vectorialSpeed = sf::Vector2f(-momentum.x, -momentum.y);
    }
}

void Driver::addCoin(int amount) {
    // TODO check for negative coins
    if (coins + amount > 10) {
        amount = 10 - coins;
    } else if (coins + amount < 0) {
        amount = 0 - coins;
    }
    coins += amount;
    if (coins < 11 && controlType == DriverControlType::PLAYER) {
        Gui::addCoin(amount);
    }
}

void Driver::pickUpPowerUp(PowerUps power) {
    powerUp = power;
    if (controlType == DriverControlType::PLAYER) {
        if (power != PowerUps::NONE) Audio::play(SFX::CIRCUIT_ITEM_RANDOMIZING);
        Gui::setPowerUp(power);
    }
}

void Driver::reset() {
    // State reset
    pressedToDrift = false;
    heightByRamp = false;
    state = (int)DriverState::NORMAL;
    for (int i = 0; i < (int)DriverState::_COUNT; i++) {
        stateEnd[i] = sf::seconds(0);
    }

    // Animator reset
    animator.reset();
}

void Driver::endRaceAndReset() { reset(); }

void Driver::setPositionAndReset(const sf::Vector2f &newPosition) {
    // Location update
    position = newPosition;
    posAngle = M_PI_2 * -1.0f;
    flightAngle = 0;
    followedPath.clear();
    prevAcceleration.clear();
    prevLap.clear();
    indexOfLap.clear();
    followedPath.push_back(position);
    prevAcceleration.push_back(0.0f);
    prevLap.push_back(0);
    indexOfLap.push_back(0);
    pathLastUpdatedAt = StateRace::currentTime;

    // Counters reset
    laps = 0;
    maxLapSoFar = 0;
    powerUp = PowerUps::NONE;
    coins = 0;
    rank = 0;
    goingForwards = true;
    lastGradient = -1;

    // TODO IMPORTANT clear all states / speeds
    // speed, momentum, etc.
    speedForward = 0.0f;
    speedTurn = 0.0f;
    speedUpwards = 0.0f;
    collisionMomentum = sf::Vector2f(0.0f, 0.0f);
    vectorialSpeed = sf::Vector2f(0.0f, 0.0f);

    // State reset
    pressedToDrift = false;
    heightByRamp = false;
    state = (int)DriverState::NORMAL;
    for (int i = 0; i < (int)DriverState::_COUNT; i++) {
        stateEnd[i] = sf::seconds(0);
    }

    // Animator reset
    animator.reset();

    // Gui reset
    Gui::reset();
}

void improvedCheckOfMapLands(Driver *self, const sf::Vector2f &position,
                             sf::Vector2f &deltaPosition) {
    sf::Vector2f nextPosition = position + deltaPosition;
    // TODO: Adjust size when character is set with the correct scale
    float halfTileWidthInMapCoord =
        float(MAP_TILE_SIZE) / MAP_ASSETS_WIDTH / 3.5f;
    float halfTileHeightInMapCoord =
        float(MAP_TILE_SIZE) / MAP_ASSETS_HEIGHT / 3.5f;

    float deltaAngle[5] = {0, M_PI_2, -M_PI_2, M_PI_4, -M_PI_4};

    for (int i = 0; i < 5; i++) {
        sf::Vector2f shifting = sf::Vector2f(
            cosf(self->posAngle + deltaAngle[i]) * halfTileWidthInMapCoord,
            sinf(self->posAngle + deltaAngle[i]) * halfTileHeightInMapCoord);
        switch (Map::getLand(nextPosition + shifting)) {
            case MapLand::BLOCK:
                handlerHitBlock(self, nextPosition + shifting);
                self->popStateEnd(DriverState::SPEED_UP);
                self->popStateEnd(DriverState::MORE_SPEED_UP);
                Gui::stopEffects();
                self->speedForward = 0.0f;
                self->collisionMomentum = sf::Vector2f(0.0f, 0.0f);
                deltaPosition = sf::Vector2f(0.0f, 0.0f);
                return;
            case MapLand::OUTER:
                self->falling = true;
                return;
            default:
                break;
        }
    }
}

void Driver::jumpRamp(const MapLand &land) {
    const float RAMP_INCLINATION = 22.5f / 90.0f;
    // 384 = 20.0 / (MAX(MAX_LINEAR_SPEED[i]) / 2.0)
    speedUpwards = RAMP_INCLINATION * speedForward * 384.0 * 8.0;
    speedUpwards = std::fmax(speedUpwards, 20.0);
    speedUpwards = std::fmin(speedUpwards, 40.0);
    speedForward = (1.0 - RAMP_INCLINATION) * speedForward;
    // 0.1 = MIN(MAX_LINEAR_SPEED[i])
    speedForward = std::fmax(speedForward, 0.1);

    heightByRamp = true;

    float normalizedAngle = posAngle;
    while (normalizedAngle >= 2 * M_PI) {
        normalizedAngle -= 2 * M_PI;
    }
    while (normalizedAngle < 0) {
        normalizedAngle += 2 * M_PI;
    }

    if (land == MapLand::RAMP_HORIZONTAL) {
        if (normalizedAngle >= 0 && normalizedAngle <= M_PI) {
            flightAngle = M_PI_2;
        } else {
            flightAngle = 3 * M_PI_2;
        }
    } else if (land == MapLand::RAMP_VERTICAL) {
        if (normalizedAngle > M_PI_2 && normalizedAngle < 3 * M_PI_2) {
            flightAngle = M_PI;
        } else {
            flightAngle = 0;
        }
    }
}

void Driver::update(const sf::Time &deltaTime) {
    // Physics variables
    float accelerationLinear = 0.0f;
    // Friction
    if (height == 0) {
        accelerationLinear += VehicleProperties::FRICTION_LINEAR_ACELERATION;
    }
    if ((!Input::held(Key::TURN_LEFT) && !Input::held(Key::TURN_RIGHT)) ||
        (Input::held(Key::TURN_LEFT) && speedTurn > 0.0f) ||
        (Input::held(Key::TURN_RIGHT) && speedTurn < 0.0f)) {
        speedTurn /= 1.2f;
    }

    // remove expired states
    popStateEnd(StateRace::currentTime);

    if (state & (int)DriverState::UNCONTROLLED) {
        animator.hit();
    } else if (state & (int)DriverState::STOPPED) {
        // nothing
    } else {
        if (height == 0) {
            animator.goForward();
        }
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

    // using current position, update acceleration
    accelerationLinear *=
        1.0f + VehicleProperties::POSITION_ACCELERATION_BONUS_PCT * rank;

    MapLand land = Map::getLand(position);
    if (land == MapLand::SLOW && (~state & (int)DriverState::STAR)) {
        if (speedForward > vehicle->slowLandMaxLinearSpeed) {
            accelerationLinear +=
                VehicleProperties::SLOW_LAND_LINEAR_ACELERATION;
        }
    }
    if (height == 0.0f) {
        if (land == MapLand::OIL_SLICK && (~state & (int)DriverState::STAR)) {
            popStateEnd(DriverState::SPEED_UP);
            popStateEnd(DriverState::MORE_SPEED_UP);
            Gui::stopEffects();
            speedTurn = 0.0f;
            speedForward =
                std::fmin(speedForward, vehicle->maxNormalLinearSpeed * 0.6f);
            speedForward = std::fmax(speedForward, 0.01f);
            pushStateEnd(DriverState::UNCONTROLLED,
                         StateRace::currentTime + UNCONTROLLED_DURATION);
        } else if (land == MapLand::RAMP) {
            std::cerr << "ERROR: MapLand::RAMP is deprecated" << std::endl;
        } else if (land == MapLand::RAMP_HORIZONTAL ||
                   land == MapLand::RAMP_VERTICAL) {
            jumpRamp(land);
        } else if (land == MapLand::ZIPPER) {
            if (state & (int)DriverState::SPEED_UP &&
                controlType != DriverControlType::PLAYER) {
                pushStateEnd(DriverState::MORE_SPEED_UP,
                             StateRace::currentTime + MORE_SPEED_UP_DURATION);
                speedForward = vehicle->maxSpeedUpLinearSpeed * 1.2f;
            } else {
                speedForward = vehicle->maxSpeedUpLinearSpeed;
            }
            pushStateEnd(DriverState::SPEED_UP,
                         StateRace::currentTime + SPEED_UP_DURATION);
        } else if (land == MapLand::OTHER) {
            // set a custom destructor to avoid deletion of the object itself
            Map::collideWithSpecialFloorObject(
                DriverPtr(this, [](Driver *) {}));
        }
    }

    // Gravity
    if ((height > 0.0f || speedUpwards > 0.0f) && !onLakitu) {
        // -9.8 * 5.0 MANUAL ADJUST
        const float gravityAceleration = -9.8 * 16.0;
        height = height + speedUpwards * deltaTime.asSeconds() +
                 0.5 * gravityAceleration * deltaTime.asSeconds() *
                     deltaTime.asSeconds();
        if (height < 0.0f) {
            if (!heightByRamp &&
                (Input::held(Key::TURN_LEFT) || Input::held(Key::TURN_RIGHT))) {
                this->pressedToDrift = true;
            }
            heightByRamp = false;
        }
        height = std::fmax(height, 0.0f);
        speedUpwards =
            speedUpwards + gravityAceleration * deltaTime.asSeconds();
        if (height == 0.0f) {
            speedUpwards = 0.0f;
        }
    }

    float maxLinearSpeed;
    if (state & (int)DriverState::MORE_SPEED_UP) {
        maxLinearSpeed = vehicle->maxSpeedUpLinearSpeed * 1.2f;
    } else if (state & (int)DriverState::SPEED_UP ||
               state & (int)DriverState::STAR) {
        maxLinearSpeed = vehicle->maxSpeedUpLinearSpeed;
    } else if (state & (int)DriverState::SPEED_DOWN) {
        maxLinearSpeed = vehicle->maxSpeedDownLinearSpeed;
    } else {
        maxLinearSpeed = vehicle->maxNormalLinearSpeed;
    }
    maxLinearSpeed = maxLinearSpeed + (COIN_SPEED * coins);

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

    float movementAngle = height == 0.0f ? posAngle : flightAngle;
    sf::Vector2f deltaPosition =
        sf::Vector2f(cosf(movementAngle), sinf(movementAngle)) * deltaSpace;

    // collision momentum
    deltaPosition += collisionMomentum;
    collisionMomentum /= 1.2f;
    deltaPosition += vectorialSpeed * deltaTime.asSeconds();
    vectorialSpeed /= 1.3f;

    if (((position + deltaPosition).x < 0.0f ||
         (position + deltaPosition).x > 1.0f) ||
        ((position + deltaPosition).y < 0.0f ||
         (position + deltaPosition).y > 1.0f)) {
        falling = true;
        deltaPosition = sf::Vector2f(0.0f, 0.0f);
    }

    if (!heightByRamp && Map::getLand(position) != MapLand::BLOCK) {
        improvedCheckOfMapLands(this, position, deltaPosition);
    }

    if (falling) {
        speedTurn = 0.0f;
        speedForward = speedForward / 1.1;
        if (controlType == DriverControlType::PLAYER) {
            animator.fall();
            if (!Gui::isBlackScreen()) {
                Map::addEffectDrown(position);
                pushStateEnd(DriverState::STOPPED,
                             StateRace::currentTime + sf::seconds(1.5f));
                Gui::fade(1.5, false);
            }
            Gui::stopEffects();

            if (Gui::isBlackScreen(true)) {
                speedTurn = 0.0f;
                speedForward = 0.0f;
                relocateToNearestGoodPosition();
                reset();
                Gui::fade(1.0, true);
                Lakitu::pickUpDriver(this);
                falling = false;
            }
        }

        else if (controlType != DriverControlType::PLAYER) {
            speedTurn = 0.0f;
            speedForward = 0.0f;
            reset();
            relocateToNearestGoodPosition();
            pushStateEnd(DriverState::STOPPED,
                         StateRace::currentTime + sf::seconds(3.5f));
            falling = false;
        }
    }

    // normal driving
    position += deltaPosition;
    if ((position.x < 0.0f || position.x > 1.0f) ||
        (position.y < 0.0f || position.y > 1.0f)) {
        falling = true;
    }

    if (height == 0) {
        posAngle += deltaAngle;
        posAngle = fmodf(posAngle, 2.0f * M_PI);
    }

    updateGradientPosition();
    animator.update(speedForward, speedTurn, height, deltaTime);

    // Store new position in history
    if (!falling && StateRace::currentTime - pathLastUpdatedAt >
                        FOLLOWED_PATH_UPDATE_INTERVAL) {
        auto it_path = followedPath.rbegin();
        int numOfUpdatesWithoutMoving = 0;
        if (controlType != DriverControlType::PLAYER) {
            while (it_path != followedPath.rend()) {
                if (fabs(position.x - it_path->x) > (1.0f / MAP_TILES_WIDTH)) {
                    break;
                }
                if (fabs(position.y - it_path->y) > (1.0f / MAP_TILES_HEIGHT)) {
                    break;
                }
                if (++numOfUpdatesWithoutMoving >= STEPS_STILL_FOR_RELOCATION) {
                    followedPath.erase(
                        followedPath.end() - STEPS_STILL_FOR_RELOCATION - 5,
                        followedPath.end());
                    prevAcceleration.erase(
                        prevAcceleration.end() - STEPS_STILL_FOR_RELOCATION - 5,
                        prevAcceleration.end());
                    relocateToNearestGoodPosition();
                    break;
                }
                it_path++;
            }
        }

        followedPath.push_back(position);
        prevLap.push_back(laps);
        prevAcceleration.push_back(accelerationLinear);
        pathLastUpdatedAt = StateRace::currentTime;
    }
}

bool Driver::canDrive() const {
    return !(state & (int)DriverState::UNCONTROLLED) &&
           !(state & (int)DriverState::STOPPED);
}

bool Driver::isImmune() const { return state & (int)DriverState::STAR; }

sf::Sprite &Driver::getSprite() { return animator.sprite; }

void Driver::getDrawables(
    const sf::RenderTarget &window, const float scale,
    std::vector<std::pair<float, sf::Sprite *>> &drawables,
    const bool withShadow) {
    // possible player moving/rotation/etc
    float width = window.getSize().x;
    float y = window.getSize().y * 45.0f / 100.0f;
    //(halfHeight * 3) / 4 + animator.sprite.getGlobalBounds().height * 1.05;
    // height is substracted for jump effect
    animator.sprite.setPosition(width / 2.0f, y);
    float moveX = animator.spriteMovementDrift;
    float moveY = animator.spriteMovementSpeed - height * 8.0f;
    animator.sprite.move(moveX * scale, moveY * scale);
    animator.sprite.scale(scale, scale);

    float z = Map::CAM_2_PLAYER_DST / MAP_ASSETS_WIDTH;
    drawables.push_back(std::make_pair(z, &animator.sprite));

    // add shadow too
    if (height > 0.0f && withShadow) {
        spriteShadow.setScale(animator.sprite.getScale() *
                              Map::CIRCUIT_HEIGHT_PCT * 2.0f);
        spriteShadow.setPosition(width / 2.0f, y);
        spriteShadow.move(moveX * scale, animator.spriteMovementSpeed * scale);
        float zShadow = z + 10000.0f;
        int alpha = std::fmaxf((50.0f - height) * 5.0f, 0.0f);
        sf::Color color(255, 255, 255, alpha);
        spriteShadow.setColor(color);
        drawables.push_back(std::make_pair(zShadow, &spriteShadow));
    }
}

void Driver::pushStateEnd(DriverState state, const sf::Time &endTime) {
    this->state |= (int)state;
    stateEnd[(int)log2((int)state)] = endTime;
}

void Driver::popStateEnd(DriverState state) {
    this->state &= ~(int)state;
    stateEnd[(int)log2((int)state)] = sf::seconds(0.0f);
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
                            const float otherWeight, const bool isOtherImmune,
                            const float distance2) {
    // immunity (star) comprobations
    if (isImmune() and !isOtherImmune) {
        data =
            CollisionData(sf::Vector2f(0.0f, 0.0f), 0.4f, CollisionType::HIT);
        return true;
    } else if (!isImmune() and isOtherImmune) {
        return false;
    }
    // either two non-immunes or two immunes
    float mySpeedMod = sqrtf(speedForward * speedForward + 1e-3f);
    float otherSpeedMod = sqrtf(otherSpeed.x * otherSpeed.x +
                                otherSpeed.y * otherSpeed.y + 1e-3f);
    float speedFactor = mySpeedMod / (mySpeedMod + otherSpeedMod);
    float weightFactor =
        sqrtf(vehicle->weight / (vehicle->weight + otherWeight) + 1e-3f);
    sf::Vector2f dir = (otherPos - position) / sqrtf(distance2 + 1e-3f);
    data = CollisionData(dir * mySpeedMod * speedFactor * weightFactor * 0.8f,
                         weightFactor * 0.95f);
    return true;
}

void Driver::getLapTrajectory(unsigned int lap, PathIterator &begin,
                              PathIterator &end) {
    if (lap < indexOfLap.size()) {
        begin = followedPath.cbegin() + indexOfLap[lap - 1];
        if (lap + 1 < indexOfLap.size()) {
            end = followedPath.cbegin() + indexOfLap[lap];
        } else {
            end = followedPath.cend();
        }
    } else {
        begin = followedPath.cend();
        end = followedPath.cend();
    }
}

void Driver::relocateToNearestGoodPosition() {
    unsigned int index = 0;
    if (followedPath.size() >= STEPS_BACK_FOR_RELOCATION) {
        index = followedPath.size() - STEPS_BACK_FOR_RELOCATION;
    }
    position = followedPath[index];
    laps = prevLap[index];
    while (Map::getLand(position) == MapLand::OUTER ||
           Map::getLand(position) == MapLand::SLOW) {
        position += AIGradientDescent::getNextDirection(position);
    }
    for (int i = 0; i < 5; i++) {
        position += AIGradientDescent::getNextDirection(position);
    }
    while (Map::getLand(position) == MapLand::OUTER) {
        position += AIGradientDescent::getNextDirection(position);
    }
    sf::Vector2f next = AIGradientDescent::getNextDirection(position);
    posAngle = std::atan2(next.y, next.x);
}
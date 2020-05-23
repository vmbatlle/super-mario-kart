#include "item.h"

// #define DEBUG_PROBABILITIES  // uncomment to show extra info about AI

#include "entities/banana.h"
#include "entities/greenshell.h"
#include "entities/redshell.h"
#include "map/map.h"
#include "states/race.h"

const sf::Time Item::THUNDER_INITIAL_DURATION = sf::seconds(5.0f);
const sf::Time Item::THUNDER_INCREMENT_DURATION = sf::seconds(1.5f);

void Item::useItem(const DriverPtr &user, const RaceRankingArray &ranking,
                   const bool isFront, const bool forceUse) {
    PowerUps powerup = user->getPowerUp();
    if (powerup == PowerUps::NONE ||
        (!forceUse && (!user->canUsePowerUp() ||
                       (user->controlType == DriverControlType::PLAYER &&
                        !Gui::canUseItem()) ||
                       !user->canDrive() || user->onLakitu))) {
        return;
    }
    // change stuff according to item
    switch (powerup) {
        case PowerUps::BANANA:
            if (user->controlType == DriverControlType::PLAYER) {
                if (isFront) {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_LAUNCH);
                } else {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_DOWN);
                }
            }
            Map::addItem(ItemPtr(new Banana(user->position, user->posAngle,
                                            isFront, user->height)));
            break;
        case PowerUps::COIN:
            user->addCoin(2);
            break;
        case PowerUps::GREEN_SHELL:
            if (user->controlType == DriverControlType::PLAYER) {
                if (isFront) {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_UP);
                } else {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_DOWN);
                }
            }
            Map::addItem(ItemPtr(new GreenShell(user->position, user->posAngle,
                                                isFront, user->height)));
            break;
        case PowerUps::RED_SHELL: {
            if (user->controlType == DriverControlType::PLAYER) {
                if (isFront) {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_UP);
                } else {
                    Audio::play(SFX::CIRCUIT_ITEM_USE_DOWN);
                }
            }
            bool found = false;
            for (unsigned int i = 1; i < ranking.size(); i++) {
                if (ranking[i] == user.get()) {
                    Map::addItem(ItemPtr(
                        new RedShell(user->position, ranking[i - 1],
                                     user->posAngle, isFront, user->height)));
                    found = true;
                    break;
                }
            }
            if (!found) {
                Map::addItem(ItemPtr(new RedShell(user->position, nullptr,
                                                  user->posAngle, isFront,
                                                  user->height)));
            }
        } break;
        case PowerUps::MUSHROOM:
            if (user->controlType == DriverControlType::PLAYER) {
                Audio::play(SFX::CIRCUIT_ITEM_MUSHROOM);
            }
            user->applyMushroom();
            break;
        case PowerUps::STAR:
            user->applyStar();
            break;
        case PowerUps::THUNDER: {
            sf::Time currentDuration = THUNDER_INITIAL_DURATION;
            bool apply = false;
            for (auto rit = ranking.rbegin(); rit != ranking.rend(); ++rit) {
                auto driver = *rit;
                if (!apply && driver == user.get())
                    apply = true;
                else if (apply && !driver->isImmune()) {
                    driver->applyThunder(currentDuration);
                    currentDuration += THUNDER_INCREMENT_DURATION;
                }
            }
            Audio::play(SFX::CIRCUIT_ITEM_THUNDER);
            Gui::thunder();
        } break;
        default:
            std::cerr << "Error: tried to use item without effect" << std::endl;
            break;
    }
    // reset and clean item
    user->pickUpPowerUp(PowerUps::NONE);
    if (user->controlType == DriverControlType::PLAYER) {
        Gui::setPowerUp(PowerUps::NONE);
    }
}

/// --- item "ai" (if statements) ---

// notes:
// methods should return min. probability (you should use your item
// once in a lap) and more probability when it's okay to use the item

// base probabilities

// "large" probability to use it in this very moment
// user should use it quickly but make it seem random
inline float strategyHighest() { return 0.6f; }

// "small" probability (don't use it now, but use it at least once in the lap)
inline float strategyLowest() {
    // in a lap of 30s, AI should have at least 50% prob of using
    // small values are added anyways so its more than 50%
    static constexpr const float SECONDS_PER_LAP = 30.0f;
    static constexpr const float PROB_OF_USING_PER_LAP = 0.5f;
    static constexpr const float checksPerLap =
        SECONDS_PER_LAP * Item::UPDATES_PER_SECOND;
    static const float prob =
        1.0f - powf(1.0f - PROB_OF_USING_PER_LAP, 1.0f / checksPerLap);
    return prob;
}

// convert to 0-1 uniform range to 0-1 non-uniform
inline float scaleProbability(const float percent, const float factor = 0.45f) {
    float base = factor;
    for (int i = 10; i >= 1; i--) {
        if (percent < base) {
            return 1.0f / i - 0.1f;
        }
        base += (1.0f - base) * factor;
    }
    return 1.0f;
}

// dont use if you have 10 coins
AIItemProb strategyLessThanTenCoins(const DriverPtr &user,
                                    const RaceRankingArray &) {
    if (user->getCoins() > 8) {
        float prob = strategyLowest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "More than 8 coins: " << prob << std::endl;
#endif
        return std::make_pair(prob, true);
    } else {
        float prob = strategyHighest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "Less than 8 coins: " << prob << std::endl;
#endif
        return std::make_pair(prob, true);
    }
}

// more probability when user is last
AIItemProb strategyBetterWhenLast(const DriverPtr &user,
                                  const RaceRankingArray &) {
    int userPos = user->rank;
    float prob =
        strategyLowest() + strategyHighest() * scaleProbability(userPos / 7.0f);
#ifdef DEBUG_PROBABILITIES
    std::cout << "Position " << userPos << ": " << prob << std::endl;
#endif
    return std::make_pair(prob, true);
}

// more probability when user is going slower
AIItemProb strategySlowOrStraightLine(const DriverPtr &user,
                                      const RaceRankingArray &) {
    if (user->speedForward / user->vehicle->maxNormalLinearSpeed < 0.3f) {
        float prob = strategyHighest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "Going slow with speed " << user->speedForward << ": "
                  << prob << std::endl;
#endif
        return std::make_pair(prob, true);
    }
    sf::Vector2f delta(0.0f, 0.0f);
    for (unsigned int i = 0; i < 10; i++) {
        delta += AIGradientDescent::getNextDirection(user->position + delta);
    }
    // convert angleDiff to -pi, pi range
    float angleDiff = atan2f(delta.y, delta.x) - user->posAngle;
    angleDiff = fmodf(angleDiff, 2.0f * M_PI);
    if (angleDiff < M_PI * -1.0f) angleDiff += 2.0f * M_PI;
    if (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
    // convert -pi, pi range to 0-1 range where 0 means -pi or pi and 1 means 0
    float prob =
        strategyLowest() +
        strategyHighest() * scaleProbability(1.0f - std::abs(angleDiff / M_PI));
#ifdef DEBUG_PROBABILITIES
    std::cout << "Straight line with angle " << angleDiff << ": " << prob
              << std::endl;
#endif
    return std::make_pair(prob, true);
}

// yeet the banana with grace
AIItemProb strategyBanana(const DriverPtr &user,
                          const RaceRankingArray &ranking) {
    unsigned int userPos = user->rank;
    // check with the 3 drivers in the back
    for (unsigned int i = userPos + 1, j = 0; i < ranking.size() && j < 3;
         i++, j++) {
        const Driver *target = ranking[i];
        sf::Vector2f distance = target->position - user->position;
        float module2 = distance.x * distance.x + distance.y * distance.y;
        if (module2 > 0.01f) {
            // too far away, won't hit it with the banana
            continue;
        }
        // if other driver is going towards my position (and thus it will eat my
        // banana)
        float angleDiff =
            atan2(distance.y, distance.x) - target->posAngle * -1.0f;
        angleDiff = fmodf(angleDiff, 2.0f * M_PI);
        if (angleDiff < M_PI * -1.0f) angleDiff += 2.0f * M_PI;
        if (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
        if (std::abs(angleDiff) * module2 < M_PI / 4.0f * 0.004f) {
            float prob =
                strategyHighest() *
                scaleProbability(1.0f - std::abs(angleDiff / M_PI), 0.25f);
#ifdef DEBUG_PROBABILITIES
            std::cout << "Right behind with angle " << angleDiff
                      << " and distance2 " << module2 << ": " << prob
                      << std::endl;
#endif
            return std::make_pair(prob, false);
        }
    }

    static constexpr const float BANANA_TRAVEL_DISTANCE = 0.20155464379f;
    sf::Vector2f bananaPos =
        user->position +
        sf::Vector2f(cosf(user->posAngle), sinf(user->posAngle)) *
            BANANA_TRAVEL_DISTANCE;

#ifdef DEBUG_PROBABILITIES
    Map::addEffectSparkles(bananaPos);
#endif

    for (unsigned int i = userPos - 2, j = 0; i < ranking.size() && j < 5;
         i++, j++) {
        if (i == userPos) {
            continue;
        }
        const Driver *target = ranking[i];
        sf::Vector2f distance = target->position - user->position;
        if (distance.x * distance.x + distance.y * distance.y > 0.04f) {
            // too far away, won't hit it with the banana
            continue;
        }

        sf::Vector2f pos = target->position;
        for (unsigned int k = 0; k < 3; k++) {
            pos += AIGradientDescent::getNextDirection(pos);
        }
        for (unsigned int k = 0; k < 15; k++) {
            pos += AIGradientDescent::getNextDirection(pos);
            if (std::abs(bananaPos.x - pos.x) < 1.0f / MAP_TILES_WIDTH &&
                std::abs(bananaPos.y - pos.y) < 1.0f / MAP_TILES_HEIGHT) {
                // kobe
#ifdef DEBUG_PROBABILITIES
                std::cout << "Kobe!" << std::endl;
#endif
                return std::make_pair(strategyHighest(), true);
            }
        }
    }

    float prob = strategyLowest();
#ifdef DEBUG_PROBABILITIES
    std::cout << "Couldn't find a good yeet: " << prob << std::endl;
#endif
    return std::make_pair(prob, false);
}

// posangle is the same as the angle formed by delta position
AIItemProb strategyUserInFront(const DriverPtr &user,
                               const RaceRankingArray &ranking) {
    const auto checkAngle = [&user](const Driver *target,
                                    const float maxDistance2,
                                    const float wantedAngle) {
        // tell angle difference
        sf::Vector2f distance = target->position - user->position;
        float module2 = distance.x * distance.x + distance.y * distance.y;
        if (module2 > maxDistance2) {
            return 2.0f * (float)M_PI * module2;
        }
        sf::Vector2f movementDisplacement =
            sf::Vector2f(cosf(target->posAngle), sinf(target->posAngle)) *
            target->speedForward * sqrtf(module2);
        sf::Vector2f shellPos = target->position + movementDisplacement;
        sf::Vector2f throwDelta = shellPos - user->position;
        float angleDiff = atan2(throwDelta.y, throwDelta.x) - wantedAngle;
        angleDiff = fmodf(angleDiff, 2.0f * M_PI);
        if (angleDiff < M_PI * -1.0f) angleDiff += 2.0f * M_PI;
        if (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
        return angleDiff * module2;
    };
    unsigned int userPos = user->rank;
    if (userPos != ranking.size() - 1) {
        float angleBackwards =
            checkAngle(ranking[userPos + 1], 0.015f, user->posAngle * -1.0f);
        if (angleBackwards < M_PI / 4.0f * 0.004f) {
            float prob =
                strategyHighest() *
                scaleProbability(1.0f - std::abs(angleBackwards / M_PI), 0.3f);
#ifdef DEBUG_PROBABILITIES
            std::cout << "Backwards " << angleBackwards << ": " << prob
                      << std::endl;
#endif
            return std::make_pair(prob, false);
        }
    }
    // userPos > 0
    if (userPos == 0) {
        float prob = strategyLowest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "User is first: " << prob << std::endl;
#endif
        return std::make_pair(prob, false);
    }
    float angleForwards =
        checkAngle(ranking[userPos - 1], 0.025f, user->posAngle);
    if (angleForwards > M_PI) {
        float prob = strategyLowest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "Next driver is too far: " << prob << std::endl;
#endif
        return std::make_pair(prob, false);
    }
    // convert -pi, pi range to 0-1 range where 0 means -pi or pi and 1
    // means 0
    float prob = strategyHighest() *
                 scaleProbability(1.0f - std::abs(angleForwards / M_PI), 0.55f);
#ifdef DEBUG_PROBABILITIES
    std::cout << "Tracking next driver with angle " << angleForwards << ": "
              << prob << std::endl;
#endif
    return std::make_pair(prob, true);
}

// for example red shells should be thrown when you're not close to the next
// person (because you might pass them anyway), use it when you're far enough
AIItemProb strategyUseWhenFarFromNextInRanking(
    const DriverPtr &user, const RaceRankingArray &ranking) {
    int userPos = user->rank;
    if (userPos == 0) {
        float prob = strategyLowest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "User is first: " << prob << std::endl;
#endif
        return std::make_pair(prob, false);
    }
    if (user->getLaps() == NUM_LAPS_IN_CIRCUIT) {
        // last lap
        float prob = strategyHighest();
#ifdef DEBUG_PROBABILITIES
        std::cout << "User is in last lap: " << prob << std::endl;
#endif
        return std::make_pair(prob, true);
    }
    const Driver *target = ranking[userPos - 1];
    static constexpr const float MAX_DIFF = 0.1f;
    sf::Vector2f distance = target->position - user->position;
    float modDiff =
        fminf(MAX_DIFF, sqrtf(fmaxf(1e-12f, distance.x * distance.x +
                                                distance.y * distance.y)));

    float prob = strategyHighest() * scaleProbability(modDiff / MAX_DIFF);
#ifdef DEBUG_PROBABILITIES
    std::cout << "Distance " << modDiff << ": " << prob << std::endl;
#endif
    return std::make_pair(prob, true);
}

// return probability 0-1 of using the item
AIItemProb Item::getUseProbability(const DriverPtr &user,
                                   const RaceRankingArray &ranking) {
#ifdef DEBUG_PROBABILITIES
#define PRINT(p, n)                           \
    case PowerUps::p:                         \
        std::cout << n << ": " << std::flush; \
        break;
    switch (user->getPowerUp()) {
        PRINT(NONE, "None")
        PRINT(BANANA, "Banana")
        PRINT(COIN, "Coin")
        PRINT(GREEN_SHELL, "Green Shell")
        PRINT(MUSHROOM, "Mushroom")
        PRINT(RED_SHELL, "Red Shell")
        PRINT(STAR, "Star")
        PRINT(THUNDER, "Thunder")
        default:
            std::cout << "Debug: unrecognized item" << std::endl;
            break;
    }
#undef PRINT
#endif
    switch (user->getPowerUp()) {
        case PowerUps::NONE:
            return std::make_pair(-1.0, false);  // don't use it
        case PowerUps::BANANA:
            return strategyBanana(user, ranking);
        case PowerUps::COIN:
            return strategyLessThanTenCoins(user, ranking);
        case PowerUps::GREEN_SHELL:
            return strategyUserInFront(user, ranking);
        case PowerUps::MUSHROOM:
            return strategySlowOrStraightLine(user, ranking);
        case PowerUps::RED_SHELL:
            return strategyUseWhenFarFromNextInRanking(user, ranking);
        case PowerUps::STAR:
            return strategySlowOrStraightLine(user, ranking);
        case PowerUps::THUNDER:
            return strategyBetterWhenLast(user, ranking);
        default:
            std::cerr << "Error: unrecognized item in Item::getUseProbability"
                      << std::endl;
            return std::make_pair(1.0, 1.0);
    }
}
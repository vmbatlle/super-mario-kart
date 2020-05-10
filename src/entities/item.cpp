#include "item.h"

#include "entities/banana.h"
#include "entities/greenshell.h"
#include "entities/redshell.h"
#include "map/map.h"
#include "states/race.h"

const sf::Time Item::THUNDER_INITIAL_DURATION = sf::seconds(5.0f);
const sf::Time Item::THUNDER_INCREMENT_DURATION = sf::seconds(1.5f);

// void Item::useItem(const DriverPtr &user, const DriverArray &drivers,
void Item::useItem(const DriverPtr &user, const RaceRankingArray &ranking,
                   const bool isFront) {
    PowerUps powerup = user->getPowerUp();
    if (powerup == PowerUps::NONE ||
        (user->controlType == DriverControlType::PLAYER &&
         !Gui::canUseItem()) ||
        !user->canDrive() || user->onLakitu) {
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
            Map::addItem(
                ItemPtr(new Banana(user->position, user->posAngle, isFront)));
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
            Map::addItem(ItemPtr(
                new GreenShell(user->position, user->posAngle, isFront)));
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
            for (uint i = 1; i < ranking.size(); i++) {
                if (ranking[i] == user.get()) {
                    Map::addItem(
                        ItemPtr(new RedShell(user->position, ranking[i - 1],
                                             user->posAngle, isFront)));
                    found = true;
                    break;
                }
            }
            if (!found) {
                Map::addItem(ItemPtr(new RedShell(user->position, nullptr,
                                                  user->posAngle, isFront)));
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
inline float strategyHighest() { return 0.5f; }

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
inline float scaleProbability(const float percent, const float factor = 0.5f) {
    float base = factor;
    for (int i = 10; i >= 1; i--) {
        if (percent < base) {
            return 1.0f / (i * i * i * i);
        }
        base += (1.0f - base) * factor;
    }
    return 1.0f;
}

// dont use if you have 10 coins
AIItemProb strategyLessThanTenCoins(const DriverPtr &user,
                                    const RaceRankingArray &) {
    if (user->getCoins() == 10) {
        return std::make_pair(strategyLowest(), true);
    } else {
        return std::make_pair(strategyHighest(), true);
    }
}

// more probability when user is last
AIItemProb strategyBetterWhenLast(const DriverPtr &user,
                                  const RaceRankingArray &ranking) {
    for (uint i = 0; i < ranking.size(); i++) {
        if (ranking[i] == user.get()) {
            float prob =
                strategyLowest() +
                strategyHighest() * scaleProbability(1.0f - (i / 7.0f));
            return std::make_pair(prob, true);
        }
    }
    std::cerr << "Error: couldn't find user in ranking array" << std::endl;
    return std::make_pair(1.0f, true);
}

// more probability when user is going slower
AIItemProb strategySlowOrStraightLine(const DriverPtr &user,
                                      const RaceRankingArray &) {
    if (user->speedForward / user->vehicle->maxNormalLinearSpeed < 0.3f) {
        return std::make_pair(strategyHighest(), true);
    }
    sf::Vector2f delta(0.0f, 0.0f);
    for (uint i = 0; i < 10; i++) {
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
    return std::make_pair(prob, true);
}

// yeet the banana with grace
AIItemProb strategyBanana(const DriverPtr &user,
                          const RaceRankingArray &ranking) {
    uint userPos = user->rank;
    // check with the 3 drivers in the back
    for (uint i = userPos + 1, j = 0; i < ranking.size() && j < 3; i++, j++) {
        const Driver *target = ranking[i];
        sf::Vector2f distance = target->position - user->position;
        if (distance.x * distance.x + distance.y * distance.y > 0.007f) {
            // too far away, won't hit it with the banana
            continue;
        }
        // if other driver is going towards my position (and thus it will eat my
        // banana)
        float angleDiff = atan2(distance.y, distance.x) - target->posAngle;
        angleDiff = fmodf(angleDiff, 2.0f * M_PI);
        if (angleDiff < M_PI * -1.0f) angleDiff += 2.0f * M_PI;
        if (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
        if (std::abs(angleDiff) < M_PI / 16.0f) {
            float prob = strategyHighest() *
                         scaleProbability(1.0f - std::abs(angleDiff / M_PI));
            return std::make_pair(prob, false);
        }
    }

    static constexpr const float BANANA_TRAVEL_DISTANCE = 0.20155464379f;
    sf::Vector2f bananaPos =
        user->position +
        sf::Vector2f(cosf(user->posAngle), sinf(user->posAngle)) *
            BANANA_TRAVEL_DISTANCE;

    for (uint i = userPos - 2, j = 0; i < ranking.size() && j < 5; i++, j++) {
        if (i == userPos) {
            continue;
        }
        const Driver *target = ranking[i];
        sf::Vector2f distance = target->position - user->position;
        if (distance.x * distance.x + distance.y * distance.y > 0.007f) {
            // too far away, won't hit it with the banana
            continue;
        }

        sf::Vector2f pos = target->position;
        for (uint k = 0; k < 5; k++) {
            pos += AIGradientDescent::getNextDirection(pos);
        }
        for (uint k = 0; k < 10; k++) {
            pos += AIGradientDescent::getNextDirection(pos);
            if (std::abs(bananaPos.x - pos.x) < 0.5f / MAP_TILES_WIDTH &&
                std::abs(bananaPos.y - pos.y) < 0.5f / MAP_TILES_HEIGHT) {
                // kobe
                return std::make_pair(strategyHighest(), true);
            }
        }
    }

    return std::make_pair(strategyLowest(), false);
}

// posangle is the same as the angle formed by delta position
AIItemProb strategyUserInFront(const DriverPtr &user,
                               const RaceRankingArray &ranking) {
    int userPos = user->rank;
    if (userPos == 0) {
        return std::make_pair(strategyLowest(), false);
    }
    // userPos > 0
    const Driver *target = ranking[userPos - 1];
    // tell angle difference
    sf::Vector2f throwDelta = target->position - user->position;
    if (throwDelta.x * throwDelta.x + throwDelta.y * throwDelta.y > 0.01f) {
        return std::make_pair(strategyLowest(), true);
    }
    float angleDiff = atan2(throwDelta.y, throwDelta.x) - user->posAngle;
    angleDiff = fmodf(angleDiff, 2.0f * M_PI);
    if (angleDiff < M_PI * -1.0f) angleDiff += 2.0f * M_PI;
    if (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
    // convert -pi, pi range to 0-1 range where 0 means -pi or pi and 1 means 0
    float prob =
        strategyHighest() * scaleProbability(1.0f - std::abs(angleDiff / M_PI));
    return std::make_pair(prob, true);
}

// for example red shells should be thrown when you're not close to the next
// person (because you might pass them anyway), use it when you're far enough
AIItemProb strategyUseWhenFarFromNextInRanking(
    const DriverPtr &user, const RaceRankingArray &ranking) {
    int userPos = user->rank;
    if (userPos == 0) {
        return std::make_pair(strategyLowest(), false);
    }
    if (user->getLaps() == 5) {
        // last lap
        return std::make_pair(strategyHighest(), true);
    }
    const Driver *target = ranking[userPos - 1];
    static constexpr const float MAX_DIFF = 0.1f;
    sf::Vector2f distance = target->position - user->position;
    float modDiff =
        std::min(MAX_DIFF, std::abs(sqrtf(distance.x * distance.x +
                                          distance.y * distance.y + 1e-3f)));

    float prob = strategyHighest() * scaleProbability(modDiff / MAX_DIFF);
    return std::make_pair(prob, true);
}

// return probability 0-1 of using the item
AIItemProb Item::getUseProbability(const DriverPtr &user,
                                   const RaceRankingArray &ranking) {
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
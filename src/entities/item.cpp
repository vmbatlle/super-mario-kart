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

// TODO minimum probability can be calculated with
// StateRace::ITEM_UPDATES_PER_SECOND assume that a lap is 30s -> # of checks in
// a lap -> probability to use at least once 70-80% of the time

// more probability when user is last
float strategyBetterWhenLast(const DriverPtr &user,
                             const RaceRankingArray &ranking) {
    for (uint i = 0; i < ranking.size(); i++) {
        if (ranking[i] == user.get()) {
            return 0.002f * i * i * i;
        }
    }
    std::cerr << "Error: couldn't find user in ranking array" << std::endl;
    return 1.0f;
}

// user should use it quickly but make it seem random
float strategyASAP(const DriverPtr &, const RaceRankingArray &) { return 0.2f; }

// more probability when user is going slower
float strategyUseWhenGoingSlow(const DriverPtr &user,
                               const RaceRankingArray &ranking) {
    // TODO tener en cuenta que puede ser mejor usarlo en líneas rectas
    if (user->speedForward / user->vehicle->maxNormalLinearSpeed < 0.5f) {
        return strategyASAP(user, ranking);
    } else {
        return 0.003f;
    }
}

// yeet the banana with grace
float strategyBanana(const DriverPtr &, const RaceRankingArray &) {
    // TODO
    // TODO mirar a los 2-3 que tienes detras y si estan en linea recta tirar el
    // platano para atras
    // TODO mirar en que tile cae el platano cuando lo tiras y calcular tiros
    // para delante (usando solo el tile) -> ver a los jugadores y ver si alguno
    // pasaria por encima del tile usando su velocidad o bien el gradiente
    return 0.2f;
}

//
float strategyUserInFront(const DriverPtr &, const RaceRankingArray &) {
    // TODO
    // TODO tener en cuenta la diferencia entre el angulo que tu lo tirarias
    // (posangle), tu angulo con el otro jugador (atan2(pos2 - pos1)) y la
    // distancia
    // TODO tambien tener en cuenta la distancia que recorreria el otro jugador
    // por su velocidad segun la distancia entre tu y el enemigo
    // posicion_a_estimar = posicion_enemigo + velocidad * (factor que depende
    // de tu distancia con el enemigo)
    return 0.2f;
}

// for example red shells should be thrown when you're not close to the next
// person (because you might pass them anyway), use it when you're far enough
float strategyUseWhenFarFromNextInRanking(const DriverPtr &,
                                          const RaceRankingArray &) {
    // TODO
    // TODO en la ultima vuelta (numlaps==5) usarlo ASAP que da igual
    // TODO mirar el camino que hay entre tu y el siguiente corredor y ver que
    // la concha no caeria a la lava (solo recorre TRACK, evita OUTER y SPECIAL
    // (y el resto))
    return 0.2f;
}

// return probability 0-1 of using the item
float Item::getUseProbability(const DriverPtr &user,
                              const RaceRankingArray &ranking) {
    switch (user->getPowerUp()) {
        case PowerUps::NONE:
            return -1.0f;  // don't use it
        case PowerUps::BANANA:
            return strategyBanana(user, ranking);
        case PowerUps::COIN:
            return strategyASAP(user, ranking);
        case PowerUps::GREEN_SHELL:
            return strategyUserInFront(user, ranking);
        case PowerUps::MUSHROOM:
            return strategyUseWhenGoingSlow(user, ranking);
        case PowerUps::RED_SHELL:
            return strategyUseWhenFarFromNextInRanking(user, ranking);
        case PowerUps::STAR:
            return strategyUseWhenGoingSlow(user, ranking);
        case PowerUps::THUNDER:
            return strategyBetterWhenLast(user, ranking);
        default:
            std::cerr << "Error: unrecognized item in Item::getUseProbability"
                      << std::endl;
            return 1.0f;
    }
}
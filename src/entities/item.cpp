#include "item.h"

#include "entities/banana.h"
#include "entities/greenshell.h"
#include "entities/redshell.h"
#include "map/map.h"
#include "states/race.h"


// void Item::useItem(const DriverPtr &user, const DriverArray &drivers,
void Item::useItem(const DriverPtr &user,
                   const RaceRankingArray &ranking, const bool isFront) {
    PowerUps powerup = user->getPowerUp();
    if (powerup == PowerUps::NONE) {
        return;
    }
    // change stuff according to item
    switch (powerup) {
        case PowerUps::BANANA:
            Map::addItem(
                ItemPtr(new Banana(user->position, user->posAngle, isFront)));
            break;
        case PowerUps::COIN:
            user->addCoin(2);
            break;
        case PowerUps::GREEN_SHELL:
            Map::addItem(ItemPtr(
                new GreenShell(user->position, user->posAngle, isFront)));
            break;
        case PowerUps::RED_SHELL: {
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
            user->applyMushroom();
            break;
        case PowerUps::STAR:
            user->applyStar();
            break;
        case PowerUps::THUNDER:
            // TODO only affect players that are ahead of you
            for (Driver* driver : ranking) {
                if (driver != user.get()) {
                    driver->applyThunder();
                } else {
                    break;
                }
            }
            Gui::thunder();
            break;
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
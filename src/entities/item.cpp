#include "item.h"

#include "entities/banana.h"
#include "entities/greenshell.h"
#include "map/map.h"

void Item::useItem(const DriverPtr &user, const std::vector<DriverPtr> &drivers,
                   const bool isFront) {
    //
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
            user->addCoin(10);  // TODO check number of coins
            break;
        case PowerUps::GREEN_SHELL:
            Map::addItem(ItemPtr(
                new GreenShell(user->position, user->posAngle, isFront)));
            break;
        case PowerUps::RED_SHELL:
            user->applyMushroom();
            break;
        case PowerUps::MUSHROOM:
            user->applyMushroom();
            break;
        case PowerUps::STAR:
            user->applyStar();
            break;
        case PowerUps::THUNDER:
            // TODO only affect players that are ahead of you
            for (const DriverPtr &driver : drivers) {
                if (driver != user) {
                    driver->applyThunder();
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
    // if (p->getPowerUp() != PowerUps::NONE) {
    //     // Throw
    //     switch (p->getPowerUp()) {
    //         case PowerUps::BANANA:
    //             if (front) {
    //             } else {
    //             }
    //             break;

    //         case PowerUps::COIN:
    //             p->addCoin(10);
    //             break;

    //         case PowerUps::GREEN_SHELL:
    //             if (front) {
    //             } else {
    //             }

    //             break;

    //         case PowerUps::MUSHROOM:
    //             p->setBonnusSpeed(1.5);

    //             break;
    //         case PowerUps::RED_SHELL:

    //             break;

    //         case PowerUps::STAR:

    //             break;

    //         case PowerUps::THUNDER:

    //             break;

    //         case PowerUps::NONE:

    //             break;
    //     }

    //     // Clean item
    //     p->pickUpPowerUp(PowerUps::NONE);
    //     if (p->controlType == DriverControlType::PLAYER)
    //         Gui::setPowerUp(PowerUps::NONE);
    // }
}
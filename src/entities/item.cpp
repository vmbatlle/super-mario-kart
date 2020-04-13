#include "item.h"

#include "map/map.h"
#include "entities/banana.h"

void Item::useItem(const DriverPtr &user, const bool isFront) {
    PowerUps powerup = user->getPowerUp();
    if (powerup == PowerUps::NONE) {
        return;
    }
    switch (powerup) {
        case PowerUps::BANANA:
        Map::addItem(ItemPtr(new Banana(user->position, user->posAngle, isFront)));
        break;
        default:
        break;
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
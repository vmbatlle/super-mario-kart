#include <cstdlib>
#include <ctime>

#include "entities/enums.h"
#include "game.h"

int main() {
#ifndef DEBUG
    srand(time(0));
#endif
    Game game(BASIC_WIDTH / 2.0f, BASIC_HEIGHT / 2.0f, 60);
    game.run();
}
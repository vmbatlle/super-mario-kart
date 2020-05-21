#include "game.h"
#include <stdlib.h>
#include <time.h>

int main() {
#ifndef DEBUG
    srand(time(0));
#endif
    Game game(256, 224, 60);
    game.run();
}
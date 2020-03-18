#include "game.h"

int main() {
    Game game(512, 448, 60);
    // Game game(1024, 896, 60);
    game.run();
}
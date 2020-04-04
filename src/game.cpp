#include "game.h"

Game::Game(const int _wx, const int _wy, const int _framerate)
    : window(sf::VideoMode(_wx, _wy), "Super Mario Kart"),
      framerate(_framerate),
      gameEnded(false),
      tryPop(0) {
    window.setFramerateLimit(framerate);
    Map::setGameWindow(*this);

    // General asset loading
    // floor objects
    Zipper::loadAssets("assets/objects/floor/misc.png",
                       sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(16, 16)));
    QuestionPanel::loadAssets(
        "assets/objects/floor/question_panel.png",
        sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(16, 16)),
        sf::IntRect(sf::Vector2i(0, 16), sf::Vector2i(16, 16)));
    OilSlick::loadAssets(
        "assets/objects/floor/misc.png",
        sf::IntRect(sf::Vector2i(0, 16), sf::Vector2i(16, 16)));
    Coin::loadAssets("assets/objects/floor/misc.png",
                     sf::IntRect(sf::Vector2i(0, 32), sf::Vector2i(8, 8)));
    // TODO jump bars
    // wall objects
    Pipe::loadAssets("assets/objects/wall/misc.png",
                     sf::IntRect(sf::Vector2i(2, 53), sf::Vector2i(24, 32)),
                     sf::IntRect(sf::Vector2i(158, 53), sf::Vector2i(24, 32)));
    Thwomp::loadAssets(
        "assets/objects/wall/misc.png",
        sf::IntRect(sf::Vector2i(2, 20), sf::Vector2i(24, 32)),
        sf::IntRect(sf::Vector2i(158, 20), sf::Vector2i(24, 32)));

    // TODO move all this loading to another state (maybe race start)
    // Circuit loading
    Map::loadCourse("assets/circuit/donut_plains_1");

    // Player loading based on circuit
    sf::Vector2f posPlayer = Map::getPlayerInitialPosition(1);
    DriverPtr player = DriverPtr(new Driver(
        "assets/drivers/yoshi.png",
        // sf::Vector2f(143.0f / MAP_ASSETS_HEIGHT,
        // 543.0f / MAP_ASSETS_WIDTH), M_PI_2 * -1.0f));
        sf::Vector2f(posPlayer.x, posPlayer.y), M_PI_2 * -1.0f,
        MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::PLAYER));

    // TODO this shouldnt be hardcoded here, it's just a test
    const char *players[7] = {
        "assets/drivers/bowser.png",
        "assets/drivers/dk.png",
        "assets/drivers/koopa.png",
        "assets/drivers/luigi.png",
        "assets/drivers/mario.png",
        "assets/drivers/peach.png",
        "assets/drivers/toad.png"
    };
    std::vector<DriverPtr> drivers = {player};
    for (int pos = 2; pos <= 8; pos++) {
        posPlayer = Map::getPlayerInitialPosition(pos);
        DriverPtr ai = DriverPtr(new Driver(
            players[pos - 2],
            sf::Vector2f(posPlayer.x, posPlayer.y), M_PI_2 * -1.0f,
            MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::AI_GRADIENT));
        drivers.push_back(ai);
    }

    // TODO more menus/etc
    pushState(StatePtr(new StateRace(*this, player, drivers)));
    pushState(StatePtr(new StateStart(*this)));
}

StatePtr Game::getCurrentState() const { return stateStack.top(); }

void Game::handleEvents(const StatePtr& currentState) {
    // Check for close event
    sf::Event event;
    while (window.pollEvent(event)) {
        currentState->handleEvent(event);
        if (event.type == sf::Event::Closed) {
            gameEnded = true;
        }
    }
}

void Game::handleTryPop() {
    while (tryPop > 0) {
        tryPop--;
        if (stateStack.empty()) {
            // TODO mensaje de error? esto no debería pasar
            // (depende de como se piense)
            gameEnded = true;
        } else {
            stateStack.pop();
        }
    }
}

void Game::run() {
    // Delta time logic
    sf::Clock timer;
    sf::Time lastTime = sf::Time::Zero;
    sf::Time fixedUpdateStep = sf::seconds(1.0f / framerate);
    sf::Time fixedUpdateTime = sf::Time::Zero;

    while (window.isOpen()) {
        StatePtr currentState = getCurrentState();

        // Calculate time spent since last frame
        sf::Time time = timer.getElapsedTime();
        sf::Time deltaTime = time - lastTime;
        lastTime = time;

        // Typical game loop
        handleEvents(currentState);  // pass reference to ignore push/pop
        currentState->update(deltaTime);
        fixedUpdateTime += deltaTime;
        while (fixedUpdateTime >= fixedUpdateStep) {
            fixedUpdateTime -= fixedUpdateStep;
            currentState->fixedUpdate(fixedUpdateStep);
            // TODO                   ^^^^^^^^^^^^^^^
            // tiene sentido que los cálculos sean con fixedUpdateStep
            // pero lo he visto también con deltaTime (es una u otra?)
        }
        currentState->draw(window);
        window.display();

        handleTryPop();
        if (gameEnded) {
            window.close();
        }
    }
}

void Game::pushState(const StatePtr& statePtr) { stateStack.push(statePtr); }

void Game::popState() { tryPop++; }  // pop at end of iteration

const sf::RenderWindow& Game::getWindow() const { return window; }
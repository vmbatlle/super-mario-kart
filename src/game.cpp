#include "game.h"

Game::Game(const int _wx, const int _wy, const int _framerate)
    : window(sf::VideoMode(_wx, _wy), "Super Mario Kart"),
      framerate(_framerate),
      gameEnded(false),
      tryPop(0) {
    window.setFramerateLimit(framerate);
    Map::setGameWindow(*this);

    QuestionPanel::loadAssets(
        "assets/track_objects.png",
        sf::IntRect(sf::Vector2i(18, 1), sf::Vector2i(16, 16)),
        sf::IntRect(sf::Vector2i(35, 1), sf::Vector2i(16, 16)));
    Pipe::loadAssets("assets/hazards/pipe.png", sf::IntRect());

    // TODO move this to another place
    DriverPtr player = DriverPtr(new Driver(
        "assets/drivers/yoshi.png",
        // sf::Vector2f(143.0f / Map::ASSETS_HEIGHT,
        // 543.0f / Map::ASSETS_WIDTH), M_PI_2 * -1.0f));
        sf::Vector2f(903.0f / Map::ASSETS_HEIGHT, 444.0f / Map::ASSETS_WIDTH),
        M_PI_2 * -1.0f));
    Map::loadCourse("assets/mario_circuit_2");

    // TODO more menus/etc
    pushState(StatePtr(new StateRace(*this, player)));
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
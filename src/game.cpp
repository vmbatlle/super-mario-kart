#include "game.h"

// Overcome circular dependency errors
#include "states/initload.h"
#include "states/start.h"

Game::Game(const int _wx, const int _wy, const int _framerate)
    : window(sf::VideoMode(_wx, _wy), "Super Mario Kart", WINDOW_STYLE),
      framerate(_framerate),
      gameEnded(false),
      tryPop(0) {
    window.setFramerateLimit(framerate);
    Map::setGameWindow(*this);

    // shouldn't do expensive operations
    pushState(StatePtr(new StateStart(*this)));
    // does all expensive loading on a separate thread
    pushState(StatePtr(new StateInitLoad(*this)));
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
            std::cerr << "Error: Popped too many states" << std::endl;
            gameEnded = true;
        } else {
            stateStack.pop();
            // normal game ending: pop last state in the stack
            if (stateStack.empty()) {
                gameEnded = true;
            }
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
        }
        currentState->draw(window);
        window.display();

        handleTryPop();
        if (gameEnded) {
            stateStack.empty();
            window.close();
        }
    }
}

void Game::pushState(const StatePtr& statePtr) { stateStack.push(statePtr); }

void Game::popState() { tryPop++; }  // pop at end of iteration

const sf::RenderWindow& Game::getWindow() const { return window; }

void Game::setResolution(uint width, uint height) {
    window.close();
    window.create(sf::VideoMode(width, height), "Super Mario Kart",
                  WINDOW_STYLE);
}
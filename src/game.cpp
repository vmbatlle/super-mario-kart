#include "game.h"

// #define DEBUG_STATES  // uncomment to print states

#define PRINT_STATES                                          \
    std::cout << "--- State stack:" << std::endl;             \
    std::stack<StatePtr> printStack(stateStack);              \
    for (unsigned int i = 0; i < stateStack.size(); i++) {    \
        std::cout << std::to_string(i + 1) << ": "            \
                  << printStack.top()->string() << std::endl; \
        printStack.pop();                                     \
    }

// Overcome circular dependency errors
#include "states/initload.h"

Game::Game(const int _bx, const int _by, const int _framerate)
    : baseWidth(_bx),
      baseHeight(_by),
      framerate(_framerate),
      gameEnded(false),
      tryPop(0) {
    Settings::tryLoadSettings();
    updateResolution();
    Map::setGameWindow(*this);
    Input::setGameWindow(getWindow());

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
#ifdef DEBUG_STATES
            PRINT_STATES
#endif
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
        if (deltaTime > sf::seconds(1.0f)) continue;

        // Typical game loop
        handleEvents(currentState);  // pass reference to ignore push/pop
        bool updated = currentState->update(deltaTime);
        fixedUpdateTime += deltaTime;
        while (fixedUpdateTime >= fixedUpdateStep) {
            fixedUpdateTime -= fixedUpdateStep;
            updated = currentState->fixedUpdate(fixedUpdateStep) || updated;
        }
        if (updated) {
            currentState->draw(window);
        }
        window.display();

        handleTryPop();
        if (gameEnded) {
            stateStack.empty();
            window.close();
            Audio::stopMusic();
            Audio::stopSFX();
            Audio::stopEngines();
            Settings::saveSettings();
        }
    }
}

void Game::pushState(const StatePtr& statePtr) {
    stateStack.push(statePtr);
#ifdef DEBUG_STATES
    PRINT_STATES
#endif
}

void Game::popState() { tryPop++; }  // pop at end of iteration

void Game::popStatesUntil(unsigned int i) {
    tryPop = std::max((size_t)0, stateStack.size() - i);
}

const sf::RenderWindow& Game::getWindow() const { return window; }

void Game::getCurrentResolution(unsigned int& width, unsigned int& height) {
    width = baseWidth * Settings::getResolutionMultiplier();
    height = baseHeight * Settings::getResolutionMultiplier();
}

void Game::updateResolution() {
    if (window.isOpen()) {
        window.close();
    }
    unsigned int resolutionMultiplier = Settings::getResolutionMultiplier();
    window.create(sf::VideoMode(baseWidth * resolutionMultiplier,
                                baseHeight * resolutionMultiplier),
                  "Super Mario Kart", WINDOW_STYLE);
    window.setFramerateLimit(framerate);
    Gui::setWindowSize(window.getSize());
    Lakitu::setWindowSize(window.getSize());
}
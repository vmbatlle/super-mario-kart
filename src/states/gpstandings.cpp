#include "gpstandings.h"

sf::Texture StateGPStandings::assetBackground, StateGPStandings::assetStandings;

const sf::Time StateGPStandings::TIME_PER_PLAYER = sf::seconds(0.1f);
const sf::Time StateGPStandings::TIME_FADE = sf::seconds(1.0f);
const sf::Vector2f StateGPStandings::ABS_ROUND_ID =
    sf::Vector2f(24.0f / BACKGROUND_WIDTH, 55.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateGPStandings::REL_CIRCUIT_NAME =
    sf::Vector2f(88.0f / BACKGROUND_WIDTH, 0.0f);
const sf::Vector2f StateGPStandings::ABS_SCOREBOARD_0 =
    sf::Vector2f(48.0f / BACKGROUND_WIDTH, 84.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateGPStandings::REL_SCOREBOARD_DX_DRIVER =
    sf::Vector2f(16.0f / BACKGROUND_WIDTH, 0.0f);
const sf::Vector2f StateGPStandings::REL_SCOREBOARD_DX_POINTS =
    sf::Vector2f(120.0f / BACKGROUND_WIDTH, 0.0f);
const sf::Vector2f StateGPStandings::REL_SCOREBOARD_DY =
    sf::Vector2f(0.0f, 12.0f / BACKGROUND_HEIGHT);

const std::array<int, (int)MenuPlayer::__COUNT>
    StateGPStandings::POINTS_PER_POSITION = {10, 8, 6, 4, 3, 2, 1, 0};

void StateGPStandings::loadAssets(const std::string &assetName,
                                  const sf::IntRect &roiBackground,
                                  const sf::IntRect &roiStandings) {
    assetBackground.loadFromFile(assetName, roiBackground);
    assetBackground.setRepeated(true);
    assetStandings.loadFromFile(assetName, roiStandings);
}

void StateGPStandings::init() {
    currentTime = sf::Time::Zero;
    timeSinceStateChange = sf::Time::Zero;
    playersShown = 8;
    framesSinceOrigin = 0;
    backgroundPosition = 0.0f;
    currentState = AnimationState::FADE_IN;
}

void StateGPStandings::handleEvent(const sf::Event &event) {
    // state is already changing, ignore inputs for now
    if (timeSinceStateChange == sf::Time::Zero) {
        return;
    }
    switch (currentState) {
        case AnimationState::INITIAL_RESULTS:
            if (Input::pressed(Key::MENU_UP, event) ||
                Input::pressed(Key::MENU_DOWN, event) ||
                Input::pressed(Key::MENU_LEFT, event) ||
                Input::pressed(Key::MENU_RIGHT, event) ||
                Input::pressed(Key::ACCEPT, event) ||
                Input::pressed(Key::CANCEL, event)) {
                setState(AnimationState::ANIMATING_DOWN);
                Audio::play(SFX::MENU_SELECTION_ACCEPT);
            }
            break;
        case AnimationState::FINAL_RESULTS:
            if (Input::pressed(Key::MENU_UP, event) ||
                Input::pressed(Key::MENU_DOWN, event) ||
                Input::pressed(Key::MENU_LEFT, event) ||
                Input::pressed(Key::MENU_RIGHT, event) ||
                Input::pressed(Key::ACCEPT, event) ||
                Input::pressed(Key::CANCEL, event)) {
                setState(AnimationState::FADE_OUT);
                Audio::play(SFX::MENU_SELECTION_ACCEPT);
            }
            break;
        default:
            break;
    }
}

bool StateGPStandings::update(const sf::Time &deltaTime) {
    currentTime += deltaTime;
    backgroundPosition -= BACKGROUND_PPS * deltaTime.asSeconds();
    timeSinceStateChange += deltaTime;
    framesSinceOrigin++;

    // fade to black
    if (currentState == AnimationState::FADE_IN &&
        timeSinceStateChange > TIME_FADE) {
        setState(AnimationState::INITIAL_RESULTS);
    } else if (currentState == AnimationState::FADE_OUT &&
               timeSinceStateChange > TIME_FADE) {
        game.popState();
    }

    // hide & show players
    if (currentState == AnimationState::ANIMATING_DOWN &&
        timeSinceStateChange > TIME_PER_PLAYER) {
        if (playersShown == 0) {
            setState(AnimationState::ANIMATING_UP);
            addPointsToPlayers();
        } else {
            playersShown--;
            Audio::play(SFX::MENU_SELECTION_MOVE);
            timeSinceStateChange = sf::Time::Zero;
        }
    } else if (currentState == AnimationState::ANIMATING_UP &&
               timeSinceStateChange > TIME_PER_PLAYER) {
        playersShown++;
        Audio::play(SFX::MENU_SELECTION_MOVE);
        timeSinceStateChange = sf::Time::Zero;
        if (playersShown == positions.size()) {
            setState(AnimationState::FINAL_RESULTS);
        }
    }

    return true;
}

void StateGPStandings::draw(sf::RenderTarget &window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u backgroundSize = assetBackground.getSize();
    float scale = windowSize.y / (float)backgroundSize.y;

    // moving background
    sf::Sprite background(assetBackground);
    int pixelsLeft = backgroundPosition * scale;
    background.setTextureRect(
        {pixelsLeft, 0, (int)backgroundSize.x, (int)backgroundSize.y});
    background.setScale(scale, scale);
    background.setPosition(0.0f, 0.0f);
    window.draw(background);

    // standings overlay
    sf::Sprite standingsOverlay(assetStandings);
    // special case
    float scaleStandings = windowSize.x / assetStandings.getSize().x;
    standingsOverlay.setScale(scaleStandings, scaleStandings);
    standingsOverlay.setPosition(0.0f, 0.0f);
    window.draw(standingsOverlay);

    // round # & circuit
    sf::Vector2f roundPos = ABS_ROUND_ID;
    sf::Vector2f circuitPos = roundPos + REL_CIRCUIT_NAME;
    TextUtils::write(
        window, "round " + std::to_string((unsigned int)circuit + 1),
        sf::Vector2f(roundPos.x * windowSize.x, roundPos.y * windowSize.y),
        scale);
    TextUtils::write(
        window, CIRCUIT_DISPLAY_NAMES[(unsigned int)circuit],
        sf::Vector2f(circuitPos.x * windowSize.x, circuitPos.y * windowSize.y),
        scale);

    // standings scoreboard
    sf::Vector2f rankPos = ABS_SCOREBOARD_0;
    sf::Vector2f namePos = rankPos + REL_SCOREBOARD_DX_DRIVER;
    sf::Vector2f pointsPos = namePos + REL_SCOREBOARD_DX_POINTS;
    sf::Color playerColor = sf::Color::Yellow;
    sf::Color normalColor = Color::Default;
    for (unsigned int i = 0; i < standings.size(); i++) {
        if (standings.size() - i <= playersShown) {
            std::string playerName =
                DRIVER_DISPLAY_NAMES[(unsigned int)standings[i]
                                         .first->getPj()] +
                " ";
            playerName += std::string(15 - playerName.length(), '.');
            std::string points = std::to_string(standings[i].second);
            points = std::string(2 - points.length(), '0') + points;
            if (currentState == AnimationState::FADE_IN ||
                currentState == AnimationState::INITIAL_RESULTS ||
                currentState == AnimationState::ANIMATING_DOWN) {
                int index = findIndexInRanking(standings[i].first);
                points +=
                    " (" + std::to_string(POINTS_PER_POSITION[index]) + ")";
            }
            sf::Color color = standings[i].first->getPj() == player &&
                                      (framesSinceOrigin / 30) % 2 == 0
                                  ? playerColor
                                  : normalColor;
            TextUtils::write(window, std::to_string(i + 1),
                             sf::Vector2f(rankPos.x * windowSize.x,
                                          rankPos.y * windowSize.y),
                             scale, color);
            TextUtils::write(window, playerName,
                             sf::Vector2f(namePos.x * windowSize.x,
                                          namePos.y * windowSize.y),
                             scale, color);
            TextUtils::write(window, points,
                             sf::Vector2f(pointsPos.x * windowSize.x,
                                          pointsPos.y * windowSize.y),
                             scale, color);
        }

        // move to next rank
        rankPos += REL_SCOREBOARD_DY;
        namePos += REL_SCOREBOARD_DY;
        pointsPos += REL_SCOREBOARD_DY;
    }

    // fade to black if necessary
    float fadePct = 0.0f;
    if (currentState == AnimationState::FADE_IN) {
        fadePct = 1.0f - timeSinceStateChange / TIME_FADE;
    } else if (currentState == AnimationState::FADE_OUT) {
        fadePct = timeSinceStateChange / TIME_FADE;
    }
    if (fadePct > 0.0f) {
        int alpha = std::min(fadePct * 255.0f, 255.0f);
        sf::Image black;
        black.create(windowSize.x, windowSize.y, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}
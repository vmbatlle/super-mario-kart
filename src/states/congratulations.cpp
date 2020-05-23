#define _USE_MATH_DEFINES
#include "congratulations.h"

std::array<sf::Texture, StateCongratulations::NUM_GRATS>
    StateCongratulations::assetCongratulations, StateCongratulations::assetOne,
    StateCongratulations::assetTwo, StateCongratulations::assetThree;
std::array<sf::Texture, 3> StateCongratulations::assetSmallTrohpies,
    StateCongratulations::assetBigTrophies,
    StateCongratulations::assetMarioTrophies;
sf::Texture StateCongratulations::assetRibbon;

const sf::Vector2f StateCongratulations::ABS_GRATS =
    sf::Vector2f(22.25f / BACKGROUND_WIDTH, 25.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateCongratulations::ABS_RIBBON =
    sf::Vector2f(79.25f / BACKGROUND_WIDTH, 5.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateCongratulations::ABS_NUMBER1 =
    sf::Vector2f(118.5f / BACKGROUND_WIDTH, 4.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateCongratulations::ABS_NUMBER23 =
    sf::Vector2f(117.0f / BACKGROUND_WIDTH, 4.0f / BACKGROUND_HEIGHT);

const sf::Vector2f StateCongratulations::ABS_TROPHY =
    sf::Vector2f(104.0f / BACKGROUND_WIDTH, 167.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateCongratulations::ABS_TEXT0 =
    sf::Vector2f(128.0f / BACKGROUND_WIDTH, 176.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateCongratulations::REL_TEXTDY =
    sf::Vector2f(0.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);

const sf::Time StateCongratulations::TIME_FADE = sf::seconds(2.0f),
               StateCongratulations::TIME_ANIMATION = sf::seconds(2.5f),
               StateCongratulations::TIME_WAIT = sf::seconds(15.0f);
const sf::Time StateCongratulations::TIME_TROPHY_INITIAL = sf::seconds(4.0f),
               StateCongratulations::TIME_TROPHY_MOVEMENT = sf::seconds(3.0f),
               StateCongratulations::TIME_TROPHY_FADE = sf::seconds(3.0f);
const sf::Vector2f StateCongratulations::CAMERA_DISPLACEMENT =
    sf::Vector2f(0.0f, 0.35f);

const std::array<sf::Vector3f, 3> StateCongratulations::PODIUM_DISPLACEMENTS = {
    sf::Vector3f(-0.05e-2f, 1e-5f, 9.0f), sf::Vector3f(-0.62e-2f, 1e-5f, 6.0f),
    sf::Vector3f(0.5e-2f, 1e-5f, 3.0f)};

void StateCongratulations::loadAssets(
    const std::string &assetName, const sf::IntRect &roiCongrats0,
    const unsigned int congratulationsDY, const sf::IntRect &roiOnes0,
    const sf::IntRect &roiTwos0, const sf::IntRect &roiThrees0,
    const unsigned int numbersDY, const sf::IntRect &roiSmallTrophy0,
    const sf::IntRect &roiBigTrophy0, const sf::IntRect &roiMarioTrophy0,
    const unsigned int trohpiesDX, const sf::IntRect &roiRibbon) {
    sf::IntRect roiCongrats(roiCongrats0);
    for (unsigned int i = 0; i < NUM_GRATS; i++) {
        assetCongratulations[i].loadFromFile(assetName, roiCongrats);
        roiCongrats.top += congratulationsDY;
    }
    sf::IntRect roiOnes(roiOnes0);
    sf::IntRect roiTwos(roiTwos0);
    sf::IntRect roiThrees(roiThrees0);
    for (unsigned int i = 0; i < NUM_GRATS; i++) {
        assetOne[i].loadFromFile(assetName, roiOnes);
        assetTwo[i].loadFromFile(assetName, roiTwos);
        assetThree[i].loadFromFile(assetName, roiThrees);
        roiOnes.top += numbersDY;
        roiTwos.top += numbersDY;
        roiThrees.top += numbersDY;
    }
    sf::IntRect roiSmallTrophy(roiSmallTrophy0);
    sf::IntRect roiBigTrophy(roiBigTrophy0);
    sf::IntRect roiMarioTrophy(roiMarioTrophy0);
    for (unsigned int i = 0; i < 3; i++) {
        assetSmallTrohpies[i].loadFromFile(assetName, roiSmallTrophy);
        assetBigTrophies[i].loadFromFile(assetName, roiBigTrophy);
        assetMarioTrophies[i].loadFromFile(assetName, roiMarioTrophy);
        roiSmallTrophy.left += trohpiesDX;
        roiBigTrophy.left += trohpiesDX;
        roiMarioTrophy.left += trohpiesDX;
    }
    assetRibbon.loadFromFile(assetName, roiRibbon);
}

void StateCongratulations::init(const GrandPrixRankingArray &standings) {
    currentTime = sf::Time::Zero;
    framesSinceOrigin = 0;

    // load circuit & get podium position using player initial positions
    Map::loadCourse(CIRCUIT_ASSET_NAMES[(unsigned int)circuit]);
    targetCameraPosition =
        (Map::getPlayerInitialPosition(1) + Map::getPlayerInitialPosition(2)) /
        2.0f;
    // items use pixel (up to MAP_ASSETS_WIDTH) coordinates
    Map::addItem(ItemPtr(new Podium(targetCameraPosition)));
    Map::addItem(ItemPtr(new FloatingFish(
        targetCameraPosition +
            sf::Vector2f(0.02f * MAP_ASSETS_WIDTH, 0.15f * MAP_ASSETS_HEIGHT),
        sf::Vector2f(-0.04f, -0.03f), false)));
    Map::addItem(ItemPtr(new FloatingFish(
        targetCameraPosition +
            sf::Vector2f(0.04f * MAP_ASSETS_WIDTH, -0.06f * MAP_ASSETS_HEIGHT),
        sf::Vector2f(-0.0015f, 0.0f), false)));
    Map::addItem(ItemPtr(new FloatingFish(
        targetCameraPosition +
            sf::Vector2f(-0.04f * MAP_ASSETS_WIDTH, -0.06f * MAP_ASSETS_HEIGHT),
        sf::Vector2f(0.002f, 0.0005f), true)));

    // spawn player, which uses 0-1 coordinates
    targetCameraPosition =
        sf::Vector2f(targetCameraPosition.x / MAP_ASSETS_WIDTH,
                     targetCameraPosition.y / MAP_ASSETS_HEIGHT);
    pseudoPlayer = DriverPtr(new Driver(
        "assets/drivers/invisible.png",
        targetCameraPosition + CAMERA_DISPLACEMENT, M_PI_2 * -1.0f,
        MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::DISABLED,
        VehicleProperties::GODMODE, MenuPlayer(0)));

    // configure driver array so only first 3 elements matter (last 5 are set
    // to pseudoPlayer so they aren't rendered by getDriverDrawables)
    for (unsigned int i = 0; i < standings.size(); i++) {
        if (standings[i].first->getPj() == player) {
            playerRankedPosition = i + 1;
        }
        if (i < PODIUM_DISPLACEMENTS.size()) {
            Driver *driver = standings[i].first;
            driver->position =
                targetCameraPosition + sf::Vector2f(PODIUM_DISPLACEMENTS[i].x,
                                                    PODIUM_DISPLACEMENTS[i].y);
            driver->height = PODIUM_DISPLACEMENTS[i].z;
            driver->posAngle = M_PI_2;
            driver->controlType = DriverControlType::DISABLED;
            orderedDrivers[i] = DriverPtr(driver, [](Driver *) {});
        } else {
            orderedDrivers[i] = pseudoPlayer;
        }
    }
}

bool StateCongratulations::fixedUpdate(const sf::Time &deltaTime) {
    currentTime += deltaTime;
    framesSinceOrigin++;
    float BEFORE = 0.02f;  // move camera a bit behind the podium
    float displacementPct = BEFORE;
    if (currentTime < TIME_ANIMATION) {
        displacementPct = 1.0f + BEFORE - currentTime / TIME_ANIMATION;
    }
    pseudoPlayer->position =
        targetCameraPosition + CAMERA_DISPLACEMENT * displacementPct;
    if (currentTime > TIME_WAIT && !hasPopped) {
        hasPopped = true;
        game.popState();
    }
    Map::updateObjects(deltaTime);

    return true;
}

void StateCongratulations::draw(sf::RenderTarget &window) {
    // scale
    static constexpr const float NORMAL_WIDTH = 512.0f;
    const float scale = window.getSize().x / NORMAL_WIDTH;
    sf::Vector2u windowSize = game.getWindow().getSize();
    const float PAD_TOP = Map::SKY_HEIGHT_PCT * 2.5f * windowSize.y;

    // black texture
    sf::Image black;
    black.create(windowSize.x, windowSize.y, sf::Color::Black);
    sf::Texture blackTex;
    blackTex.loadFromImage(black);

    window.clear(sf::Color::Black);

    // Get textures from map
    sf::Texture tSkyBack, tSkyFront, tCircuit;
    Map::skyTextures(pseudoPlayer, tSkyBack, tSkyFront);
    Map::circuitTexture(pseudoPlayer, tCircuit);

    // Create sprites and scale them accordingly
    sf::Sprite skyBack(tSkyBack), skyFront(tSkyFront), circuit(tCircuit);
    float backFactor = windowSize.x / (float)tSkyBack.getSize().x;
    float frontFactor = windowSize.x / (float)tSkyFront.getSize().x;
    skyBack.setScale(backFactor, backFactor);
    skyFront.setScale(frontFactor, frontFactor);

    // Sort them correctly in Y position and draw
    float currentHeight = PAD_TOP;
    skyBack.setPosition(0.0f, currentHeight);
    skyFront.setPosition(0.0f, currentHeight);
    window.draw(skyBack);
    window.draw(skyFront);
    currentHeight += windowSize.y * Map::SKY_HEIGHT_PCT;
    circuit.setPosition(0.0f, currentHeight);
    window.draw(circuit);
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;

    // Circuit objects (must be before minimap)
    std::vector<std::pair<float, sf::Sprite *>> wallObjects;
    Map::getWallDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getItemDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getDriverDrawables(window, pseudoPlayer, orderedDrivers, scale,
                            wallObjects);
    std::sort(wallObjects.begin(), wallObjects.end(),
              [](const std::pair<float, sf::Sprite *> &lhs,
                 const std::pair<float, sf::Sprite *> &rhs) {
                  return lhs.first > rhs.first;
              });
    for (const auto &pair : wallObjects) {
        pair.second->move(0.0f, PAD_TOP);
        window.draw(*pair.second);
    }

    // black borders above and below
    sf::Sprite blackOverlay(blackTex);
    blackOverlay.setPosition(0.0f, currentHeight);
    window.draw(blackOverlay);
    blackOverlay.scale(1.0f, PAD_TOP / windowSize.y);
    blackOverlay.setPosition(0.0f, 0.0f);
    window.draw(blackOverlay);

    // UI overlay (text)

    // finished 3 first positions
    if (playerRankedPosition <= 3) {
        sf::Sprite ribbonSprite(assetRibbon);
        ribbonSprite.setPosition(ABS_RIBBON.x * windowSize.x,
                                 ABS_RIBBON.y * windowSize.y);
        ribbonSprite.setScale(scale * 3.0f, scale * 3.0f);
        unsigned int animFrame = (framesSinceOrigin / 3) % NUM_GRATS;
        sf::Sprite congratsSprite(assetCongratulations[animFrame]);
        congratsSprite.setPosition(ABS_GRATS.x * windowSize.x,
                                   ABS_GRATS.y * windowSize.y);
        congratsSprite.setScale(scale * 3.0f, scale * 3.0f);
        sf::Sprite numberSprite;
        if (playerRankedPosition == 1) {
            numberSprite.setTexture(assetOne[animFrame]);
            numberSprite.setPosition(ABS_NUMBER1.x * windowSize.x,
                                     ABS_NUMBER1.y * windowSize.y);
        } else if (playerRankedPosition == 2) {
            numberSprite.setTexture(assetTwo[animFrame]);
            numberSprite.setPosition(ABS_NUMBER23.x * windowSize.x,
                                     ABS_NUMBER23.y * windowSize.y);
        } else {
            numberSprite.setTexture(assetThree[animFrame]);
            numberSprite.setPosition(ABS_NUMBER23.x * windowSize.x,
                                     ABS_NUMBER23.y * windowSize.y);
        }
        numberSprite.setScale(scale * 3.0f, scale * 3.0f);
        window.draw(ribbonSprite);
        window.draw(congratsSprite);
        window.draw(numberSprite);
    } else {
        float textScale = windowSize.x / BACKGROUND_WIDTH;
        TextUtils::write(window, "better luck next time!",
                         sf::Vector2f(128.0f * textScale, 20.0f * textScale),
                         textScale, Color::MenuPrimaryOnFocus, true,
                         TextUtils::TextAlign::CENTER);
    }

    sf::Texture *trophyTexture = nullptr;
    if (playerRankedPosition <= 3) {
        // get texture (will also be used later)
        if (mode == RaceMode::GRAND_PRIX_1 && ccOption == CCOption::CC150) {
            trophyTexture = &assetMarioTrophies[playerRankedPosition - 1];
        } else if (mode == RaceMode::GRAND_PRIX_1) {  // 50cc or 100cc
            trophyTexture = &assetBigTrophies[playerRankedPosition - 1];
        } else {  // versus
            trophyTexture = &assetSmallTrohpies[playerRankedPosition - 1];
        }

        // draw texture in the back
        if (currentTime > TIME_TROPHY_INITIAL + TIME_TROPHY_MOVEMENT) {
            sf::Sprite trophySprite(*trophyTexture);
            trophySprite.setPosition(ABS_TROPHY.x * windowSize.x,
                                     ABS_TROPHY.y * windowSize.y);
            trophySprite.setScale(scale * 6.0f, scale * 6.0f);
            float pct = fmaxf(
                (currentTime - (TIME_TROPHY_INITIAL + TIME_TROPHY_MOVEMENT)) /
                    TIME_TROPHY_FADE,
                0.0f);
            int alpha = fminf(pct * 255.0f, 255.0f);
            trophySprite.setColor(sf::Color(255, 255, 255, alpha));
            window.draw(trophySprite);
        }
    }

    std::string text1;
    switch (ccOption) {
        case CCOption::CC50:
            text1 = "50cc";
            break;
        case CCOption::CC100:
            text1 = "100cc";
            break;
        case CCOption::CC150:
            text1 = "150cc";
            break;
        default:
            std::cerr << "Error: invalid CC option" << std::endl;
            break;
    }
    switch (mode) {
        case RaceMode::GRAND_PRIX_1:
            text1 += " grand prix";
            break;
        case RaceMode::VERSUS:
            text1 += " versus";
            break;
        default:
            std::cerr << "Error: invalid race mode" << std::endl;
            break;
    }
    std::string positionNames[] = {"1st", "2nd", "3rd", "4th",
                                   "5th", "6th", "7th", "8th"};
    std::string text2 = DRIVER_DISPLAY_NAMES[(unsigned int)player] +
                        " finished " + positionNames[playerRankedPosition - 1];
    std::string text3 = "thank you for playing!";
    std::string texts[] = {text1, text2, text3};
    sf::Vector2f textPos(ABS_TEXT0);
    for (int i = 0; i < 3; i++) {
        TextUtils::write(
            window, texts[i],
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f, Color::MenuPrimaryOnFocus, true,
            TextUtils::TextAlign::CENTER);
        textPos += REL_TEXTDY;
    }

    // trophy open animation
    if (playerRankedPosition <= 3 &&
        currentTime < TIME_TROPHY_INITIAL + TIME_TROPHY_MOVEMENT) {
        float pct = fmaxf(
            (currentTime - TIME_TROPHY_INITIAL) / TIME_TROPHY_MOVEMENT, 0.0f);
        sf::Sprite leftBlack(blackTex), rightBlack(blackTex);
        leftBlack.setScale(0.5f, 1.0f - currentHeight / windowSize.y);
        rightBlack.setScale(0.5f, 1.0f - currentHeight / windowSize.y);
        leftBlack.setPosition(0.0f, currentHeight);
        rightBlack.setPosition(0.5f * windowSize.x, currentHeight);
        leftBlack.move(-0.5f * pct * windowSize.x, 0.0f);
        rightBlack.move(0.5f * pct * windowSize.x, 0.0f);
        window.draw(leftBlack);
        window.draw(rightBlack);
        sf::Sprite leftTrophy(*trophyTexture), rightTrophy(*trophyTexture);
        leftTrophy.setTextureRect(sf::IntRect(0, 0, 8, 16));
        rightTrophy.setTextureRect(sf::IntRect(8, 0, 8, 16));
        leftTrophy.setPosition(ABS_TROPHY.x * windowSize.x,
                               ABS_TROPHY.y * windowSize.y);
        rightTrophy.setPosition(
            (ABS_TROPHY.x + 24.0f / BACKGROUND_WIDTH) * windowSize.x,
            ABS_TROPHY.y * windowSize.y);
        leftTrophy.setScale(scale * 6.0f, scale * 6.0f);
        rightTrophy.setScale(scale * 6.0f, scale * 6.0f);
        leftTrophy.move(-0.5f * pct * windowSize.x, 0.0f);
        rightTrophy.move(0.5f * pct * windowSize.x, 0.0f);
        window.draw(leftTrophy);
        window.draw(rightTrophy);
    }

    // fade to black if necessary
    if (currentTime < TIME_FADE) {
        float pct = 1.0f - currentTime / TIME_FADE;
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}
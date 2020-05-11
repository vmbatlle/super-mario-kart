#include "endranks.h"

// driver faces
std::array<sf::Texture, EndRanks::NUM_TEXTURES> EndRanks::assetsHappy,
    EndRanks::assetsSad, EndRanks::assetsNormal;
// ranks 1-8
std::array<sf::Texture, EndRanks::NUM_TEXTURES> EndRanks::numbersBlack,
    EndRanks::numbersYellow, EndRanks::numbersRed;

float EndRanks::FACE_DY, EndRanks::NUMBER_DX;

const sf::Vector2f EndRanks::ABS_POSITION = sf::Vector2f(
    16.0f / EndRanks::NORMAL_WIDTH, 44.5f / EndRanks::NORMAL_HEIGHT);

const RaceRankingArray *EndRanks::ranks;
unsigned int EndRanks::framesSinceOrigin;

void EndRanks::loadAssets(
    const std::string &assetName, const sf::IntRect &roiHappy0,
    const sf::IntRect &roiSad0, const sf::IntRect &roiNormal0,
    const unsigned int roiFaceDY, const sf::IntRect &roiBlack0,
    const sf::IntRect &roiYellow0, const sf::IntRect &roiRed0,
    const unsigned int roiNumberDX) {
    sf::IntRect roiHappy(roiHappy0), roiSad(roiSad0), roiNormal(roiNormal0);
    sf::IntRect roiBlack(roiBlack0), roiYellow(roiYellow0), roiRed(roiRed0);
    FACE_DY = roiFaceDY / NORMAL_HEIGHT;
    NUMBER_DX = roiNumberDX / NORMAL_WIDTH;
    for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
        // faces
        assetsHappy[i].loadFromFile(assetName, roiHappy);
        assetsSad[i].loadFromFile(assetName, roiSad);
        assetsNormal[i].loadFromFile(assetName, roiNormal);
        roiHappy.top += roiFaceDY;
        roiSad.top += roiFaceDY;
        roiNormal.top += roiFaceDY;
        // numbers
        numbersBlack[i].loadFromFile(assetName, roiBlack);
        numbersYellow[i].loadFromFile(assetName, roiYellow);
        numbersRed[i].loadFromFile(assetName, roiRed);
        roiBlack.left += roiNumberDX;
        roiYellow.left += roiNumberDX;
        roiRed.left += roiNumberDX;
    }
}

void EndRanks::update(const sf::Time &) {
    if (ranks == nullptr) {
        return;
    }
    framesSinceOrigin++;
}

void EndRanks::draw(sf::RenderTarget &window) {
    if (ranks == nullptr) {
        return;
    }

    sf::Vector2u windowSize = window.getSize();
    float scale = windowSize.x / NORMAL_WIDTH;
    sf::Vector2f pos(ABS_POSITION);
    for (unsigned int i = 0; i < ranks->size(); i++) {
        const Driver *driver = (*ranks)[i];
        if (driver->getLaps() <= NUM_LAPS_IN_CIRCUIT) {  // hasn't finished
            break;
        }
        sf::Sprite numberSprite;
        // flash numbers 1-4, 5-8 dont flash (careful with 0-index)
        if (i >= 4 || (framesSinceOrigin / 4) % 3 == 0) {
            numberSprite.setTexture(numbersBlack[i]);
        } else if ((framesSinceOrigin / 4) % 3 == 1) {
            numberSprite.setTexture(numbersYellow[i]);
        } else {
            numberSprite.setTexture(numbersRed[i]);
        }
        numberSprite.setPosition(pos.x * windowSize.x, pos.y * windowSize.y);
        numberSprite.setScale(scale, scale);
        window.draw(numberSprite);

        sf::Sprite driverSprite;
        // drivers 1-4 change expression
        unsigned int driverId = (unsigned int)driver->getPj();
        if (i >= 4) {
            driverSprite.setTexture(assetsSad[driverId]);
        } else if ((framesSinceOrigin / 15) % 2 == 0) {
            driverSprite.setTexture(assetsNormal[driverId]);
        } else {
            driverSprite.setTexture(assetsHappy[driverId]);
        }
        driverSprite.setPosition((pos.x + NUMBER_DX) * windowSize.x,
                                 pos.y * windowSize.y);
        driverSprite.setScale(scale, scale);
        window.draw(driverSprite);

        pos.y += FACE_DY;
    }
}

void EndRanks::reset(const RaceRankingArray *_ranks) {
    ranks = _ranks;
    framesSinceOrigin = 0;
}
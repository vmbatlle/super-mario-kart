#include "driveranimator.h"

#include "map/map.h"

// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val)
{
  hue %= 360;
  while(hue<0) hue += 360;

  if(sat<0.f) sat = 0.f;
  if(sat>1.f) sat = 1.f;

  if(val<0.f) val = 0.f;
  if(val>1.f) val = 1.f;

  int h = hue/60;
  float f = float(hue)/60-h;
  float p = val*(1.f-sat);
  float q = val*(1.f-sat*f);
  float t = val*(1.f-sat*(1-f));

  switch(h)
  {
    default:
    case 0:
    case 6: return sf::Color(val*255, t*255, p*255);
    case 1: return sf::Color(q*255, val*255, p*255);
    case 2: return sf::Color(p*255, val*255, t*255);
    case 3: return sf::Color(p*255, q*255, val*255);
    case 4: return sf::Color(t*255, p*255, val*255);
    case 5: return sf::Color(val*255, p*255, q*255);
  }
}

DriverAnimator::DriverAnimator(const char* spriteFile, DriverControlType control) {
    for (int i = 0; i < 12; i++)
        driving[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 32, 32, 32));
    for (int i = 0; i < 5; i++)
        others[i].loadFromFile(spriteFile, sf::IntRect(32 * i, 0, 32, 32));

    sprite.setTexture(driving[0]);

    smashTime = sf::seconds(0);
    starTime = sf::seconds(0);
    starColor = 0;
    sScale = 2;

    state = PlayerState::GO_FORWARD;
    sprite.setOrigin(driving[0].getSize().x / 2,
                     sprite.getGlobalBounds().height);
    sprite.scale(sScale, sScale);
    if (control == DriverControlType::AI_GRADIENT) {
        sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
        sScale = Map::CIRCUIT_HEIGHT_PCT;
    }
}

void DriverAnimator::goForward() {
    state = PlayerState::GO_FORWARD;
}

void DriverAnimator::goRight() {
    state = PlayerState::GO_RIGHT;
}

void DriverAnimator::goLeft() {
    state = PlayerState::GO_LEFT;
}

void DriverAnimator::hit() { state = PlayerState::HIT; }

void DriverAnimator::fall() { state = PlayerState::FALLING; }

void DriverAnimator::update(float speedTurn, const sf::Time &deltaTime) {
    switch (state) {
        case PlayerState::GO_FORWARD:
            sprite.setTexture(driving[0]);
            sprite.setScale(sScale, sScale);
            break;

        case PlayerState::GO_RIGHT:
            if (speedTurn < 1.0f * (1.f / 4))
                sprite.setTexture(driving[1]);
            else if (speedTurn < 1.0f * (1.f / 2))
                sprite.setTexture(driving[2]);
            else if (speedTurn < 1.0f * (3.f / 4))
                sprite.setTexture(driving[3]);
            else
                sprite.setTexture(driving[4]);
            sprite.setScale(sScale, sScale);
            break;

        case PlayerState::GO_LEFT:
            if (speedTurn > -1.0f * (1.f / 4))
                sprite.setTexture(driving[1]);
            else if (speedTurn > -1.0f * (1.f / 2))
                sprite.setTexture(driving[2]);
            else if (speedTurn > -1.0f * (3.f / 4))
                sprite.setTexture(driving[3]);
            else
                sprite.setTexture(driving[4]);
            sprite.setScale(-sScale, sScale);
            break;

        case PlayerState::GO_BACK:
            /* code */
            break;

        case PlayerState::HIT:
            hitPos = (hitPos + 1) % 22;
            sprite.setTexture(driving[hitTextuIdx[hitPos]]);
            if (hitPos > 11) {
                sprite.setScale(-sScale, sScale);
            } else
                sprite.setScale(sScale, sScale);
            break;

        case PlayerState::FALLING:  // NO va
        {
            sf::Vector2f s = sprite.getScale();
            if (s.x > 0) {
                // TODO this shouldnt be here
                sprite.scale(0.9f, 0.9f);
            }
            break;
        }

        case PlayerState::CRASH:
            /* code */
            break;

        default:
            sprite.setTexture(driving[0]);
            break;
    }

    if (smallTime > sf::seconds(0)) {
        smallTime -= deltaTime;
        sprite.scale(1/2,1/2);
    }

    if (smashTime > sf::seconds(0)) {
        smashTime -= deltaTime;
        sprite.scale(1,1/2);
    }

    if (starTime > sf::seconds(0)) {
        starTime -= deltaTime;
        sprite.setColor(hsv(starColor,1.0f, 1.0f));
        starColor += 7;
    } else {
        sprite.setColor(sf::Color::White);
        starColor = 0;
    }
}

bool DriverAnimator::canDrive() const {
    return state != PlayerState::HIT || state != PlayerState::FALLING;
}

void DriverAnimator::small(sf::Time duration) {
    smallTime = duration;
}

void DriverAnimator::smash(sf::Time duration) {
    smashTime = duration;
}

void DriverAnimator::star(sf::Time duration) {
    starTime = duration;
}

sf::Sprite DriverAnimator::getMinimapSprite(float angle) const {
    sf::Sprite minimapSprite(sprite);  // copy sprite (important for scale)
    minimapSprite.setScale(sScale, sScale);
     
    angle += M_PI / 2;                  // adjust
    angle = fmodf(angle, 2.0f * M_PI);  // 0-2pi range

    if (angle < 0)  // 0-2pi range
        angle += 2.0f * M_PI;

    if (state != PlayerState::HIT && state != PlayerState::FALLING) {
        for (int i = 1; i <= 23; i++) {
            if (angle <= ((i - 0.5f) * 2.0f * M_PI) / 22.0f) {
                minimapSprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (i > 11) minimapSprite.scale(-1, 1);
                break;
            }
        }
    } else {
        if (hitPos > 11) {
            minimapSprite.scale(-1, 1);
        }
    }

    if (smallTime > sf::seconds(0)) {
        minimapSprite.scale(0.5,0.5);
    }
    if (smashTime > sf::seconds(0)) {
        minimapSprite.scale(1,0.5);
    }
    

    return minimapSprite;
}

void DriverAnimator::setViewSprite(float viewerAngle, float driverAngle) {
    float diff = viewerAngle - driverAngle;

    diff = fmodf(diff, 2.0f * M_PI);
    if (diff < 0)  // 0-2pi range
        diff += 2.0f * M_PI;

    sprite.setScale(Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
    if (sprite.getScale().x < 0) {
        sprite.setScale(-Map::CIRCUIT_HEIGHT_PCT, Map::CIRCUIT_HEIGHT_PCT);
    }

    if (state != PlayerState::HIT && state != PlayerState::FALLING) {
        for (int i = 1; i <= 23; i++) {
            if (diff <= ((i - 0.5f) * 2.0f * M_PI) / 22.0f) {
                sprite.setTexture(driving[hitTextuIdx[i - 1]]);
                if (diff < M_PI) {
                    sprite.scale(-1, 1);
                }
                break;
            }
        }
    } else {
        if (hitPos > 11) {
            sprite.scale(-1, 1);
        }
    }

    if (smallTime > sf::seconds(0)) {
        sprite.scale(0.5,0.5);
    }
    if (smashTime > sf::seconds(0)) {
        sprite.scale(1,0.5);
    }
}

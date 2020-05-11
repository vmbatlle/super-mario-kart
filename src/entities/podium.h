#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class Podium : public Item {
   private:
    static sf::Texture assetPodium;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    Podium(const sf::Vector2f &position);

    void update(const sf::Time &) override{};

    inline std::string name() const override { return "Podium"; }
};
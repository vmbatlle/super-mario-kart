#include <SFML/Graphics.hpp>
#include <vector>

#define _USE_MATH_DEFINES

#include <cmath>

int main()
{
	int wx = 1000, wy = 800;
	int radius = 5;

    sf::RenderWindow window(sf::VideoMode(wx, wy), "SFML works!");
    //sf::CircleShape shape(radius);
    sf::Sprite shape;

    //shape.setFillColor(sf::Color::Yellow);

	// get the target texture (where the stuff has been drawn)
	sf::Texture texture;
	texture.loadFromFile("star.bmp");

    std::array<sf::CircleShape, 1000> circles;
    for (auto& circle : circles) {
        circle = sf::CircleShape(radius);
	    circle.setTexture(&texture);
        circle.setPosition(wx / 2, wy / 2);
        circle.setScale(0.1f,0.1f);
        circle.setOrigin(5,5);
    }
    std::array<sf::Vector2f, 1000> move;
    for (auto& vec : move) {
        float angle = rand() % 360;
        vec.x = 10 * sinf(angle * M_PI / 180.0f);
        vec.y = 10 * cosf(angle * M_PI / 180.0f);
    }
    int i = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (int m = 0; m < 1000; m++) {
            circles[m].move(move[m]);
            circles[m].rotate(1.f);
            sf::Vector2f scale = circles[m].getScale();
            if (scale.x < 10) circles[m].setScale(scale.x+.1f, scale.y+.1f);
        }        
        circles[i].setPosition(wx / 2, wy / 2);
        circles[i].setScale(0.1f,0.1f);
        i = ++i % 1000;


		sf::sleep(sf::milliseconds(15));



        window.clear();
        for (auto& circle : circles) {
            window.draw(circle);
        }
        window.display();
    }

    return 0;
}
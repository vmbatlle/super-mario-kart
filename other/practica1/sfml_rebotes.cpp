#include <SFML/Graphics.hpp>

int main()
{
	int wx = 1024, wy = 658;
	int radius = 100;
	int x = 10, y = 10;

    sf::RenderWindow window(sf::VideoMode(wx, wy), "SFML works!");
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color::Cyan);

	// get the target texture (where the stuff has been drawn)
	sf::Texture texture;
	texture.loadFromFile("hello_world.bmp");
	shape.setTexture(&texture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


		shape.move(x, y);
		sf::Vector2f pos = shape.getPosition();
		if (pos.x + 2 * radius >= wx || pos.x <= 0) {
			x = x * -1;
		}
		if (pos.y + 2 * radius >= wy || pos.y <= 0) {
			y = y * -1;
		}
		sf::sleep(sf::milliseconds(15));



        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
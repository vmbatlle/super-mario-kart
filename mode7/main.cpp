#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
 
 
class Mode7 final
{
 
    private:
        unsigned int m_screenWidth, m_screenHeight;
        unsigned int m_T, m_L; // variable intermédiare m_T = m_screenHeght /2 et m_L = mscreenWidth / 2 (T: top, L: left)
        unsigned int m_imageWidth, m_imageHeight;
        float m_FOV, m_D; // m_D distance du point de la caméra au plan de projection
        float m_cameraX, m_cameraY, m_cameraZ;
        float m_cosinus, m_sinus;
        sf::Image m_image, m_imageTransformed;
        sf::Texture m_texture;
        sf::Sprite m_sprite;
 
    public:
        Mode7() = delete;
        Mode7(const Mode7&) = delete;
        Mode7(const Mode7&&) = delete;
        Mode7(std::string const &file, unsigned int width, unsigned int height, float x, float y, float z, float theta, float FOV);
        void setScreen(unsigned int width, unsigned int height);
        void loadImage(std::string const &file);
        void setCamera(float x, float y, float z);
        void setFOVangle(float angle);
        void setTheta(float theta);
        void calc();
        sf::Sprite getSprite();
 
        void calc2();
};
 
/*Mode7::Mode7() : m_screenWidth{}, m_screenHeight{}, m_T{}, m_L{},
            m_imageWidth{}, m_imageHeight{}, m_FOV{}, m_D{}, m_cameraX{},
            m_cameraY{}, m_cameraZ{}, m_cosinus{1}, m_sinus{} {}
*/
 
Mode7::Mode7(std::string const &file, unsigned int width, unsigned int height, float cameraX, float cameraY, float cameraZ, float theta, float FOV)
{
    setScreen(width, height);
 
    try
    {
        loadImage(file);
    }
    catch(char const *s)
    {
        std::cout << s << " " << file << std::endl;
        exit(EXIT_FAILURE);
    }
 
    setCamera(cameraX, cameraY, cameraZ);
    setFOVangle(FOV);
    setTheta(theta);
}
 
void Mode7::setScreen(unsigned int width, unsigned int height)
{
    m_screenWidth = width;
    m_screenHeight = height;
    m_T = height / 2;
    m_L = width / 2;
 
    m_imageTransformed.create(m_screenWidth, m_screenHeight, sf::Color::White);
    m_texture.create(m_screenWidth, m_screenHeight);
}
 
void Mode7::loadImage(std::string const &file)
{
    sf::Vector2u vec;
 
    if(!m_image.loadFromFile(file))
        throw "Unable to load";
    //std::cerr << "Unable to load" << std::endl;
 
    vec = m_image.getSize();
    m_imageWidth = vec.x;
    m_imageHeight = vec.y;
}
 
void Mode7::setCamera(float x, float y, float z)
{
    m_cameraX = x;
    m_cameraY = y;
    m_cameraZ = z;
}
 
void Mode7::setFOVangle(float angle)
{
    m_FOV = angle;
    m_D = m_T / std::tan(m_FOV * 3.1415 / 360.0);
}
 
void Mode7::setTheta(float theta)
{
    m_cosinus = std::cos(theta * 3.1415 / 180.0);
    m_sinus = std::sin(theta * 3.1415 / 180.0);
}
 
void Mode7::calc()
{
    for(unsigned int ys{m_T + 1}; ys < m_screenHeight; ys++)
        for(unsigned int xs{}; xs < m_screenWidth; xs++)
            {
                float const xw = m_cameraX - m_cameraY*((float)xs - (float)m_L) / ((float)m_T - (float)ys) * m_cosinus - m_D*m_cameraY / ((float)m_T - (float)ys) * m_sinus;
                float const zw = m_cameraZ - m_cameraY*((float)xs - (float)m_L) / ((float)m_T - (float)ys) * m_sinus + m_D*m_cameraY / ((float)m_T - (float)ys) * m_cosinus;
 
                if(xw < 0 || zw < 0 || xw >= m_imageWidth || zw >= m_imageHeight)
                    m_imageTransformed.setPixel(xs, ys, sf::Color::White);
                else
                    m_imageTransformed.setPixel(xs, ys, m_image.getPixel((unsigned int)xw, (unsigned int )zw));
            }
}
 
 
void Mode7::calc2()
{
    for(unsigned int ys{m_T + 1}; ys < m_screenHeight; ys++)
        for(unsigned int xs{}; xs < m_screenWidth; xs++)
            {
                float const xw = m_cosinus*(((float)xs - (float)m_L) / ((float)m_T - (float)ys) * m_cameraY - m_cameraX) + m_sinus * ((float)m_D / ((float)m_T - (float)ys) * m_cameraY + m_cameraZ);
                float const zw = m_sinus*(((float)xs - (float)m_L) / ((float)m_T - (float)ys) * m_cameraY - m_cameraX) - m_cosinus * ((float)m_D / ((float)m_T - (float)ys) * m_cameraY + m_cameraZ);
 
                if(xw < 0 || zw < 0 || xw >= m_imageWidth || zw >= m_imageHeight)
                    m_imageTransformed.setPixel(xs, ys, sf::Color::Blue);
                else
                    m_imageTransformed.setPixel(xs, ys, m_image.getPixel((unsigned int)xw, (unsigned int )zw));
            }
}
 
sf::Sprite Mode7::getSprite()
{
    m_texture.update(m_imageTransformed);
    m_sprite.setTexture(m_texture);
 
    return m_sprite;
}
 
int main()
{
    float cameraX{20.0}, cameraY{10.0}, cameraZ{20.0};
    float theta{};
 
    unsigned int tempsMoyen{};
    unsigned int frame{};
 
    // creation d'une fenetre avec la bibliothèque SFML et limitation du FPS à 30
    sf::RenderWindow window{sf::VideoMode(640, 480), "Mode7_2cpp"};
    window.setFramerateLimit(30);
 
    // initialisation
    Mode7 x{"./kart_map.png", 640, 480, cameraX, cameraY, cameraZ, theta, 100.0};
 
 
    // définition d'un cercle de rayon 10 au centre de l'écran
    sf::CircleShape circle{10.0};
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(640.0/2.0 - 10.0, 480.0 / 2.0 + 90.0);
 
    while(window.isOpen())
    {
        sf::Event event;
 
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
 
                case sf::Event::KeyPressed:
                    switch(event.key.code)
                    {
                        case sf::Keyboard::Numpad3: // camera height
                            cameraY++;
                            break;
 
                        case sf::Keyboard::Numpad1: // camera height
                            cameraY--;
                            break;
 
                    // déplacement de la caméra
                        case sf::Keyboard::Left:
                            cameraX--;
                            break;
 
                        case sf::Keyboard::Right:
                            cameraX++;
                            break;
 
                        case sf::Keyboard::Up:
                            cameraZ--;
                            break;
 
                        case sf::Keyboard::Down:
                            cameraZ++;
                            break;
 
 
                    // rotation de la caméra
                        case sf::Keyboard::Numpad7:
                            theta--;
                            break;
 
                        case sf::Keyboard::Numpad9:
                            theta++;
                            break;
 
                        default:
                            break;
                    }
                    break;
 
                default:
                    break;
            }
        }
 
        window.clear(sf::Color::White);
 
/* ---------- */
sf::Clock clock;
 
        // calcul de l'image à afficher en fonction de la position de la caméra et de l'angle theta
        x.setCamera(cameraX, cameraY, cameraZ);
        x.setTheta(theta);
        x.calc();
 
 
sf::Time elapsed1 = clock.getElapsedTime();
    std::cout << "Temps écoulé : " << elapsed1.asMilliseconds() << "ms" << std::endl;
    clock.restart();
 
    frame++;
    tempsMoyen += elapsed1.asMilliseconds();
/* ---------- */
 
        // affichage
        window.draw(x.getSprite());
        window.draw(circle);
        window.display();
    }
 
 
    std::cout << "Temps moyen de calcul : " << tempsMoyen/frame << "ms"  << std::endl
    << "Nombres d'images affichées : " << frame << std::endl;
 
    return 0;
}
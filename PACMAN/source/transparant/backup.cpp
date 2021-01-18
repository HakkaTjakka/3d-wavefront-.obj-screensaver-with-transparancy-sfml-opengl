#include <SFML/Graphics.hpp>

#include <windows.h>
#include <math.h>

bool setShape(HWND hWnd, const sf::Image& image)
{
    const sf::Uint8* pixelData = image.getPixelsPtr();
    HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

    // Determine the visible region
    for (unsigned int y = 0; y < image.getSize().y; y++)
    {
        for (unsigned int x = 0; x < image.getSize().x; x++)
        {
            if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0)
            {
                HRGN hRegionDest = CreateRectRgn(0, 0, 1, 1);
                HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
                CombineRgn(hRegionDest, hRegion, hRegionPixel, RGN_XOR);
                DeleteObject(hRegion);
                DeleteObject(hRegionPixel);
                hRegion = hRegionDest;
            }
        }
    }

    SetWindowRgn(hWnd, hRegion, true);
    DeleteObject(hRegion);
    return true;
}

bool setTransparency(HWND hWnd, unsigned char alpha)
{
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
    return true;
}


int main()
{
    // Change this to the wanted transparency
    const unsigned char opacity = 185;

    // Load an image with transparent parts that will define the shape of the window
    sf::Image backgroundImage;
    backgroundImage.loadFromFile("image.png");

    // Create the window and center it on the screen
    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window", sf::Style::None);
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    // These functions return false on an unsupported OS or when it is not supported on linux (e.g. display doesn't support shape extention)
    setShape(window.getSystemHandle(), backgroundImage);
    setTransparency(window.getSystemHandle(), opacity);

    // We will also draw the image on the window instead of just showing an empty window with the wanted shape
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);

    // Main loop to display the image while the window is open (pressing the escape key to close the window)

    int w_x=window.getSize().x/2;
    int w_y=window.getSize().y/2;
    float tt=0.0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }


        float x=20*sin(tt);
        float y=20*cos(tt);
        tt+=0.05;
        backgroundSprite.setPosition(x,y);
        window.clear(sf::Color::Transparent);
        window.draw(backgroundSprite);

        window.display();
    }

    return 0;
}


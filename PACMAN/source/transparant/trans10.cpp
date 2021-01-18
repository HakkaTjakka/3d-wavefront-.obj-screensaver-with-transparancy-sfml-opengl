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

//    SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
    SetLayeredWindowAttributes(hWnd, 0x00000000, alpha, LWA_COLORKEY);

    return true;

}


int main()
{
    // Change this to the wanted transparency
    const unsigned char opacity = 255;

    // Load an image with transparent parts that will define the shape of the window
    sf::Image backgroundImage;
//    backgroundImage.loadFromFile("trans1978x1078.png");
    backgroundImage.loadFromFile("trans1918x1078_50.png");

    // Create the window and center it on the screen
    sf::ContextSettings contextSettings; //(0,0,2,3,0);

    contextSettings.depthBits    = 24;
    contextSettings.stencilBits  = 8;
    contextSettings.antialiasingLevel = 3;
    contextSettings.majorVersion = 3;
    contextSettings.minorVersion = 3;
    contextSettings.sRgbCapable = false;

    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window", sf::Style::None ,contextSettings);
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    // These functions return false on an unsupported OS or when it is not supported on linux (e.g. display doesn't support shape extention)
    window.setVerticalSyncEnabled(true);
    setShape(window.getSystemHandle(), backgroundImage);
    setTransparency(window.getSystemHandle(), opacity);

    // We will also draw the image on the window instead of just showing an empty window with the wanted shape

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    backgroundTexture.loadFromFile("image.png");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundTexture.setSmooth(true);
    // Main loop to display the image while the window is open (pressing the escape key to close the window)

    int w_x=window.getSize().x/2;
    int w_y=window.getSize().y/2;

    sf::RenderTexture renderTexture;
    renderTexture.create(backgroundImage.getSize().x, backgroundImage.getSize().y, 32);
    renderTexture.setSmooth(true);

    sf::Sprite renderTextureSprite(renderTexture.getTexture());
    renderTextureSprite.setColor({255, 255, 0, 255});
    renderTextureSprite.setOrigin(backgroundTexture.getSize().x/2,backgroundTexture.getSize().y/2);
    printf("%d,%d\n",backgroundTexture.getSize().x/2,backgroundTexture.getSize().y/2);
//    renderTextureSprite.setOrigin(20,20);
/*
    sf::BlendMode::Zero,
    sf::BlendMode::One,
    sf::BlendMode::SrcColor,
    sf::BlendMode::OneMinusSrcColor,
    sf::BlendMode::DstColor,
    sf::BlendMode::OneMinusDstColor,
    sf::BlendMode::SrcAlpha,
    sf::BlendMode::OneMinusSrcAlpha,
    sf::BlendMode::DstAlpha,
    sf::BlendMode::OneMinusDstAlpha

    sf::BlendMode::Add,
    sf::BlendMode::Subtract,
    sf::BlendMode::ReverseSubtract,
*/

//    sf::BlendMode MyBlend=sf::BlendMode(sf::BlendAlpha);

    sf::RenderStates states;

    sf::Shader shaderAlpha;
    shaderAlpha.loadFromFile( "alpha.frag", sf::Shader::Fragment );
//    shaderAlpha.setUniform( "texture", renderBlurY.getTexture());
    shaderAlpha.setUniform("texture", sf::Shader::CurrentTexture);



//    states.shader = &your_shader;
    states.blendMode = sf::BlendMode(sf::BlendAlpha);
    states.shader = &shaderAlpha;



//    sf::BlendMode MyBlend(sf::BlendMode::One,sf::BlendMode::One,sf::BlendMode::Add, sf::BlendMode::One,sf::BlendMode::SrcAlpha,sf::BlendMode::Add);
/*
    sf::BlendMode MyBlend(
                          sf::BlendMode::SrcColor,       //colorSrcFactor
                          sf::BlendMode::DstColor,       //colorDstFactor
                          sf::BlendMode::Add,       //colorEquation
                          sf::BlendMode::SrcAlpha,       //alphaSrcFactor
                          sf::BlendMode::OneMinusSrcAlpha,       //alphaDstFactor
                          sf::BlendMode::Subtract);           //alphaEquation
*/

//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    MyBlend.alphaDstFactor=sf::BlendMode::OneMinusSrcAlpha;
//    MyBlend.alphaDstFactor=sf::BlendMode::OneMinusSrcAlpha;
//    MyBlend.alphaSrcFactor = sf::BlendMode::One;
//    MyBlend.alphaSrcFactor = sf::BlendMode::Zero;


    backgroundSprite.setPosition(0,0);
//    renderTexture.clear(sf::Color::Transparent);
    renderTexture.clear(sf::Color(0,0,0,0));
//    renderTexture.draw(backgroundSprite);
//    renderTexture.draw(backgroundSprite,states);
    renderTexture.draw(backgroundSprite,states);
//    renderTexture.draw(backgroundSprite,MyBlend);
    renderTexture.display();


//    sf::Shader picture_shader;
//    picture_states.blendMode=MyBlend;
//    sf::RenderStates picture_states;

//    picture_states.shader = &picture_shader;

    int s_x=backgroundTexture.getSize().x;
    int s_y=backgroundTexture.getSize().y;

    float tt=0.0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Num1) {
                if (!shaderAlpha.loadFromFile( "alpha.frag", sf::Shader::Fragment )) {
                    printf("Error loding shader\n");
                } else {
                    printf("Shader loaded\n");
                }
            //    shaderAlpha.setUniform( "texture", renderBlurY.getTexture());
                shaderAlpha.setUniform("texture", sf::Shader::CurrentTexture);


            }

        }


        float x=x=w_x-s_x/2.0-(w_x/2-s_x/2.0)*sin(tt);
        float y=y=w_y-s_y/2.0-(w_y/2-s_y/2.0)*cos(tt);

        tt+=0.005;

        window.clear(sf::Color::Transparent);

        backgroundSprite.setPosition(x,y);
        window.draw(backgroundSprite);
//        window.draw(backgroundSprite, states);

        x=x=w_x;
        y=y=w_y;
        x=x=w_x+(w_x/2-s_x/2.0)*sin(tt);
        y=y=w_y+(w_y/2-s_y/2.0)*cos(tt);

        renderTextureSprite.setPosition(x,y);
        renderTextureSprite.setRotation(tt*100);

//        window.draw(renderTextureSprite,MyBlend);

//        window.draw(renderTextureSprite, states);
        window.draw(renderTextureSprite);

        window.display();
    }

    return 0;
}


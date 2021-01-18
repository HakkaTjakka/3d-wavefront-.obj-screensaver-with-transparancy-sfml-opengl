#include <SFML/Graphics.hpp>
#include <math.h>
//#include <dwmapi.h>

#if _WIN32_WINNT < 0x0501
    #define _WIN32_WINNT 0x0501
#endif
#include <windows.h>

// Set part of the window that can be clicked by removing fully transparent pixels from the region
void setShape3(HWND hWnd, const sf::Image& image)
{
    const sf::Uint8* pixelData = image.getPixelsPtr();
    HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

    for (unsigned int y = 0; y < image.getSize().y; y++)
    {
        for (unsigned int x = 0; x < image.getSize().x; x++)
        {
            if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0)
            {
                HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
                CombineRgn(hRegion, hRegion, hRegionPixel, RGN_XOR);
                DeleteObject(hRegionPixel);
            }
        }
    }

    SetWindowRgn(hWnd, hRegion, true);
    DeleteObject(hRegion);
}

/*
DWMAPI DwmEnableBlurBehindWindow(
  HWND                 hWnd,
  const DWM_BLURBEHIND *pBlurBehind
);

HRESULT EnableBlurBehind(HWND hwnd)
{
   HRESULT hr = S_OK;

   // Create and populate the Blur Behind structure
   DWM_BLURBEHIND bb = {0};

   // Enable Blur Behind and apply to the entire client area
   bb.dwFlags = DWM_BB_ENABLE;
   bb.fEnable = true;
   bb.hRgnBlur = NULL;

   // Apply Blur Behind
   hr = DwmEnableBlurBehindWindow(hwnd, &bb);
   if (SUCCEEDED(hr))
   {
      // ...
   }
   return hr;
}

*/

int main12()
{
    // Change this if you want to apply transparency on top of the one already in the image
    const sf::Uint8 alpha = 255;

    // Load an image with transparent parts that will be drawn to the screen
    sf::Image image;
    image.loadFromFile("image1920x1080.png");

    // Create the window and center it on the screen
    sf::RenderWindow window(sf::VideoMode(image.getSize().x, image.getSize().y, 32), "Transparent Window", sf::Style::None);
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - image.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - image.getSize().y) / 2));

    HWND hWnd = window.getSystemHandle();

    // Full transparent pixels should not be part of the window
//    DwmEnableBlurBehindWindow(hWnd);


    setShape3(hWnd, image);

    // Create a DC for our bitmap
    HDC hdcWnd = GetDC(hWnd);

    HDC hdc = CreateCompatibleDC(hdcWnd);

    // The window has to be layered if you want transparency
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    // Create our DIB section and select the bitmap into the DC
    void* pvBits;
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = image.getSize().x;
    bmi.bmiHeader.biHeight = image.getSize().y;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = image.getSize().x * image.getSize().y * 4;

    HBITMAP hbitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);

    SelectObject(hdc, hbitmap);

    // Copy the pixels from the image to the bitmap (but pre-multiply the alpha value)
    const sf::Uint8* pixelData = image.getPixelsPtr();
    const int x_s=image.getSize().x;
    const int y_s=image.getSize().y;

    int t=0;

//    y * x_s * 4 + x * 4;
    int a= y_s*x_s - x_s;
    for (unsigned int y = 0; y < y_s; ++y)
    {
        for (unsigned int x = 0; x < x_s; ++x)
        {
//            x + (y_s - y - 1) * x_s
//            x   - y*x_s + a;
            float fAlphaFactor = (float)pixelData[t + 3] / (float)0xff;
  //          float *fApointer=&fAlphaFactor;

//            ((UINT32 *)pvBits)[x - y*x_s + a]

            ((UINT32 *)pvBits)[x - y*x_s + a]
                = (pixelData[t + 3] << 24) |                        //0xaa000000
                 ((UCHAR)(pixelData[t] * fAlphaFactor) << 16) |     //0x00rr0000
                 ((UCHAR)(pixelData[t + 1] * fAlphaFactor) << 8) |  //0x0000gg00
                 ((UCHAR)(pixelData[t + 2]   * fAlphaFactor));      //0x000000bb
//            float fAlphaFactor = (float)pixelData[y * x_s * 4 + x * 4 + 3] / (float)0xff;
//            ((UINT32 *)pvBits)[x + (y_s - y - 1) * x_s]
//                = (pixelData[y * x_s * 4 + x * 4 + 3] << 24) |                        //0xaa000000
//                 ((UCHAR)(pixelData[y * x_s * 4 + x * 4] * fAlphaFactor) << 16) |     //0x00rr0000
//                 ((UCHAR)(pixelData[y * x_s * 4 + x * 4 + 1] * fAlphaFactor) << 8) |  //0x0000gg00
//                 ((UCHAR)(pixelData[y * x_s * 4 + x * 4 + 2]   * fAlphaFactor));      //0x000000bb
            t+=4;
        }
    }

    // Put the image on the screen
    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {(long)image.getSize().x, (long)image.getSize().y};
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = AC_SRC_ALPHA;
    bf.SourceConstantAlpha = alpha;
    UpdateLayeredWindow(hWnd, NULL, NULL, &sizeWnd, hdc, &ptSrc, 0, &bf, ULW_ALPHA);


    // Cleanup
//    DeleteObject(hbitmap);
//    DeleteDC(hdc);
//    DeleteDC(hdcWnd);


    // Main loop that handles the events but does not draw anything
    float x=0.0;
    float y=0.0;
    float tt=0.0;

    sf::Image backgroundImage;

    backgroundImage.loadFromFile("image.png");

//    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window",sf::Style::None);
//    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
//                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    window.setVerticalSyncEnabled(true);
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);

    sf::RenderTexture renderTexture;
    renderTexture.create(backgroundImage.getSize().x, backgroundImage.getSize().y, 32);
    sf::Sprite renderTextureSprite(renderTexture.getTexture());
    renderTextureSprite.setColor({255, 255, 255, 128});

    int w_x=window.getSize().x/2;
    int w_y=window.getSize().y/2;
  while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        x=20*sin(tt);
        y=20*cos(tt);
        tt+=0.05;
//            window.draw(backgroundSprite);

//        UpdateLayeredWindow(hWnd, NULL, NULL, &sizeWnd, hdc, &ptSrc, 0, &bf, ULW_ALPHA);

/*
        x=20*sin(tt);
        y=20*cos(tt);
        tt+=0.05;
        backgroundSprite.setPosition(x,y);
        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(backgroundSprite);
        renderTexture.display();


        window.clear(sf::Color::Transparent);
        UpdateLayeredWindow(hWnd, NULL, NULL, &sizeWnd, hdc, &ptSrc, 0, &bf, ULW_ALPHA);
        window.draw(renderTextureSprite);
        window.display();
*/
    UpdateLayeredWindow(hWnd, NULL, NULL, &sizeWnd, hdc, &ptSrc, 0, &bf, ULW_ALPHA);

        // Don't waste CPU time
        sf::sleep(sf::milliseconds(10));
    }
}


const unsigned char backgroundOpacity = 150;

int main4(int argc, char* argv[])
{
    sf::Image backgroundImage;
    backgroundImage.loadFromFile("image.png");

    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window",sf::Style::None);
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);

    sf::RenderTexture renderTexture;
    renderTexture.create(backgroundImage.getSize().x, backgroundImage.getSize().y, 32);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(backgroundSprite);
        renderTexture.display();

        sf::Sprite renderTextureSprite(renderTexture.getTexture());
        renderTextureSprite.setColor({255, 255, 255, backgroundOpacity});

        window.clear({0, 0, 0, backgroundOpacity});
        window.draw(renderTextureSprite);
        window.display();
    }

    return 0;
}

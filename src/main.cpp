#include <SFML/Graphics.hpp>
#include <optional>
#include <dwmapi.h>
#include <iostream>
#include "lyrics.hpp"

#if defined(SFML_SYSTEM_WINDOWS)
#include <windows.h>

void enableBlurBehind(HWND hwnd) // unsigned char opacity
{
    SetWindowPos(hwnd, HWND_TOPMOST, 100, 100, 400, 600, SWP_SHOWWINDOW);
    HRESULT hr = S_OK;
    DWM_BLURBEHIND bb = {0};
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = true;
    bb.hRgnBlur = NULL;
    DwmEnableBlurBehindWindow(hwnd, &bb);
}
void setTransparency(HWND hWnd, unsigned char alpha)
{
    SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
}

void toggleWindowStyle(HWND hWnd, bool addBorders)
{
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    if (addBorders)
        style |= (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME); // add borders and titles
    else
        style &= ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME); // remove borders and titles
    SetWindowLongPtr(hWnd, GWL_STYLE, style);
    // update window
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
bool isMouseInside(HWND hwnd)
{
    POINT cursorPos;
    RECT windowRect;
    GetCursorPos(&cursorPos);
    GetWindowRect(hwnd, &windowRect);
    return (cursorPos.x >= windowRect.left && cursorPos.x <= windowRect.right &&
            cursorPos.y >= windowRect.top && cursorPos.y <= windowRect.bottom);
}
#else
void enableBlurBehind(Window wnd)
{
}
void setTransparency(Window wnd, unsigned char alpha)
{
}
void toggleWindowStyle(Window wnd, bool addBorders)
{
}
bool isMouseInside(Window wnd)
{
    return false;
}
#endif

void updateLyricsList(sf::RenderWindow &window, std::vector<sf::Text> &lyrics, int currentIndex)
{
    sf::Vector2u winSize = window.getSize();
    float centerY = winSize.y / 2.f;
    // longest string in the lyrics
    float maxWidth = 0.f;
    for (const auto &lyric : lyrics)
    {
        sf::Text tempText(lyric.getFont(), lyric.getString(), 100);
        maxWidth = std::max(maxWidth, tempText.getLocalBounds().size.x);
    }
    // dynamically calculate font size based on the longest string
    int fontSize = static_cast<int>((winSize.x * 0.9f) / maxWidth * 100); // scale to fit 90% of window width
    float lineSpacing = static_cast<float>(fontSize) * 1.5f;              // line spacing is 1.5x font size
    for (size_t i = 0; i < lyrics.size(); ++i)
    {
        lyrics[i].setCharacterSize(fontSize);

        if (i <= currentIndex)
        {
            lyrics[i].setFillColor(sf::Color(255, 255, 255));
        }
        else
        {
            lyrics[i].setFillColor(sf::Color(255, 255, 255, 170));
        }
        lyrics[i].setStyle(sf::Text::Bold);
        // centralize horizontally
        sf::FloatRect bounds = lyrics[i].getLocalBounds();
        lyrics[i].setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        // position vertically based on the current line
        float y;
        if (i < currentIndex)
        {
            y = centerY - (currentIndex - i) * lineSpacing;
        }
        else
        {
            y = centerY + (i - currentIndex) * lineSpacing;
        }
        lyrics[i].setPosition({winSize.x / 2.f, y});
    }
}

std::vector<sf::Text> createLyricsText(const sf::Font &font, int fontSize)
{
    std::vector<sf::Text> lyricsText;
    for (const auto &line : lyricsArray)
    {
        sf::Text text(font, line, fontSize);
        text.setFillColor(sf::Color::White);
        lyricsText.push_back(text);
    }
    return lyricsText;
}

int main()
{
    const unsigned char opacity = 230;
    sf::RenderWindow window(sf::VideoMode({400, 600}), "Lyrics Overlay", sf::Style::None);
    window.setFramerateLimit(144);
    enableBlurBehind(window.getNativeHandle());
    setTransparency(window.getNativeHandle(), opacity);
    int fontSize = 35;

    const sf::Font font("roboto.ttf");
    sf::Text actualMusic(font, "Music Playing: Pink Floyd - Dogs", 15);
    actualMusic.setFillColor(sf::Color::Green);
    actualMusic.setStyle(sf::Text::Bold);
    actualMusic.setPosition({10, 10});

    sf::Clock clock;

    std::vector<sf::Text> lyrics = createLyricsText(font, fontSize);
    int currentIndex = 0;
    updateLyricsList(window, lyrics, currentIndex);

    bool isBorderless = true;
    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::Resized>())
            {
                updateLyricsList(window, lyrics, currentIndex);
            }
            if (isMouseInside(window.getNativeHandle()) && isBorderless)
            {
                isBorderless = false;
                toggleWindowStyle(window.getNativeHandle(), true);
            }
            else if (!isMouseInside(window.getNativeHandle()) && !isBorderless)
            {
                isBorderless = true;
                toggleWindowStyle(window.getNativeHandle(), false);
            }
        }
        window.clear(sf::Color(0, 0, 0, 0));

        window.draw(actualMusic);
        if (currentIndex < lyrics.size())
        {
            if (clock.getElapsedTime().asSeconds() >= 2)
            {
                currentIndex++;
                if (currentIndex < lyrics.size())
                {
                    updateLyricsList(window, lyrics, currentIndex);
                    clock.restart();
                }
            }
        }
        if (currentIndex >= lyrics.size())
        {
            clock.stop();
        }
        for (size_t i = 0; i < lyrics.size(); i++)
        {
            if (lyrics[i].getPosition().y > 100)
            {
                window.draw(lyrics[i]);
            }
        }
        window.display();
    }
}

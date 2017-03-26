#ifndef JC_UTIL
#define JC_UTIL

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

enum _entityCategory {
    HORIZONTAL =    1,
    PLAYER =        2,
    FEET =          3,
    FISH =          8,
    FALL =          16,
    LADDER =        32,
    THORN =         64,
    GOAL =          128,
    VERTICAL =      256,
    SLIP =          512,
    BOULDER =      1024
};

template <typename T>
std::string asString(T number) {
    std::stringstream sstream;
    sstream << number;
    return sstream.str();
}

sf::Vector2f normalize(const sf::Vector2f v) {

    float length = hypot(v.x, v.y);

    if (length == 0)    return sf::Vector2f(0, 0);

    return sf::Vector2f(v.x/length, v.y/length);
}

sf::Vector2f convertb2Vec2(const b2Vec2& vec) {
    return sf::Vector2f(vec.x*40.0f, -vec.y*40.0f);
}

b2Vec2 convertVector2f(const sf::Vector2f& vec) {
    return b2Vec2(vec.x/40.0f, -vec.y/40.0f);
}

sf::Color convertb2Color(const b2Color& color, const float alpha = 255.0f) {
    return sf::Color(color.r*255, color.g*255, color.b*255, alpha);
}

struct Input {

    Input() = default;

    void collect(sf::RenderWindow& window, sf::View& scene_view){

        mouse = sf::Mouse::getPosition(window);
        scene_mouse = window.mapPixelToCoords(mouse, scene_view);

        mmb_delta = 0.0f;
        lmb_released = false;
        rmb_released = false;
        lmb_held = false;
        rmb_held = false;

        keys_released.clear();
        keys_pressed.clear();
        keys_held.clear();

        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseWheelMoved:
                    mmb_delta = event.mouseWheel.delta;
                    break;

                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button==sf::Mouse::Left)          lmb_released = true;
                    else if (event.mouseButton.button==sf::Mouse::Right)    rmb_released = true;
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button==sf::Mouse::Left)          lmb_held = true;
                    else if (event.mouseButton.button==sf::Mouse::Right)    rmb_held = true;
                    break;

                case sf::Event::KeyReleased:
                    keys_released.push_back(event.key.code);
                    break;

                case sf::Event::KeyPressed:
                    keys_pressed.push_back(event.key.code);
                    break;
            }
        }

        for (int key = sf::Keyboard::A; key != sf::Keyboard::KeyCount; ++key) {

            if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key)) keys_held.push_back((sf::Keyboard::Key)key);
        }
    }

    bool keyPressed(const sf::Keyboard::Key& key) const {
        return std::find(keys_pressed.begin(), keys_pressed.end(), key) != keys_pressed.end();
    }

    bool keyHeld(const sf::Keyboard::Key& key) const {
        return std::find(keys_held.begin(), keys_held.end(), key) != keys_held.end();
    }

    bool keyReleased(const sf::Keyboard::Key& key) const {
        return std::find(keys_released.begin(), keys_released.end(), key) != keys_released.end();
    }

    sf::Vector2i mouse;
    sf::Vector2f scene_mouse;

    bool lmb_released;
    bool lmb_held;
    bool rmb_released;
    bool rmb_held;

    float mmb_delta;

    std::vector<sf::Keyboard::Key> keys_released;
    std::vector<sf::Keyboard::Key> keys_held;
    std::vector<sf::Keyboard::Key> keys_pressed;
};

#endif
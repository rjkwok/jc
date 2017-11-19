#ifndef JC_GUI
#define JC_GUI

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#include "util.h"
#include "level.h"

class GUIElement {

public:

    GUIElement() = default;

    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

class GUIRect : public GUIElement {

public:

    GUIRect() = default;
    GUIRect(float x, float y, float width, float height, sf::Color color, float border_size=0.0f) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(color);
    }

    GUIRect(float x, float y, float width, float height, sf::Color color, float border_size, sf::Color border_color) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(border_color);
    }

    virtual void update() {

    };

    virtual void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

protected:

    sf::RectangleShape shape;
};

class GUIWidthBar: public GUIRect {

public:

    GUIWidthBar(float x, float y, float width, float height, sf::Color color, float border_size=0.0f) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(color);

        max_width = width;
    }

    GUIWidthBar(float x, float y, float width, float height, sf::Color color, float border_size, sf::Color border_color) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(border_color);

        max_width = width;
    }

    void setMeasurand(float* measurand) {
        this->measurand = measurand;
    }

    void update() {

        shape.setSize(sf::Vector2f(max_width*(*measurand), shape.getSize().y));
    };

protected:

    float* measurand = nullptr;
    float  max_width;
};


class GUIHeightBar: public GUIRect {

public:

    GUIHeightBar(float x, float y, float width, float height, sf::Color color, float border_size=0.0f) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(color);

        max_height = height;
    }

    GUIHeightBar(float x, float y, float width, float height, sf::Color color, float border_size, sf::Color border_color) {

        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineThickness(border_size);
        shape.setOutlineColor(border_color);

        max_height = height;
    }

    void setMeasurand(float* measurand) {
        this->measurand = measurand;
    }

    void update() {

        shape.setSize(sf::Vector2f(shape.getSize().x, max_height*(*measurand)));
    };

protected:

    float* measurand = nullptr;
    float  max_height;
};

class GUIText: public GUIElement {

public:

    GUIText() = default;
    GUIText(sf::Vector2f p, std::string str, sf::Font* font, sf::Color color, float size=32.0) {

        text.setString(str);
        text.setFont(*font);
        text.setCharacterSize(size);
        text.setColor(color);

        text.setPosition(p);
    }

    virtual void update() {

    }

    virtual void draw(sf::RenderWindow& window) {
        window.draw(text);
    }

protected:

    sf::Text text;
};

class GUICenteredText: public GUIText {

public:

    GUICenteredText(sf::Vector2f p, std::string str, sf::Font* font, sf::Color color, float size=32.0) {

        text.setString(str);
        text.setFont(*font);
        text.setCharacterSize(size);
        text.setColor(color);

        sf::FloatRect rect = text.getLocalBounds();
        text.setOrigin(rect.width/2.0f, rect.height/2.0f);
        text.setPosition(p);
    }
};

class GUI {

public:

    GUI() = default;

    ~GUI() {

        for (auto each: elements) {
            delete each;
        }
    }

    void update(const Input& input, Level& level, const float dt) {

        for (auto each: elements) {
            each->update();
        }
    }

    void draw(sf::RenderWindow& window) {

        for (auto each: elements) {
            each->draw(window);
        }
    }

    void addElement(GUIElement* element) {
        elements.push_back(element);
    }

private:
    ///////////////////////////
    std::vector<GUIElement*> elements;

};

#endif
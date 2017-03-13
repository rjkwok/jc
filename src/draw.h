#ifndef JC_B2DRAW
#define JC_B2DRAW

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

class b2DebugDraw : public b2Draw
{

public:

    b2DebugDraw(sf::RenderWindow& window) : window(window) { }

    /// Draw a closed polygon provided in CCW order.
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {

        sf::ConvexShape shape(vertexCount);

        for (int index = 0; index < vertexCount; ++index) {
            shape.setPoint(index, convertb2Vec2(vertices[index]));
        }

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 0.0f));

        window.draw(shape);
    }

    /// Draw a solid closed polygon provided in CCW order.
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {

        sf::ConvexShape shape(vertexCount);

        for (int index = 0; index < vertexCount; ++index) {
            shape.setPoint(index, convertb2Vec2(vertices[index]));
        }

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 155.0f));

        window.draw(shape);
    }

    /// Draw a circle.
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override {

        sf::CircleShape shape(40.0f*radius);

        shape.setOrigin(40.0f*radius, 40.0f*radius);
        shape.setPosition(convertb2Vec2(center));

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 0.0f));

        window.draw(shape);
    }

    /// Draw a solid circle.
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override {

        sf::CircleShape shape(40.0f*radius);

        shape.setOrigin(40.0f*radius, 40.0f*radius);
        shape.setPosition(convertb2Vec2(center));

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 155.0f));

        window.draw(shape);
    }

    /// Draw a line segment.
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {

        sf::RectangleShape shape(convertb2Vec2(b2Vec2((p2 - p1).Length(), 0.03f)));

        shape.setOrigin(0.0f, 1.5f);
        shape.setPosition(convertb2Vec2(p1));

        shape.setRotation(atan2(p1.y - p2.y, p2.x - p1.x) * 180.0/3.14159);

        shape.setOutlineThickness(0);
        shape.setFillColor(convertb2Color(color, 200.0f));

        window.draw(shape);
    }

    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    void DrawTransform(const b2Transform& xf) override {

    }

    /// Draw a point.
    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override {

        sf::CircleShape shape(size);

        shape.setOrigin(size, size);
        shape.setPosition(convertb2Vec2(p));

        shape.setOutlineThickness(0);
        shape.setFillColor(convertb2Color(color, 200.0f));

        window.draw(shape);
    }

private:

    ///////////////////////////////////////////////////

    sf::RenderWindow& window;
};

#endif
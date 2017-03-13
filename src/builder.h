#ifndef JC_BUILDER
#define JC_BUILDER

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#include "util.h"
#include "level.h"

class Builder {

public:

    Builder() = default;

    virtual void update(const Input& input, Level* level) {

    }

    virtual void draw(sf::RenderWindow& window) {

    }
};

class EdgeBuilder: public Builder {

public:

    EdgeBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            constructionPoints.push_back(gridSnappedPoint);
        }

        if (input.lmb_released) {
            constructionPoints.clear();
        }

        if (constructionPoints.size() == 1) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(gridSnappedPoint.y - constructionPoints[0].y, gridSnappedPoint.x - constructionPoints[0].x)*180.0f/3.14159f);
        }

        if (constructionPoints.size() == 2) {

            if (abs(constructionPoints[1].x - constructionPoints[0].x) > 0.01f && abs(constructionPoints[1].y - constructionPoints[0].y) > 0.01f) {
                level->edges.push_back(Edge(*level->world, constructionPoints[0], constructionPoints[1], SLIP));
            }
            if (abs(constructionPoints[1].x - constructionPoints[0].x) > 0.01f) {
                level->edges.push_back(Edge(*level->world, constructionPoints[0], constructionPoints[1], HORIZONTAL));
            }
            else {
                level->edges.push_back(Edge(*level->world, constructionPoints[0], constructionPoints[1], VERTICAL));
            }
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);

        if (constructionPoints.size() == 1) window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class PinnedEdgeBuilder: public Builder {

public:

    PinnedEdgeBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            constructionPoints.push_back(gridSnappedPoint);
        }

        if (input.lmb_released) {
            constructionPoints.clear();
        }

        if (constructionPoints.size() == 1) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(gridSnappedPoint.y - constructionPoints[0].y, gridSnappedPoint.x - constructionPoints[0].x)*180.0f/3.14159f);
        }

        if (constructionPoints.size() == 2) {

            level->pinnedEdges.push_back(PinnedEdge(*level->world, constructionPoints[0], constructionPoints[1]));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);

        if (constructionPoints.size() == 1) window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class SwingBuilder: public Builder {

public:

    SwingBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            constructionPoints.push_back(gridSnappedPoint);
        }

        if (input.lmb_released) {
            constructionPoints.clear();
        }

        if (constructionPoints.size() == 1) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(gridSnappedPoint.y - constructionPoints[0].y, gridSnappedPoint.x - constructionPoints[0].x)*180.0f/3.14159f);
        }

        if (constructionPoints.size() == 2 || constructionPoints.size() == 3) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((constructionPoints[1] - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((constructionPoints[1] - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(constructionPoints[1].y - constructionPoints[0].y, constructionPoints[1].x - constructionPoints[0].x)*180.0f/3.14159f);
        }

        if (constructionPoints.size() == 4) {

            level->swings.push_back(Swing(*level->world, constructionPoints[0], constructionPoints[1], constructionPoints[2], constructionPoints[3]));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);

        if (constructionPoints.size() >= 1) window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class FallingEdgeBuilder: public Builder {

public:

    FallingEdgeBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            constructionPoints.push_back(gridSnappedPoint);
        }

        if (input.lmb_released) {
            constructionPoints.clear();
        }

        if (constructionPoints.size() == 1) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(gridSnappedPoint.y - constructionPoints[0].y, gridSnappedPoint.x - constructionPoints[0].x)*180.0f/3.14159f);
        }

        if (constructionPoints.size() == 2) {
            level->edges.push_back(Edge(*level->world, constructionPoints[0], constructionPoints[1], FALL));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);

        if (constructionPoints.size() == 1) window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class LadderBuilder: public Builder {

public:

    LadderBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
        shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.0f)));
        shape.setPoint(1, convertb2Vec2(b2Vec2(0.0f, 4.0f)));
        shape.setPoint(2, convertb2Vec2(b2Vec2(2.0f, 4.0f)));
        shape.setPoint(3, convertb2Vec2(b2Vec2(2.0f, 0.0f)));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));
        shape.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            level->ladders.push_back(Ladder(*level->world, gridSnappedPoint));
        }

    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
        window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class BouncerBuilder: public Builder {

public:

    BouncerBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
        shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.0f)));
        shape.setPoint(1, convertb2Vec2(b2Vec2(0.0f, 3.0f)));
        shape.setPoint(2, convertb2Vec2(b2Vec2(6.0f, 3.0f)));
        shape.setPoint(3, convertb2Vec2(b2Vec2(6.0f, 0.0f)));

        /*circleShape.setOutlineThickness(3);
        circleShape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        circleShape.setFillColor(sf::Color(55, 255, 55, 155.0f));
        circleShape.setRadius(40.0f*3.0f);
        circleShape.setOrigin(40.0f*3.0f, 40.0f*3.0f);*/
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));
        shape.setPosition(convertb2Vec2(gridSnappedPoint));
       // circleShape.setPosition(convertb2Vec2(gridSnappedPoint + b2Vec2(3.0f, 3.0f)));

        if (input.rmb_released) {
            level->bouncers.push_back(Bouncer(*level->world, gridSnappedPoint));
        }

    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
        window.draw(shape);
       // window.draw(circleShape);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
   // sf::CircleShape circleShape;
};

class ThornBuilder: public Builder {

public:

    ThornBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(3);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.keyReleased(sf::Keyboard::Up))    height += 1.0f;
        if (input.keyReleased(sf::Keyboard::Down))  height -= 1.0f;
        if (input.keyReleased(sf::Keyboard::Left))  angle += 1.5708f;
        if (input.keyReleased(sf::Keyboard::Right)) angle -= 1.5708f;

        shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.0f)));
        shape.setPoint(1, convertb2Vec2(b2Vec2(1.0f, height)));
        shape.setPoint(2, convertb2Vec2(b2Vec2(2.0f, 0.0f)));
        shape.setPosition(convertb2Vec2(gridSnappedPoint));
        shape.setRotation(-angle*180.0f/3.14159);

        if (input.rmb_released) {
            level->thorns.push_back(Thorn(*level->world, gridSnappedPoint, height, angle));
        }

    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
        window.draw(shape);
    }

    b2Vec2 gridSnappedPoint;
    float angle = 0.0f;
    float height = 2.0f;
    sf::CircleShape cursor;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
};

class BoulderSpawnBuilder: public Builder {

public:

    BoulderSpawnBuilder() {

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                mGrid.append(sf::Vertex(convertb2Vec2(b2Vec2(2.0f*i, 2.0f*j)), sf::Color(255, 255, 255, 255)));
            }
        }

        cursor = sf::CircleShape(20.0f);
        cursor.setOrigin(20.0f, 20.0f);
        cursor.setOutlineThickness(3);
        cursor.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        cursor.setFillColor(sf::Color(255, 255, 255, 0.0f));

        shape = sf::ConvexShape(4);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        shape.setFillColor(sf::Color(55, 255, 55, 155.0f));

        boulder.setOutlineThickness(3);
        boulder.setOutlineColor(sf::Color(55, 255, 55, 200.0f));
        boulder.setFillColor(sf::Color(55, 255, 55, 155.0f));
    }

    void update(const Input& input, Level* level) override {

        b2Vec2 selectedPoint = convertVector2f(input.scene_mouse);

        /*
            a     b
              [ ]
            c     d
        */

        b2Vec2 a = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 b = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, ceil(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 c = b2Vec2(floor(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);
        b2Vec2 d = b2Vec2(ceil(selectedPoint.x/2.0f)*2.0f, floor(selectedPoint.y/2.0f)*2.0f);

        if ((a - selectedPoint).Length() < (b - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (c - selectedPoint).Length() &&
            (a - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = a;
        }
        else if ((b - selectedPoint).Length() < (c - selectedPoint).Length() &&
                 (b - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = b;
        }
        else if ((c - selectedPoint).Length() < (d - selectedPoint).Length())
        {
            gridSnappedPoint = c;
        }
        else
        {
            gridSnappedPoint = d;
        }

        cursor.setPosition(convertb2Vec2(gridSnappedPoint));

        if (input.rmb_released) {
            constructionPoints.push_back(gridSnappedPoint);
        }

        if (input.lmb_released) {
            constructionPoints.clear();
        }

        if (input.keyReleased(sf::Keyboard::Up))    radius += 1.0f;
        if (input.keyReleased(sf::Keyboard::Down))  radius -= 1.0f;

        if (input.keyReleased(sf::Keyboard::Left))  bouldersPerSecond -= 1.0f;
        if (input.keyReleased(sf::Keyboard::Right)) bouldersPerSecond += 1.0f;

        if (constructionPoints.size() == 1) {

            shape.setPoint(0, convertb2Vec2(b2Vec2(0.0f, 0.3f/2.0f)));
            shape.setPoint(1, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), 0.3f/2.0f)));
            shape.setPoint(2, convertb2Vec2(b2Vec2((gridSnappedPoint - constructionPoints[0]).Length(), -0.3f/2.0f)));
            shape.setPoint(3, convertb2Vec2(b2Vec2(0.0f, -0.3f/2.0f)));
            shape.setPosition(convertb2Vec2(constructionPoints[0]));
            shape.setRotation(-atan2(gridSnappedPoint.y - constructionPoints[0].y, gridSnappedPoint.x - constructionPoints[0].x)*180.0f/3.14159f);

            boulder.setRadius(radius*40.0f);
            boulder.setOrigin(radius*40.0f, radius*40.0f);
            boulder.setPosition(convertb2Vec2(constructionPoints[0]));
        }

        if (constructionPoints.size() == 2) {

            level->boulderSpawns.push_back(BoulderSpawn(*level->world, constructionPoints[0], constructionPoints[1], radius, bouldersPerSecond));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);

        if (constructionPoints.size() == 1) {
            window.draw(shape);
            window.draw(boulder);
        }
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
    sf::ConvexShape shape;
    sf::CircleShape boulder;
    float radius = 1.0f;
    float bouldersPerSecond = 0.2f;
};

#endif
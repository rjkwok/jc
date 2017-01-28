#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

enum _entityCategory {
    ENTITY =    0x0001,
    PLAYER =    0x0002,
    FEET =      0x0004,
    FISH =      0x0008,
    FALL =      0x0010,
    LADDER =    0x0020
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

        sf::CircleShape shape(radius);

        shape.setOrigin(radius, radius);
        shape.setPosition(convertb2Vec2(center));

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 0.0f));

        window.draw(shape);
    }

    /// Draw a solid circle.
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override {

        sf::CircleShape shape(radius);

        shape.setOrigin(radius, radius);
        shape.setPosition(convertb2Vec2(center));

        shape.setOutlineThickness(3);
        shape.setOutlineColor(convertb2Color(color, 200.0f));
        shape.setFillColor(convertb2Color(color, 155.0f));

        window.draw(shape);
    }

    /// Draw a line segment.
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {

        sf::RectangleShape shape(sf::Vector2f((p2 - p1).Length(), 3.0f));

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

class Camera {

public:

    Camera(const sf::View& view) {

        mView = view;
        mScale = 1.0f;
        mWidth = view.getSize().x;
        mHeight = view.getSize().y;
    }

    void setPixelsPerSecondSquared(const float a) {
        mA = a;
    }

    void zoom(const float scaling) {

        mScale += scaling;

        if (mScale < 0.1f)      mScale = 0.1f;
        else if (mScale > 8.0f) mScale = 8.0f;
    }

    void track(const sf::Vector2f& point) {

        mTrackingPoint = point;
    }

    void update(const float dt) {

        sf::Vector2f a = normalize(mTrackingPoint - (mView.getCenter() + mV*0.5f))*mA;

        mV = mV + (a*dt);

        mView.setSize(mWidth*mScale, mHeight*mScale);
        mView.setCenter(mView.getCenter() + (mV*dt));
    }

    float mScale;
    float mWidth;
    float mHeight;
    sf::View mView;

private:

    sf::Vector2f mTrackingPoint;
    float mA;
    sf::Vector2f mV;

};

class TexturedBody {

public:

    TexturedBody() = default;
    TexturedBody(b2Body* body) : mBody(body) { }

    b2Body* mBody;

protected:
    sf::Sprite sprite;
};

class EditableShape {

public:

    EditableShape() = default;

    ~EditableShape() {

        for (auto ptr : points) {
            delete ptr;
        }
    }

    virtual void generateBody(b2World& world, bool fall=false) = 0;

    virtual json getJSON() = 0;
    /////////////////////////////////////////////

    std::vector<b2Vec2*> points;

protected:

    b2Body* body = nullptr;
};

class Edge {

public:

    Edge() = default;

    Edge(b2World& world, const b2Vec2& start, const b2Vec2& end, const unsigned short bitmask)
        : start(start)
        , end(end)
        , bitmask(bitmask)
    {
        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.fixedRotation = true;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = bitmask;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);
    }

    Edge(b2World& world, const json& edge_data) {

        start = b2Vec2(edge_data["start"]["x"], edge_data["start"]["y"]);
        end = b2Vec2(edge_data["end"]["x"], edge_data["end"]["y"]);
        bitmask = edge_data["mask"];

        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.fixedRotation = true;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = bitmask;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);
    }

    json getJSON() {
        json edge_data;
        edge_data["start"] = { { "x", start.x }, { "y", start.y } };
        edge_data["end"] = { { "x", end.x }, { "y", end.y } };
        edge_data["mask"] = bitmask;
        return edge_data;
    }

    b2Vec2 start;
    b2Vec2 end;
    unsigned short bitmask;
};

class Ladder {

public:

    Ladder() = default;

    Ladder(b2World& world, const b2Vec2 pos) : pos(pos) {

        b2BodyDef bodyDef;
        bodyDef.position = pos;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 4.0f), b2Vec2(2.0f, 4.0f), b2Vec2(2.0f, 0.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = LADDER;
        fixtureDef.isSensor = true;
        fixtureDef.density = 1.0f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);
    }

    Ladder(b2World&world, const json& ladder_data) {

        pos = b2Vec2(ladder_data["pos"]["x"], ladder_data["pos"]["y"]);

        b2BodyDef bodyDef;
        bodyDef.position = pos;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 4.0f), b2Vec2(2.0f, 4.0f), b2Vec2(2.0f, 0.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = LADDER;
        fixtureDef.isSensor = true;
        fixtureDef.density = 1.0f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);
    }

    json getJSON() {
        json ladder_data;
        ladder_data["pos"] = { { "x", pos.x }, { "y", pos.y } };
        return ladder_data;
    }

    b2Vec2 pos;
};

class Level {

public:

    Level() : world(b2Vec2(0.0f, -90.81f)) {

    }

    ~Level() {

        for (auto ptr : texturedBodies) {
            delete ptr;
        }
    }

    void loadFromFile(const std::string& path) {

        // Load
        json levelData;

        std::ifstream file(path);

        if (!file.good())   return;

        file >> levelData;
        file.close();

        for (auto each: levelData["edges"]) {
            edges.push_back(Edge(world, each));
        }

        for (auto each: levelData["ladders"]) {
            ladders.push_back(Ladder(world, each));
        }
    }

    void dumpToFile(const std::string& path) {

        // Save terrain
        json levelData;
        levelData["edges"] = json::array();
        for (auto edge: edges) {

            levelData["edges"].push_back(edge.getJSON());
        }
        for (auto ladder: ladders) {

            levelData["ladders"].push_back(ladder.getJSON());
        }

        std::ofstream file(path);
        file << std::setw(4) << levelData << std::endl;
        file.close();
    }

    // Terrain is stored as a collection of thin static rectangles defined by point pairs

    std::vector<Edge> edges;
    std::vector<Ladder> ladders;
    std::vector<TexturedBody*> texturedBodies;
    b2World world;
};

class Builder {

public:

    Builder() = default;

    virtual void update(const Input& input, Level* level, sf::RenderWindow& window, sf::View& windowView) {

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
    }

    void update(const Input& input, Level* level, sf::RenderWindow& window, sf::View& windowView) override {

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

        if (constructionPoints.size() == 2) {
            level->edges.push_back(Edge(level->world, constructionPoints[0], constructionPoints[1], ENTITY));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
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
    }

    void update(const Input& input, Level* level, sf::RenderWindow& window, sf::View& windowView) override {

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

        if (constructionPoints.size() == 2) {
            level->edges.push_back(Edge(level->world, constructionPoints[0], constructionPoints[1], FALL));
            constructionPoints.clear();
        }
    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    std::vector<b2Vec2> constructionPoints;
    sf::VertexArray mGrid;
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
    }

    void update(const Input& input, Level* level, sf::RenderWindow& window, sf::View& windowView) override {

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
            level->ladders.push_back(Ladder(level->world, gridSnappedPoint));
        }

    }

    void draw(sf::RenderWindow& window) {

        window.draw(mGrid);
        window.draw(cursor);
    }

    b2Vec2 gridSnappedPoint;
    sf::CircleShape cursor;
    sf::VertexArray mGrid;
};

class Player : public TexturedBody {

public:

    Player(Level& level) {

        // Define main player bounds
        b2BodyDef boxBodyDef;
        boxBodyDef.position = b2Vec2(0.0f, 50.0f);
        boxBodyDef.fixedRotation = true;
        boxBodyDef.type = b2_dynamicBody;

        mBody = level.world.CreateBody(&boxBodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox(1.0f, 1.0f);

        b2FixtureDef boxFixtureDef;
        boxFixtureDef.filter.categoryBits = PLAYER;
        boxFixtureDef.shape = &boxShape;
        boxFixtureDef.friction = 0.0f;
        boxFixtureDef.density = 1.0f;

        mBody->CreateFixture(&boxFixtureDef)->SetUserData((void*)this);

        // Define "foot" sensor for jumpability

        b2PolygonShape sensorShape;
        sensorShape.SetAsBox(1.0f, 0.25f, b2Vec2(0.0f, -1.0f), 0.0f);

        b2FixtureDef sensorFixtureDef;
        sensorFixtureDef.filter.categoryBits = FEET;
        sensorFixtureDef.shape = &sensorShape;
        sensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&sensorFixtureDef)->SetUserData((void*)this);
    }

    void update(const Input& input, Level& level) {

        if (input.keyPressed(sf::Keyboard::Space)) {

            if (touchingGround()) {
                mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 30.0f));
            }
            else if (mExtraJumps > 0) {
                mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 30.0f));
                --mExtraJumps;
            }
        }

        if (touchingGround())   mExtraJumps = 1;

        float axis = 0.0f;

        if (input.keyHeld(sf::Keyboard::D)) axis += 1.0f;
        if (input.keyHeld(sf::Keyboard::A)) axis -= 1.0f;

        if (axis != 0 || touchingGround()) {
            if (input.keyHeld(sf::Keyboard::LShift)) {
                mBody->SetLinearVelocity(b2Vec2(axis*30.0f, mBody->GetLinearVelocity().y));
            }
            else {
                mBody->SetLinearVelocity(b2Vec2(axis*15.0f, mBody->GetLinearVelocity().y));
            }
        }

        if (touchingLadder()) {
            if (input.keyHeld(sf::Keyboard::W)) {
                mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 20.0f));
            }
        }

        if (axis > 0)   mFacingRight = true;
        if (axis < 0)   mFacingRight = false;

        if (input.keyPressed(sf::Keyboard::Return)) fireProjectile(level);
    }

    void fireProjectile(Level& level) {

        // Spawn a fish and flip it through the air

        b2BodyDef fishDef;

        if (mFacingRight) {
            fishDef.position = mBody->GetPosition() + b2Vec2(1.2f, 0.3f);
            fishDef.linearVelocity = mBody->GetLinearVelocity() + b2Vec2(15.0f, 5.0f);
            fishDef.angularVelocity = -7.0f;
        }
        else {
            fishDef.position = mBody->GetPosition() + b2Vec2(-1.2f, 0.3f);
            fishDef.linearVelocity = mBody->GetLinearVelocity() + b2Vec2(-15.0f, 5.0f);
            fishDef.angularVelocity = 7.0f;
        }

        fishDef.type = b2_dynamicBody;

        b2PolygonShape fishShape;
        fishShape.SetAsBox(0.4f, 0.2f);

        b2FixtureDef fishFixtureDef;
        fishFixtureDef.filter.categoryBits = FISH;
        fishFixtureDef.shape = &fishShape;
        fishFixtureDef.friction = 1.0f;
        fishFixtureDef.density = 1.0f;

        b2Body* fishBody = level.world.CreateBody(&fishDef);

        TexturedBody* texturedBody = new TexturedBody(fishBody);

        fishBody->CreateFixture(&fishFixtureDef)->SetUserData((void*)texturedBody);
        level.texturedBodies.push_back(texturedBody);
    }

    b2Vec2 getPosition() const {

        return mBody->GetPosition();
    }

    bool touchingGround() {

        return mFootContactCount > 0;
    }

    bool touchingLadder() {

        return mLadderContactCount > 0;
    }

    int mFootContactCount = 0;
    int mLadderContactCount = 0;

private:
    ///////////////////////////

    bool mFacingRight = true;
    int mExtraJumps = 0;
};

class MyContactListener : public b2ContactListener {

    void BeginContact(b2Contact* contact) {

        beginContactEvents.push_back(std::pair<b2Fixture*, b2Fixture*>(contact->GetFixtureA(), contact->GetFixtureB()));

        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LADDER && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LADDER && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount++;
        }
    }

    void EndContact(b2Contact* contact) {

        endContactEvents.push_back(std::pair<b2Fixture*, b2Fixture*>(contact->GetFixtureA(), contact->GetFixtureB()));

        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LADDER && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LADDER && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount--;
        }
    }

public:

    std::vector<std::pair<b2Fixture*, b2Fixture*> > beginContactEvents;
    std::vector<std::pair<b2Fixture*, b2Fixture*> > endContactEvents;

    Level* level;
};

int main() {

    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Debug", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Debug");
    sf::View view = window.getView();
    sf::View window_view = window.getView();

    // Create the world

    Level level;

    std::vector<Builder*> builders;
    builders.push_back(new Builder());
    builders.push_back(new EdgeBuilder());
    builders.push_back(new FallingEdgeBuilder());
    builders.push_back(new LadderBuilder());
    auto builder = builders.begin();

    Player player(level);

    MyContactListener myContactListener;
    myContactListener.level = &level;
    level.world.SetContactListener(&myContactListener);

    level.loadFromFile("level.json");
    //

    Camera camera(view);
    camera.setPixelsPerSecondSquared(8000.0f);

    b2DebugDraw debugDraw(window);
    debugDraw.SetFlags( b2Draw::e_shapeBit );
    level.world.SetDebugDraw(&debugDraw);

    Input input;

    int32 positionIterations = 2;
    int32 velocityIterations = 6;

    //initialize loop timer
    sf::Clock timer;
    double dt = 0;
    //

    while (window.isOpen()) {

        //loop header
        window.clear();
        dt = timer.getElapsedTime().asSeconds();

        if (dt == 0)    dt = 0.000000001;

        timer.restart();
        //

        camera.track(convertb2Vec2(player.getPosition()));
        camera.zoom(-input.mmb_delta);
        camera.update(dt);
        window.setView(camera.mView);

        input.collect(window, camera.mView);

        if (input.keyReleased(sf::Keyboard::Escape)) {
            window.close();
        }

        if (input.keyReleased(sf::Keyboard::Tab)) {
            builder++;
            if (builder == builders.end())  builder = builders.begin();
        }

        (*builder)->update(input, &level, window, window_view);

        player.update(input, level);

        level.world.Step(dt, velocityIterations, positionIterations);

        for (auto pair: myContactListener.beginContactEvents) {
            if (pair.first->GetFilterData().categoryBits == FALL) {
                pair.first->GetBody()->SetType(b2_dynamicBody);
            }
            if (pair.second->GetFilterData().categoryBits == FALL) {
                pair.second->GetBody()->SetType(b2_dynamicBody);
            }
            if (pair.first->GetFilterData().categoryBits == FISH && pair.second->GetFilterData().categoryBits & (ENTITY | FALL)) {
                level.world.DestroyBody(pair.first->GetBody());
            }
            if (pair.second->GetFilterData().categoryBits == FISH && pair.first->GetFilterData().categoryBits & (ENTITY | FALL)) {
               level.world.DestroyBody(pair.second->GetBody());
            }
        }
        myContactListener.beginContactEvents.clear();
        myContactListener.endContactEvents.clear();


        window.clear();
        (*builder)->draw(window);
        level.world.DrawDebugData();
        window.display();
    }

    for (auto ptr: builders) {
        delete ptr;
    }

    level.dumpToFile("level.json");
    return 0;
}
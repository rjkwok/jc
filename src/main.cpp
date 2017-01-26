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
    FISH =      0x0008
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

    Camera(const sf::View& view) : mView(view) { }

    void setPixelsPerSecondSquared(const float a) {
        mA = a;
    }

    void track(const sf::Vector2f& point) {

        mTrackingPoint = point;
    }

    void update(const float dt) {

        sf::Vector2f a = normalize(mTrackingPoint - (mView.getCenter() + mV*0.5f))*mA;

        mV = mV + (a*dt);

        mView.setCenter(mView.getCenter() + (mV*dt));
    }

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

    virtual void generateBody(b2World& world) = 0;

    virtual json getJSON() = 0;
    /////////////////////////////////////////////

    std::vector<b2Vec2*> points;

protected:

    b2Body* body = nullptr; 
};

class EditableEdge : public EditableShape {

public:

    EditableEdge() = default;
    
    EditableEdge(const b2Vec2& start, const b2Vec2& end) { 

        points.push_back(new b2Vec2(start));
        points.push_back(new b2Vec2(end));
    }

    EditableEdge(const json& edge_data) {

        points.push_back(new b2Vec2(edge_data["start"]["x"], edge_data["start"]["y"]));
        points.push_back(new b2Vec2(edge_data["end"]["x"], edge_data["end"]["y"]));
    }

    virtual void generateBody(b2World& world) override {

        if (body)   world.DestroyBody(body);

        b2BodyDef bodyDef;
        bodyDef.position = *points[0];
        bodyDef.angle = atan2(points[1]->y - points[0]->y, points[1]->x - points[0]->x);

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2(points[1]->x - points[0]->x, thickness/2.0f), b2Vec2(points[1]->x - points[0]->x, -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&rectangle, 0.0f); 
    }

    virtual json getJSON() override {
        json edge_data;
        edge_data["start"] = { { "x", points[0]->x }, { "y", points[0]->y } };
        edge_data["end"] = { { "x", points[1]->x }, { "y", points[1]->y } };
        return edge_data;
    }
};

class Level {

public:

    Level() : world(b2Vec2(0.0f, -9.81f)) {

    }

    ~Level() {

        for (auto ptr : texturedBodies) {
            delete ptr;
        }
        for (auto ptr : edges) {
            delete ptr;
        }
    }

    void editTerrain(const Input& input, sf::RenderWindow& window, sf::View& windowView) {

        float maxDistance = 20.0f;

        float shortestDistance = -1.0f;
        EditableEdge* closestEdge = nullptr;
        b2Vec2* closestPoint = nullptr;

        for (auto edge : edges) {
            for (auto point : edge->points) {

                sf::Vector2i windowPoint = window.mapCoordsToPixel(convertb2Vec2(*point), windowView);
                float distance = hypot(input.mouse.x - windowPoint.x, input.mouse.y - windowPoint.y);

                if (distance <= maxDistance && (shortestDistance < 0 || distance < shortestDistance)) {
                    shortestDistance = distance;
                    closestEdge = edge;
                    closestPoint = point;
                }
            }
        }

        if (input.lmb_released) {

            if (selectedShape) {
                selectedShape->generateBody(world);
                selectedShape = nullptr;
                selectedPoint = nullptr;
            }
            else {

                // select the closest point if within tolerance
                if (closestPoint) {
                    selectedShape = closestEdge;
                    selectedPoint = closestPoint;
                }
            }
        }

        if (selectedPoint) {
            *selectedPoint = convertVector2f(input.scene_mouse);
        }

        if (input.rmb_released) {
            constructionPoints.push_back(convertVector2f(input.scene_mouse));
        }

        if (constructionPoints.size() == 2) {
            edges.push_back(new EditableEdge(constructionPoints[0], constructionPoints[1]));
            edges[edges.size() - 1]->generateBody(world);
            constructionPoints.clear();
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
            edges.push_back(new EditableEdge(b2Vec2(each["start"]["x"], each["start"]["y"]), b2Vec2(each["end"]["x"], each["end"]["y"])));
            edges[edges.size() - 1]->generateBody(world);
        }
    }

    void dumpToFile(const std::string& path) {

        // Save terrain
        json levelData;
        levelData["edges"] = json::array();
        for (auto edge: edges) {

            levelData["edges"].push_back(edge->getJSON());
        }

        std::ofstream file(path);
        file << std::setw(4) << levelData << std::endl;
        file.close();
    }

    // Terrain is stored as a collection of thin static rectangles defined by point pairs
    // Each point can be dragged around in the editor and the rectangle will be re-generated

    std::vector<b2Vec2> constructionPoints;

    b2Vec2* selectedPoint = nullptr;
    EditableEdge* selectedShape = nullptr;

    std::vector<EditableEdge*> edges;
    std::vector<TexturedBody*> texturedBodies;
    std::vector<TexturedBody*> queuedForRemoval;
    b2World world;
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
        boxFixtureDef.friction = 0.7f;
        boxFixtureDef.density = 1.0f;
       
        mBody->CreateFixture(&boxFixtureDef);

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

        if (input.keyPressed(sf::Keyboard::Space) && canJump()) {

            mBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 50.0f), true);
        }

        float axis = 0.0f;

        if (input.keyHeld(sf::Keyboard::D)) axis += 1.0f;
        if (input.keyHeld(sf::Keyboard::A)) axis -= 1.0f;

        if (axis != 0 || canJump()) mBody->SetLinearVelocity(b2Vec2(axis*8.0f, mBody->GetLinearVelocity().y));

        if (axis > 0)   mFacingRight = true;
        if (axis < 0)   mFacingRight = false;

        if (input.keyHeld(sf::Keyboard::S)) mBody->ApplyForceToCenter(b2Vec2(0.0f, -500.0f), true);

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

    bool canJump() {

        return mFootContactCount > 0;
    }

    int mFootContactCount = 0;

private:
    ///////////////////////////

    bool mFacingRight = true;
};

class MyContactListener : public b2ContactListener {

    void BeginContact(b2Contact* contact) {

        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == FISH && contact->GetFixtureB()->GetFilterData().categoryBits == ENTITY) {
            //contact->GetFixtureA()->GetBody()->SetActive(false);
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    level->queuedForRemoval.push_back(static_cast<TexturedBody*>(fixtureUserData));
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FISH && contact->GetFixtureA()->GetFilterData().categoryBits == ENTITY) {
            //contact->GetFixtureB()->GetBody()->SetActive(false);
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    level->queuedForRemoval.push_back(static_cast<TexturedBody*>(fixtureUserData));
        }
    }

    void EndContact(b2Contact* contact) {
        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
    }

public:

    Level* level;

};

int main() {

    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Debug", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Debug");
    sf::View view = window.getView();
    sf::View window_view = window.getView();

    // Create the world

    Level level;

    b2BodyDef groundBodyDef;
    groundBodyDef.position = b2Vec2(0.0f, 0.0f);

    b2PolygonShape groundShape;
    groundShape.SetAsBox(50.0f, 1.0f);

    level.world.CreateBody(&groundBodyDef)->CreateFixture(&groundShape, 1.0f);

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
        camera.update(dt);
        window.setView(camera.mView);

        input.collect(window, camera.mView);

        if (input.keyReleased(sf::Keyboard::Escape)) {
            window.close();
        }

        level.editTerrain(input, window, window_view);

        player.update(input, level);

        level.world.Step(dt, velocityIterations, positionIterations);

        for (auto ptr: level.queuedForRemoval) {

            level.world.DestroyBody(ptr->mBody);
        }
        level.queuedForRemoval.clear();

        window.clear();
        level.world.DrawDebugData();
        window.display();
    }

    level.dumpToFile("level.json");
    return 0;
}
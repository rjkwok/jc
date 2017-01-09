#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#include <iostream>

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

    sf::View getView() {
        return mView;
    }

private:

    sf::Vector2f mTrackingPoint;
    float mA;
    sf::Vector2f mV;
    sf::View mView;
};

class Player {

public:

    Player(b2World& world) {

        // Define main player bounds
        b2BodyDef boxBodyDef;
        boxBodyDef.position = b2Vec2(0.0f, 50.0f);
        boxBodyDef.type = b2_dynamicBody;

        mBody = world.CreateBody(&boxBodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox(1.0f, 1.0f);

        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape = &boxShape;
        boxFixtureDef.friction = 0.7f;
        boxFixtureDef.density = 1.0f;
       
        mBody->CreateFixture(&boxFixtureDef);

        // Define "foot" sensor for jumpability

        b2PolygonShape sensorShape;
        sensorShape.SetAsBox(1.0f, 0.25f, b2Vec2(0.0f, -1.0f), 0.0f);

        b2FixtureDef sensorFixtureDef;
        sensorFixtureDef.shape = &sensorShape;
        sensorFixtureDef.isSensor = true;
        
        mBody->CreateFixture(&sensorFixtureDef)->SetUserData((void*)this);
    }

    void update(const Input& input) {

        if (input.keyPressed(sf::Keyboard::Space) && canJump()) {

            mBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 50.0f), true);
        }

        float axis = 0.0f;

        if (input.keyHeld(sf::Keyboard::D)) axis += 1.0f;
        if (input.keyHeld(sf::Keyboard::A)) axis -= 1.0f;

        if (axis != 0 || canJump()) mBody->SetLinearVelocity(b2Vec2(axis*14.0f, mBody->GetLinearVelocity().y));

        if (input.keyHeld(sf::Keyboard::S)) mBody->ApplyForceToCenter(b2Vec2(0.0f, -500.0f), true);
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

    b2Body* mBody;
};

class FootContactListener : public b2ContactListener {

    void BeginContact(b2Contact* contact) {
        //check if fixture A was the foot sensor
        void* fixtureUserData = contact->GetFixtureA()->GetUserData();
        if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        //check if fixture B was the foot sensor
        fixtureUserData = contact->GetFixtureB()->GetUserData();
        if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
    }

    void EndContact(b2Contact* contact) {
        //check if fixture A was the foot sensor
        void* fixtureUserData = contact->GetFixtureA()->GetUserData();
        if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        //check if fixture B was the foot sensor
        fixtureUserData = contact->GetFixtureB()->GetUserData();
        if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
    }

};

int main() {

    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Debug", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Debug");
    sf::View view = window.getView();

    // Create the world

    b2World world(b2Vec2(0.0f, -9.81f));

    b2BodyDef groundBodyDef;
    groundBodyDef.position = b2Vec2(0.0f, 0.0f);

    b2PolygonShape groundShape;
    groundShape.SetAsBox(50.0f, 1.0f);

    world.CreateBody(&groundBodyDef)->CreateFixture(&groundShape, 1.0f);

    Player player(world);

    FootContactListener FootContactListener;

    world.SetContactListener(&FootContactListener);
    //

    Camera camera(view);
    camera.setPixelsPerSecondSquared(8000.0f);

    b2DebugDraw debugDraw(window);
    debugDraw.SetFlags( b2Draw::e_shapeBit );
    world.SetDebugDraw(&debugDraw);

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
        window.setView(camera.getView());

        input.collect(window, view);

        if (input.keyReleased(sf::Keyboard::Escape)) {
            window.close();
        }

        player.update(input);

        world.Step(dt, velocityIterations, positionIterations);

        window.clear();
        world.DrawDebugData();
        window.display();
    }

    return 0;
}
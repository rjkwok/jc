#ifndef JC_LEVEL
#define JC_LEVEL

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

#include "contact.h"

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

    Ladder(b2World& world, const json& ladder_data) {

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

class Bouncer {

public:

    Bouncer() = default;

    Bouncer(b2World& world, const b2Vec2 pos) : pos(pos) {

        b2BodyDef bodyDef;
        bodyDef.position = pos;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 3.0f), b2Vec2(6.0f, 3.0f), b2Vec2(6.0f, 0.0f) };
        rectangle.Set(vertices, 4);

        b2CircleShape circle;
        circle.m_radius = 3.0f;
        circle.m_p = b2Vec2(3.0f, 3.0f);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = VERTICAL;
        fixtureDef.density = 1.0f;
        fixtureDef.restitution = 1.0f;
        fixtureDef.friction = 0.0f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

       /* b2FixtureDef circleFixtureDef;
        circleFixtureDef.filter.categoryBits = VERTICAL;
        circleFixtureDef.density = 1.0f;
        circleFixtureDef.restitution = 1.0f;
        circleFixtureDef.shape = &circle;

        body->CreateFixture(&circleFixtureDef);*/
    }

    Bouncer(b2World& world, const json& bouncer_data) {

        pos = b2Vec2(bouncer_data["pos"]["x"], bouncer_data["pos"]["y"]);

        b2BodyDef bodyDef;
        bodyDef.position = pos;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 3.0f), b2Vec2(6.0f, 3.0f), b2Vec2(6.0f, 0.0f) };
        rectangle.Set(vertices, 4);

        b2CircleShape circle;
        circle.m_radius = 3.0f;
        circle.m_p = b2Vec2(3.0f, 3.0f);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = VERTICAL;
        fixtureDef.density = 1.0f;
        fixtureDef.restitution = 1.0f;
        fixtureDef.friction = 0.0f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

      /* b2FixtureDef circleFixtureDef;
        circleFixtureDef.filter.categoryBits = VERTICAL;
        circleFixtureDef.density = 1.0f;
        circleFixtureDef.restitution = 1.0f;
        circleFixtureDef.shape = &circle;

        body->CreateFixture(&circleFixtureDef);*/
    }

    json getJSON() {
        json bouncer_data;
        bouncer_data["pos"] = { { "x", pos.x }, { "y", pos.y } };
        return bouncer_data;
    }

    b2Vec2 pos;
};

class Swing {

public:

    Swing() = default;

    Swing(b2World& world, const b2Vec2& start, const b2Vec2& end, const b2Vec2& pinA, const b2Vec2& pinB)
        : start(start)
        , end(end)
        , pinA(pinA)
        , pinB(pinB)
    {
        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.type = b2_dynamicBody;
        bodyDef.fixedRotation = true;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = SLIP;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

        b2BodyDef pinADef;
        pinADef.position = pinA;

        b2Body* pinABody = world.CreateBody(&pinADef);

        b2CircleShape pinShape;
        pinShape.m_radius = 1.0f;

        b2FixtureDef pinFixtureDef;
        pinFixtureDef.isSensor = true;
        pinFixtureDef.density = 0.0f;
        pinFixtureDef.shape = &pinShape;

        pinABody->CreateFixture(&pinFixtureDef);

        b2BodyDef pinBDef;
        pinBDef.position = pinB;

        b2Body* pinBBody = world.CreateBody(&pinBDef);

        pinBBody->CreateFixture(&pinFixtureDef);

        b2RopeJointDef ropeDefA;
        ropeDefA.localAnchorA = b2Vec2(0.0f, 0.0f);
        ropeDefA.localAnchorB = b2Vec2(0.0f, 0.0f);
        ropeDefA.maxLength = (start - pinA).Length();
        ropeDefA.bodyA = body;
        ropeDefA.bodyB = pinABody;

        world.CreateJoint(&ropeDefA);

        b2RopeJointDef ropeDefB;
        ropeDefB.localAnchorA = b2Vec2((end - start).Length(), 0.0f);
        ropeDefB.localAnchorB = b2Vec2(0.0f, 0.0f);
        ropeDefB.maxLength = (end - pinB).Length();
        ropeDefB.bodyA = body;
        ropeDefB.bodyB = pinBBody;

        world.CreateJoint(&ropeDefB);
    }

    Swing(b2World& world, const json& edge_data) {

        start = b2Vec2(edge_data["start"]["x"], edge_data["start"]["y"]);
        end = b2Vec2(edge_data["end"]["x"], edge_data["end"]["y"]);
        pinA = b2Vec2(edge_data["pinA"]["x"], edge_data["pinA"]["y"]);
        pinB = b2Vec2(edge_data["pinB"]["x"], edge_data["pinB"]["y"]);

        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.type = b2_dynamicBody;
        bodyDef.fixedRotation = true;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = SLIP;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

        b2BodyDef pinADef;
        pinADef.position = pinA;

        b2Body* pinABody = world.CreateBody(&pinADef);

        b2CircleShape pinShape;
        pinShape.m_radius = 1.0f;

        b2FixtureDef pinFixtureDef;
        pinFixtureDef.isSensor = true;
        pinFixtureDef.density = 0.0f;
        pinFixtureDef.shape = &pinShape;

        pinABody->CreateFixture(&pinFixtureDef);

        b2BodyDef pinBDef;
        pinBDef.position = pinB;

        b2Body* pinBBody = world.CreateBody(&pinBDef);

        pinBBody->CreateFixture(&pinFixtureDef);

        b2RopeJointDef ropeDefA;
        ropeDefA.localAnchorA = b2Vec2(0.0f, 0.0f);
        ropeDefA.localAnchorB = b2Vec2(0.0f, 0.0f);
        ropeDefA.maxLength = (start - pinA).Length();
        ropeDefA.bodyA = body;
        ropeDefA.bodyB = pinABody;

        world.CreateJoint(&ropeDefA);

        b2RopeJointDef ropeDefB;
        ropeDefB.localAnchorA = b2Vec2((end - start).Length(), 0.0f);
        ropeDefB.localAnchorB = b2Vec2(0.0f, 0.0f);
        ropeDefB.maxLength = (end - pinB).Length();
        ropeDefB.bodyA = body;
        ropeDefB.bodyB = pinBBody;

        world.CreateJoint(&ropeDefB);
    }

    json getJSON() {
        json edge_data;
        edge_data["start"] = { { "x", start.x }, { "y", start.y } };
        edge_data["end"] = { { "x", end.x }, { "y", end.y } };
        edge_data["pinA"] = { { "x", pinA.x }, { "y", pinA.y } };
        edge_data["pinB"] = { { "x", pinB.x }, { "y", pinB.y } };
        return edge_data;
    }

    b2Vec2 start;
    b2Vec2 end;
    b2Vec2 pinA;
    b2Vec2 pinB;
};


class PinnedEdge {

public:

    PinnedEdge() = default;

    PinnedEdge(b2World& world, const b2Vec2& start, const b2Vec2& end)
        : start(start)
        , end(end)
    {
        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.fixedRotation = false;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = FALL;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

        b2BodyDef pinDef;
        pinDef.position = start;

        b2Body* pinBody = world.CreateBody(&pinDef);

        b2CircleShape pinShape;
        pinShape.m_radius = 0.1f;

        b2FixtureDef pinFixtureDef;
        pinFixtureDef.isSensor = true;
        pinFixtureDef.density = 0.0f;
        pinFixtureDef.shape = &pinShape;

        pinBody->CreateFixture(&pinFixtureDef);

        b2RevoluteJointDef pinJointDef;
        pinJointDef.Initialize(body, pinBody, start);

        world.CreateJoint(&pinJointDef);
    }

    PinnedEdge(b2World& world, const json& edge_data) {

        start = b2Vec2(edge_data["start"]["x"], edge_data["start"]["y"]);
        end = b2Vec2(edge_data["end"]["x"], edge_data["end"]["y"]);

        b2BodyDef bodyDef;
        bodyDef.position = start;
        bodyDef.angle = atan2(end.y - start.y, end.x - start.x);
        bodyDef.fixedRotation = false;

        float thickness = 0.3f;

        b2PolygonShape rectangle;
        b2Vec2 vertices[4] = { b2Vec2(0.0f, thickness/2.0f), b2Vec2((end - start).Length(), thickness/2.0f), b2Vec2((end - start).Length(), -thickness/2.0f), b2Vec2(0.0f, -thickness/2.0f) };
        rectangle.Set(vertices, 4);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = FALL;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &rectangle;

        body->CreateFixture(&fixtureDef);

        b2BodyDef pinDef;
        pinDef.position = start;

        b2Body* pinBody = world.CreateBody(&pinDef);

        b2CircleShape pinShape;
        pinShape.m_radius = 0.1f;

        b2FixtureDef pinFixtureDef;
        pinFixtureDef.isSensor = true;
        pinFixtureDef.density = 0.0f;
        pinFixtureDef.shape = &pinShape;

        pinBody->CreateFixture(&pinFixtureDef);

        b2RevoluteJointDef pinJointDef;
        pinJointDef.Initialize(body, pinBody, start);

        world.CreateJoint(&pinJointDef);
    }

    json getJSON() {
        json edge_data;
        edge_data["start"] = { { "x", start.x }, { "y", start.y } };
        edge_data["end"] = { { "x", end.x }, { "y", end.y } };
        return edge_data;
    }

    b2Vec2 start;
    b2Vec2 end;
};

class Thorn {

public:

    Thorn() = default;

    Thorn(b2World& world, const b2Vec2 pos, const float height, const float angle)
        : pos(pos)
        , height(height)
        , angle(angle)
    {
        b2BodyDef bodyDef;
        bodyDef.position = pos;
        bodyDef.angle = angle;

        b2PolygonShape thorn;
        b2Vec2 vertices[3] = { b2Vec2(0.0f, 0.0f), b2Vec2(1.0f, height), b2Vec2(2.0f, 0.0f) };
        thorn.Set(vertices, 3);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = THORN;
        fixtureDef.density = 1.0f;
        fixtureDef.isSensor = true;
        fixtureDef.shape = &thorn;

        body->CreateFixture(&fixtureDef);
    }

    Thorn(b2World& world, const json& thorn_data) {

        pos = b2Vec2(thorn_data["pos"]["x"], thorn_data["pos"]["y"]);
        height = thorn_data["height"];
        angle = thorn_data["angle"];

        b2BodyDef bodyDef;
        bodyDef.position = pos;
        bodyDef.angle = angle;

        b2PolygonShape thorn;
        b2Vec2 vertices[3] = { b2Vec2(0.0f, 0.0f), b2Vec2(1.0f, height), b2Vec2(2.0f, 0.0f) };
        thorn.Set(vertices, 3);

        b2Body* body = world.CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = THORN;
        fixtureDef.density = 1.0f;
        fixtureDef.isSensor = true;
        fixtureDef.shape = &thorn;

        body->CreateFixture(&fixtureDef);
    }

    json getJSON() {
        json thorn_data;
        thorn_data["pos"] = { { "x", pos.x }, { "y", pos.y } };
        thorn_data["height"] = height;
        thorn_data["angle"] = angle;
        return thorn_data;
    }

    b2Vec2 pos;
    float height;
    float angle;
};

class BoulderSpawn {

public:

    BoulderSpawn() = default;

    BoulderSpawn(b2World& world, const b2Vec2 start, const b2Vec2 end, float radius, float bouldersPerSecond)
        : pos(start)
        , radius(radius)
        , bouldersPerSecond(bouldersPerSecond)
    {
        direction = end - start;
        direction.Normalize();
    }

    BoulderSpawn(b2World& world, const json& boulder_spawn_data) {

        pos = b2Vec2(boulder_spawn_data["pos"]["x"], boulder_spawn_data["pos"]["y"]);
        direction = b2Vec2(boulder_spawn_data["direction"]["x"], boulder_spawn_data["direction"]["y"]);
        radius = boulder_spawn_data["radius"];
        bouldersPerSecond = boulder_spawn_data["bouldersPerSecond"];
    }

    void update(b2World& world, const float dt) {

        secondsSinceLastBoulder += dt;

        if (secondsSinceLastBoulder >= 1.0f/bouldersPerSecond) {
            spawnBoulder(world);
            secondsSinceLastBoulder = 0.0f;
        }
    }

    void spawnBoulder(b2World& world) {

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = pos;

        b2Body* body = world.CreateBody(&bodyDef);

        b2CircleShape boulder;
        boulder.m_p.Set(0.0f, 0.0f);
        boulder.m_radius = radius;

        b2FixtureDef fixtureDef;
        fixtureDef.filter.categoryBits = BOULDER;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.0f;
        fixtureDef.shape = &boulder;

        body->CreateFixture(&fixtureDef);

        body->SetLinearVelocity(20.0f*direction);
        body->SetAngularVelocity(-12.566);
    }

    json getJSON() {
        json boulder_spawn_data;
        boulder_spawn_data["pos"] = { { "x", pos.x }, { "y", pos.y } };
        boulder_spawn_data["direction"] = { { "x", direction.x }, { "y", direction.y } };
        boulder_spawn_data["radius"] = radius;
        boulder_spawn_data["bouldersPerSecond"] = bouldersPerSecond;
        return boulder_spawn_data;
    }

    b2Vec2 pos;
    b2Vec2 direction;
    float radius;
    float bouldersPerSecond;

private:

    float secondsSinceLastBoulder;
};

class Level {

public:

    Level() {
        world = new b2World(b2Vec2(0.0f, -90.81f));
    }

    void loadFromFile(const std::string& path) {

        // reset
        world = new b2World(b2Vec2(0.0f, -90.81f));
        level.world->SetContactListener(&myContactListener);

        edges.clear();
        ladders.clear();
        thorns.clear();
        boulderSpawns.clear();
        bouncers.clear();
        swings.clear();
        pinnedEdges.clear();
        //

        // Load
        json levelData;
        std::ifstream file(path);

        if (!file.good())   return;

        file >> levelData;
        file.close();

        for (auto each: levelData["edges"]) {
            edges.push_back(Edge(*world, each));
        }

        for (auto each: levelData["ladders"]) {
            ladders.push_back(Ladder(*world, each));
        }

        for (auto each: levelData["thorns"]) {
            thorns.push_back(Thorn(*world, each));
        }

        for (auto each: levelData["boulderSpawns"]) {
            boulderSpawns.push_back(BoulderSpawn(*world, each));
        }

        for (auto each: levelData["bouncers"]) {
            bouncers.push_back(Bouncer(*world, each));
        }

        for (auto each: levelData["swings"]) {
            swings.push_back(Swing(*world, each));
        }

        for (auto each: levelData["pinnedEdges"]) {
            pinnedEdges.push_back(PinnedEdge(*world, each));
        }

        spawn = b2Vec2(levelData["spawn"]["x"], levelData["spawn"]["y"]);
        min = levelData["min"];
        max = levelData["max"];

        b2BodyDef minDef;
        minDef.position = b2Vec2(min, 0.0f);
        b2EdgeShape edgeShape;
        edgeShape.Set(b2Vec2(0.0f, 100.0f), b2Vec2(0.0f, -100.0f));

        b2FixtureDef minFixtureDef;
        minFixtureDef.shape = &edgeShape;
        minFixtureDef.density = 0.0f;
        minFixtureDef.filter.categoryBits = VERTICAL;

        world->CreateBody(&minDef)->CreateFixture(&minFixtureDef);

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
        for (auto thorn: thorns) {

            levelData["thorns"].push_back(thorn.getJSON());
        }
        for (auto boulderSpawn: boulderSpawns) {

            levelData["boulderSpawns"].push_back(boulderSpawn.getJSON());
        }
        for (auto bouncer: bouncers) {

            levelData["bouncers"].push_back(bouncer.getJSON());
        }
        for (auto swing: swings) {

            levelData["swings"].push_back(swing.getJSON());
        }

        for (auto edge: pinnedEdges) {

            levelData["pinnedEdges"].push_back(edge.getJSON());
        }

        levelData["spawn"] = { { "x", spawn.x }, { "y", spawn.y } };
        levelData["min"] = min;
        levelData["max"] = max;

        std::ofstream file(path);
        file << std::setw(4) << levelData << std::endl;
        file.close();
    }

    void update(const float dt) {

        for (BoulderSpawn& boulderSpawn: boulderSpawns) {
            boulderSpawn.update(*world, dt);
        }
    }

    b2Vec2 spawn;
    float min;
    float max;
    std::vector<Edge> edges;
    std::vector<Ladder> ladders;
    std::vector<Thorn> thorns;
    std::vector<BoulderSpawn> boulderSpawns;
    std::vector<Bouncer> bouncers;
    std::vector<Swing> swings;
    std::vector<PinnedEdge> pinnedEdges;
    MyContactListener myContactListener;
    b2World* world;
};

#endif
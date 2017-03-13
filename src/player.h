#ifndef JC_PLAYER
#define JC_PLAYER

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#include "util.h"
#include "level.h"

class Player {

public:

    Player(Level& level) {
        respawn(level);
    }

    void respawn(Level& level) {

        // Define main player bounds
        b2BodyDef boxBodyDef;
        boxBodyDef.position = level.spawn;
        boxBodyDef.fixedRotation = true;
        boxBodyDef.type = b2_dynamicBody;

        mBody = level.world->CreateBody(&boxBodyDef);

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

    void kill(Level& level) {

        level.world->DestroyBody(mBody);
        respawn(level);
    }

    void update(const Input& input, Level& level) {

        if (touchingThorn()) {
            kill(level);
        }

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

        if (axis != 0 || (touchingGround() && !touchingSlipSurface())) {
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
    }

    b2Vec2 getPosition() const {

        return mBody->GetPosition();
    }

    bool touchingGround() {

        return mFootContactCount > 0;
    }

    bool touchingSlipSurface() {

        return mSlipContactCount > 0;
    }

    bool touchingLadder() {

        return mLadderContactCount > 0;
    }

    bool touchingThorn() {

        return mThornContactCount > 0;
    }

    int mFootContactCount = 0;
    int mLadderContactCount = 0;
    int mThornContactCount = 0;
    int mSlipContactCount = 0;
    b2Body* mBody;
    sf::Sprite sprite;

private:
    ///////////////////////////

    bool mFacingRight = true;
    int mExtraJumps = 0;
};

#endif
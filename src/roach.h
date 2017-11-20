#ifndef JC_ROACH
#define JC_ROACH

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#include "util.h"
#include "level.h"

class Roach {
public:

	Roach(Level& level, sf::Texture& texture, int roachNum) {
		spawn(level, roachNum);

       	sprite.setTexture(texture, true);
        sf::Vector2u bounds = texture.getSize();
        sprite.setOrigin(bounds.x/2.0f, bounds.y);
        sprite.setScale(0.3f, 0.3f); // will have to change depenting on texture used
    }

    void spawn(Level& level, int roachNum){

        b2Vec2 roachSpawn;

        if(roachNum == 1){
            roachSpawn = b2Vec2(85,10);
        }
        if(roachNum == 2){
            roachSpawn = b2Vec2(20,10);
        }

    	b2BodyDef boxBodyDef;
        boxBodyDef.position = (roachSpawn);
        boxBodyDef.fixedRotation = true;
        boxBodyDef.type = b2_dynamicBody;

        mBody = level.world->CreateBody(&boxBodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox(2.0f, 1.0f);

        b2FixtureDef boxFixtureDef;
        boxFixtureDef.filter.categoryBits = ROACH;
        boxFixtureDef.shape = &boxShape;
        boxFixtureDef.friction = 0.0f;
        boxFixtureDef.density = 1.0f;

        mBody->CreateFixture(&boxFixtureDef)->SetUserData((void*)this);

        //left pathing sensor
        b2PolygonShape LPathSensorShape;
        LPathSensorShape.SetAsBox(0.25f, 0.25f, b2Vec2(-2.5f, -1.0f), 0.0f);

        b2FixtureDef LPathSensorFixtureDef;
        LPathSensorFixtureDef.filter.categoryBits = LPATH;
        LPathSensorFixtureDef.shape = &LPathSensorShape;
        LPathSensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&LPathSensorFixtureDef)->SetUserData((void*)this);

        //right pathing sensor
        b2PolygonShape RPathSensorShape;
        RPathSensorShape.SetAsBox(0.25f, 0.25f, b2Vec2(2.5f, -1.0f), 0.0f);

        b2FixtureDef RPathSensorFixtureDef;
        RPathSensorFixtureDef.filter.categoryBits = RPATH;
        RPathSensorFixtureDef.shape = &RPathSensorShape;
        RPathSensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&RPathSensorFixtureDef)->SetUserData((void*)this);

    }
    

   void update(){

        walk(mWalkSpeed*mDirection);

        if(mLFloorContactCount == 0){
            mDirection = 1;
        }

        if(mLWallContactCount > 0){
            mDirection = 1;
        }

        if(mRFloorContactCount == 0){
            mDirection = -1;
        }

        if(mRWallContactCount > 0){
            mDirection = -1;
        }

    }

    void walk(float x_velocity) {
        mBody->SetLinearVelocity(b2Vec2(x_velocity, mBody->GetLinearVelocity().y));
    }

    b2Vec2 getPosition() const {

        return mBody->GetPosition();
    }


    sf::Sprite sprite;
    b2Body* mBody;
    int mLFloorContactCount = 0;
    int mLWallContactCount = 0;
    int mRFloorContactCount = 0;
    int mRWallContactCount = 0;

private:

    int mDirection = 1;
    const float mWalkSpeed = 5.0f;

};

#endif
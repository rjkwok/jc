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

	Roach(Level& level, sf::Texture& texture) {
		spawn(level);

       	sprite.setTexture(texture, true);
        sf::Vector2u bounds = texture.getSize();
        sprite.setOrigin(bounds.x/2.0f, bounds.y);
        sprite.setScale(0.3f, 0.3f); // will have to change depenting on texture used
    }

    void spawn(Level& level){
        b2Vec2 roachSpawnOne = b2Vec2(30,10);
    	b2BodyDef boxBodyDef;
        boxBodyDef.position = (roachSpawnOne);
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
        LPathSensorFixtureDef.filter.categoryBits = PATH;
        LPathSensorFixtureDef.shape = &LPathSensorShape;
        LPathSensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&LPathSensorFixtureDef)->SetUserData((void*)this);

        //right pathing sensor
        b2PolygonShape RPathSensorShape;
        RPathSensorShape.SetAsBox(0.25f, 0.25f, b2Vec2(2.5f, -1.0f), 0.0f);

        b2FixtureDef RPathSensorFixtureDef;
        RPathSensorFixtureDef.filter.categoryBits = PATH;
        RPathSensorFixtureDef.shape = &RPathSensorShape;
        RPathSensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&RPathSensorFixtureDef)->SetUserData((void*)this);

    }
    

   void update(){

        walk(mWalkSpeed*mDirection);

        if(willThereBeGround() == 0){
            mDirection = mDirection*(-1);
        }

        if(willThereBeWall() == 0){
            mDirection = mDirection*(-1);
        }

        std::cout<<"mDirection"<<mDirection<<"     Wallcount"<<mWallContactCount<<std::endl;

    }

    void walk(float x_velocity) {
        mBody->SetLinearVelocity(b2Vec2(x_velocity, mBody->GetLinearVelocity().y));
    }

    b2Vec2 getPosition() const {

        return mBody->GetPosition();
    }

    bool willThereBeGround() {

        return mFloorContactCount > 0;
    }

    bool willThereBeWall() {

        return mWallContactCount > 0;
    }

    sf::Sprite sprite;
    b2Body* mBody;
    int mFloorContactCount = 0;
    int mWallContactCount = 0;

private:

    int mDirection = 1;
    const float mWalkSpeed = 5.0f;

};

#endif
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
    	b2BodyDef boxBodyDef;
        boxBodyDef.position = level.spawn;
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

        b2PolygonShape footSensorShape;
        footSensorShape.SetAsBox(1.0f, 0.25f, b2Vec2(0.0f, -1.0f), 0.0f);

        b2FixtureDef sensorFixtureDef;
        sensorFixtureDef.filter.categoryBits = FEET;
        sensorFixtureDef.shape = &footSensorShape;
        sensorFixtureDef.isSensor = true;

        mBody->CreateFixture(&sensorFixtureDef)->SetUserData((void*)this);

        

    }
    

   // void update(const Input& input, Level& level, const float dt){


     // }

    sf::Sprite sprite;
    b2Body* mBody;

private:

};

#endif
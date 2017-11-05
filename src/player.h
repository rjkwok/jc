#ifndef JC_PLAYER
#define JC_PLAYER

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#include "util.h"
#include "level.h"
#include "animation.h"

class Player {


public:

    float mHealthPercent;

    Player(Level& level, sf::Texture& texture) {
        respawn(level);

        sprite.setTexture(texture, true);
        sf::Vector2u bounds = texture.getSize();
        sprite.setOrigin(bounds.x/2.0f, bounds.y);
        sprite.setScale(0.3f, 0.3f);
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
        mHealth = mMaxHealth;
    }

    void jump() {
        if (touchingGround()) {
            mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 30.0f));
        }
        else if (mDoubleJumpEnabled && mExtraJumps > 0) {
            mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 30.0f));
            --mExtraJumps;
        }
    }

    void climb() {
        if (mLadderClimbEnabled && touchingLadder()) {
            mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 20.0f));
        }
    }

    void walk(float x_velocity) {
        if (x_velocity != 0 || (touchingGround() && !touchingSlipSurface())) {
            mBody->SetLinearVelocity(b2Vec2(x_velocity, mBody->GetLinearVelocity().y));
        }
    }

    void update(const Input& input, Level& level, const float dt, std::map<std::string, sf::Texture*>& textures) {

        mHealthPercent = (mHealth/float(mMaxHealth));

        if (touchingThorn()) {

            //if no i frames give i frames and take health, if at min health kil
            if (mHaveIFrames == false) {

                mHaveIFrames = true;

                if (mHealth != 1) {
                    mHealth--;
                } else {
                kill(level);
                }
            }

        }

        if (mIFrameTime > mMaxIFrameTime) {
            mHaveIFrames = false;
            mIFrameTime = 0;
        }

        if (mHaveIFrames == true){
            mIFrameTime += dt;  //net change in time since damage
        }

        // Reset x velocity to 0
        mBody->SetLinearVelocity(b2Vec2(0, mBody->GetLinearVelocity().y));

        if (!animation.play(dt, sprite, 2)) { // as this is the default animation(0,0) it is basically a clock that triggers false every 1/fps seconds
            sprite.setTexture(*textures["neutral"]);
        }

        // Mapping of user inputs to player actions
        if (input.keyPressed(sf::Keyboard::Space)) {

            jump();
        }
        if (input.keyHeld(sf::Keyboard::W)) {

            climb();
        }
        if (input.keyHeld(sf::Keyboard::D)) {

            walk(mSprintEnabled && input.keyHeld(sf::Keyboard::LShift) ? 2*mWalkSpeed : mWalkSpeed);
            mFacingRight = true;
        }
        if (input.keyHeld(sf::Keyboard::A)) {

            walk(mSprintEnabled && input.keyHeld(sf::Keyboard::LShift) ? -2*mWalkSpeed : -mWalkSpeed);
            mFacingRight = false;
        }

        // front facing punch
        if ((mFacingRight && input.keyPressed(sf::Keyboard::Right)) || (!mFacingRight && input.keyPressed(sf::Keyboard::Left))) {

            if (input.keyHeld(sf::Keyboard::LShift)) {
                // counter
            }
            else {
                // melee
                Effect effect(textures["forward_effect"], 613, 251, 0, 0, 0.1);
                effect.setPosition(sprite.getPosition() + sf::Vector2f(-160*sprite.getScale().x, -576*sprite.getScale().y));
                effect.setVelocity(mBody->GetLinearVelocity());
                effect.setScale(sprite.getScale().x, sprite.getScale().y);
                level.effects.push_back(effect);
                animation = Animation(0,0); // "set the animation" currently just resets the animation so that we sit on this frame for a full cycle
                sprite.setTexture(*textures["forward"]);
            }
        }

        // upwards punch
        if (input.keyPressed(sf::Keyboard::Up)) {

            if (input.keyHeld(sf::Keyboard::LShift)) {
                // counter
            }
            else {
                // melee
                Effect effect(textures["up_effect"], 750, 569, 0, 0, 0.1);
                effect.setPosition(sprite.getPosition() + sf::Vector2f(-350*sprite.getScale().x, -831*sprite.getScale().y));
                effect.setVelocity(mBody->GetLinearVelocity());
                effect.setScale(sprite.getScale().x, sprite.getScale().y);
                level.effects.push_back(effect);
                animation = Animation(0,0); // "set the animation" currently just resets the animation so that we sit on this frame for a full cycle
                sprite.setTexture(*textures["up"]);
            }
        }

        // rear facing punch
        if ((mFacingRight && input.keyPressed(sf::Keyboard::Left)) || (!mFacingRight && input.keyPressed(sf::Keyboard::Right))) {

            if (input.keyHeld(sf::Keyboard::LShift)) {
                // counter
            }
            else {
                // melee
                Effect effect(textures["back_effect"], 154, 408, 0, 0, 0.1);
                effect.setPosition(sprite.getPosition() + sf::Vector2f(-260*sprite.getScale().x, -776*sprite.getScale().y));
                effect.setVelocity(mBody->GetLinearVelocity());
                effect.setScale(sprite.getScale().x, sprite.getScale().y);
                level.effects.push_back(effect);
                animation = Animation(0,0); // "set the animation" currently just resets the animation so that we sit on this frame for a full cycle
                sprite.setTexture(*textures["back"]);
            }
        }

        // If the player has recontacted the ground this tick, "restock" the amount of extra jumps for the next tick
        if (touchingGround())   mExtraJumps = 1;

        // Set sprite direction and orientation
        sprite.setPosition(convertb2Vec2(getPosition() - b2Vec2(0.0f, 1.0f)));
        if (mFacingRight)   sprite.setScale(0.6f, 0.6f);
        else                sprite.setScale(-0.6f, 0.6f);
    }

    void draw(sf::RenderWindow& window, const float dt) {
        if (mHaveIFrames == 1){
            if (mFlashTimer <= mMaxFlashTime){
               window.draw(sprite); 
               mFlashTimer += dt;
            }else if(mFlashTimer > mMaxFlashTime && mFlashTimer < (2 * mMaxFlashTime)){
                mFlashTimer += dt;
            }else{
                mFlashTimer = 0;
            }
            
        }else{
            mFlashTimer = 0;
            window.draw(sprite);  
        }
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
    Animation animation;

    bool mDoubleJumpEnabled = true;
    bool mLadderClimbEnabled = true;
    bool mSprintEnabled = true;

private:
    ///////////////////////////

    bool mFacingRight = true;
    bool mHaveIFrames = false;
    int mExtraJumps = 0;
    int mHealth = 3;
    float mMaxIFrameTime = 0.5; //in seconds
    const int mMaxHealth = 3;
    const float mWalkSpeed = 15.0f;
    float mIFrameTime = 0;
    float mFlashTimer = 0;
    float mMaxFlashTime = 0.08;
};

#endif
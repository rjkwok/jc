#ifndef JC_CONTACT
#define JC_CONTACT

#include "Box2D/Box2D.h"
#include "player.h"
#include "roach.h"

class MyContactListener : public b2ContactListener {

    void BeginContact(b2Contact* contact) {

        beginContactEvents.push_back(std::pair<b2Fixture*, b2Fixture*>(contact->GetFixtureA(), contact->GetFixtureB()));

        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET && contact->GetFixtureB()->GetFilterData().categoryBits != VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET && contact->GetFixtureA()->GetFilterData().categoryBits != VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount++;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET && contact->GetFixtureB()->GetFilterData().categoryBits == SLIP) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mSlipContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET && contact->GetFixtureA()->GetFilterData().categoryBits == SLIP) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mSlipContactCount++;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LADDER && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LADDER && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount++;
        }
        if (((contact->GetFixtureA()->GetFilterData().categoryBits == THORN || contact->GetFixtureA()->GetFilterData().categoryBits == BOULDER) 
            || contact->GetFixtureA()->GetFilterData().categoryBits == ROACH) && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mThornContactCount++;
        }
        if (((contact->GetFixtureB()->GetFilterData().categoryBits == THORN || contact->GetFixtureB()->GetFilterData().categoryBits == BOULDER) 
            || contact->GetFixtureB()->GetFilterData().categoryBits == ROACH) && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mThornContactCount++;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LPATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLFloorContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LPATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLFloorContactCount++;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LPATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLWallContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LPATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLWallContactCount++;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == RPATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRFloorContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == RPATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRFloorContactCount++;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == RPATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRWallContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == RPATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRWallContactCount++;
        }        


    }

    void EndContact(b2Contact* contact) {

        endContactEvents.push_back(std::pair<b2Fixture*, b2Fixture*>(contact->GetFixtureA(), contact->GetFixtureB()));

        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET && contact->GetFixtureB()->GetFilterData().categoryBits != VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET && contact->GetFixtureA()->GetFilterData().categoryBits != VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mFootContactCount--;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == FEET && contact->GetFixtureB()->GetFilterData().categoryBits == SLIP) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mSlipContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == FEET && contact->GetFixtureA()->GetFilterData().categoryBits == SLIP) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mSlipContactCount--;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LADDER && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LADDER && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mLadderContactCount--;
        }
        if (((contact->GetFixtureA()->GetFilterData().categoryBits == THORN || contact->GetFixtureA()->GetFilterData().categoryBits == BOULDER)
        || contact->GetFixtureA()->GetFilterData().categoryBits == ROACH)  && contact->GetFixtureB()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mThornContactCount--;
        }
        if (((contact->GetFixtureB()->GetFilterData().categoryBits == THORN || contact->GetFixtureB()->GetFilterData().categoryBits == BOULDER)
        || contact->GetFixtureB()->GetFilterData().categoryBits == ROACH) && contact->GetFixtureA()->GetFilterData().categoryBits == PLAYER) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Player*>(fixtureUserData)->mThornContactCount--;
        }
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LPATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLFloorContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LPATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLFloorContactCount--;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == LPATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLWallContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == LPATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mLWallContactCount--;
        }  
        if (contact->GetFixtureA()->GetFilterData().categoryBits == RPATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRFloorContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == RPATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRFloorContactCount--;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == RPATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRWallContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == RPATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mRWallContactCount--;
        }   
    }

public:

    std::vector<std::pair<b2Fixture*, b2Fixture*> > beginContactEvents;
    std::vector<std::pair<b2Fixture*, b2Fixture*> > endContactEvents;
};

#endif
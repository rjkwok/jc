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
        if (contact->GetFixtureA()->GetFilterData().categoryBits == PATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mFloorContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == PATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mFloorContactCount++;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == PATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mWallContactCount++;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == PATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mWallContactCount++;
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
        if (contact->GetFixtureA()->GetFilterData().categoryBits == PATH && contact->GetFixtureB()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mFloorContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == PATH && contact->GetFixtureA()->GetFilterData().categoryBits == HORIZONTAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mFloorContactCount--;
        }        
        if (contact->GetFixtureA()->GetFilterData().categoryBits == PATH && contact->GetFixtureB()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureB()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mWallContactCount--;
        }
        if (contact->GetFixtureB()->GetFilterData().categoryBits == PATH && contact->GetFixtureA()->GetFilterData().categoryBits == VERTICAL) {
            void* fixtureUserData = contact->GetFixtureA()->GetUserData();
            if (fixtureUserData)    static_cast<Roach*>(fixtureUserData)->mWallContactCount--;
        }   
    }

public:

    std::vector<std::pair<b2Fixture*, b2Fixture*> > beginContactEvents;
    std::vector<std::pair<b2Fixture*, b2Fixture*> > endContactEvents;
};

#endif
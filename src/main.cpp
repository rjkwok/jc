#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

#include "util.h"
#include "contact.h"
#include "level.h"
#include "player.h"
#include "draw.h"
#include "camera.h"
#include "builder.h"
#include "hud.h"
#include "roach.h"
#include "animation.h"
#include "gui.h"

void loadTexture(std::map<std::string, sf::Texture*>& textures, const std::string name, const std::string path) {

    textures[name] = new sf::Texture();
    textures[name]->loadFromFile(path);
}

int main() {

    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Debug", sf::Style::Fullscreen);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Debug");
    sf::View view = window.getView();
    sf::View window_view = window.getView();

    // Create the world
    Level level;

    std::vector<Builder*> builders;
    builders.push_back(new Builder());
    builders.push_back(new EdgeBuilder());
    builders.push_back(new FallingEdgeBuilder());
    builders.push_back(new LadderBuilder());
    builders.push_back(new ThornBuilder());
    builders.push_back(new BoulderSpawnBuilder());
    builders.push_back(new BouncerBuilder());
    builders.push_back(new SwingBuilder());
    builders.push_back(new PinnedEdgeBuilder());
    auto builder = builders.begin();

    level.loadFromFile("level.json");

    // We need a lot of textures and we need to reference them a lot right now to create new effects, switch between
    // player attacks, etc. so we can store them in this labelled container and pass it around the program to any
    // function that needs to use textures.
    std::map<std::string, sf::Texture*> textures;
    loadTexture(textures, "neutral", "neutral.png");
    loadTexture(textures, "forward", "forward.png");
    loadTexture(textures, "up", "up.png");
    loadTexture(textures, "back", "back.png");
    loadTexture(textures, "roach", "roachTextureTest");
    loadTexture(textures, "forward_effect", "forward_effect.png");
    loadTexture(textures, "up_effect", "up_effect.png");
    loadTexture(textures, "back_effect", "back_effect.png");

    Player player(level, *textures["neutral"]);
    Roach roachTest(level, *textures["roach"]);
    //

    Camera camera(view);
    camera.setPixelsPerSecondSquared(8000.0f);
    camera.mMin = level.min*40;
    camera.mMax = level.max*40;

    b2DebugDraw debugDraw(window);
    debugDraw.SetFlags( b2Draw::e_shapeBit);
    debugDraw.AppendFlags( b2Draw::e_jointBit );
    level.world->SetDebugDraw(&debugDraw);

    Input input;

    int32 positionIterations = 2;
    int32 velocityIterations = 6;

    //initialize loop timer
    sf::Clock timer;
    double dt = 0;
    //

    MyContactListener myContactListener;
    level.setContactListener(&myContactListener);

    while (window.isOpen()) {

        //loop header
        window.clear();
        dt = timer.getElapsedTime().asSeconds();

        if (dt == 0)    dt = 0.000000001;

        timer.restart();
        //

        if (player.getPosition().x > level.max) {

            // go to next level
            //level.loadFromFile("");
            player.respawn(level);
        }

        camera.track(convertb2Vec2(player.getPosition()));
        camera.zoom(-input.mmb_delta);
        camera.update(dt);
        window.setView(camera.mView);

        input.collect(window, camera.mView);

        if (input.keyReleased(sf::Keyboard::Escape)) {
            window.close();
        }

        if (input.keyReleased(sf::Keyboard::Tab)) {
            builder++;
            if (builder == builders.end())  builder = builders.begin();
        }

        (*builder)->update(input, &level);

        player.update(input, level, dt, textures);
        level.update(dt);
        hudUpdate(window, window_view, player.mHealthPercent);

        level.world->Step(dt, velocityIterations, positionIterations);

        for (auto pair: myContactListener.beginContactEvents) {
            if ((pair.first->GetFilterData().categoryBits == GOAL && pair.second->GetFilterData().categoryBits == PLAYER) ||
                (pair.second->GetFilterData().categoryBits == GOAL && pair.first->GetFilterData().categoryBits == PLAYER)) {

                // level is completed
            }
            if (pair.first->GetFilterData().categoryBits == FALL) {
                pair.first->GetBody()->SetType(b2_dynamicBody);
            }
            if (pair.second->GetFilterData().categoryBits == FALL) {
                pair.second->GetBody()->SetType(b2_dynamicBody);
            }
            if (pair.first->GetFilterData().categoryBits == FISH && pair.second->GetFilterData().categoryBits & (HORIZONTAL | FALL)) {
                level.world->DestroyBody(pair.first->GetBody());
            }
            if (pair.second->GetFilterData().categoryBits == FISH && pair.first->GetFilterData().categoryBits & (HORIZONTAL | FALL)) {
               level.world->DestroyBody(pair.second->GetBody());
            }
            if (pair.first->GetFilterData().categoryBits == BOULDER && pair.second->GetFilterData().categoryBits == THORN) {
                level.world->DestroyBody(pair.first->GetBody());
            }
            if (pair.second->GetFilterData().categoryBits == BOULDER && pair.first->GetFilterData().categoryBits == THORN) {
               level.world->DestroyBody(pair.second->GetBody());
            }
        }
        myContactListener.beginContactEvents.clear();
        myContactListener.endContactEvents.clear();


        (*builder)->draw(window);
        level.world->DrawDebugData();
        player.draw(window,dt);
        for (Effect effect: level.effects) {
            effect.draw(window);
        }
        window.display();
    }

    for (auto ptr: builders) {
        delete ptr;
    }

    for (auto pair: textures) {
        delete pair.second;
    }

    level.dumpToFile("level.json");
    return 0;
}
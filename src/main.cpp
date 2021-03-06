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

struct Campaign {

    Campaign() = default;
    Campaign(const std::string& title, const std::string& cover_path, const std::string boss_path)
        : title(title)
        , boss_path(boss_path)
    {
        cover = new sf::Texture();
        cover->loadFromFile(cover_path);
    }

    ~Campaign() {
        delete cover;
    }

    void addLevel(const std::string& path) {
        level_paths.push_back(path);
    }

    std::string title;

    std::vector<std::string> level_paths;
    std::string boss_path;

    // cover image resource
    sf::Texture* cover;
};

int main() {

    std::vector<Campaign> campaigns;

    campaigns.push_back(Campaign("A Gift Of Religion", "", ""));
    campaigns[0].addLevel("level.json");

    // generate campaign menu from the campaigns vector

    // hardcode top-level campaign-options-quit menu
    sf::Texture* title_card = new sf::Texture();

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

    Player player(level);
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
            level.loadFromFile("");
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

        player.update(input, level);
        level.update(dt);

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


        window.clear();
        (*builder)->draw(window);
        level.world->DrawDebugData();
        window.display();
    }

    for (auto ptr: builders) {
        delete ptr;
    }

    level.dumpToFile("level.json");
    return 0;
}
#ifndef JC_ANIM
#define JC_ANIM

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

// This function is used in the process of updating a sprite during animation. It divides the texture on a sprite
// into rectangles based on the sprite size, assigns them implicit indices, and then returns the coordinates of the
// rectangle that would be at the specified index.

sf::IntRect getFrame(int frame_index, sf::Sprite &sprite){

    sf::IntRect frame_window = sprite.getTextureRect();
    double x_index = (int(frame_index * frame_window.width) % int(sprite.getTexture()->getSize().x))/double(frame_window.width);
    double y_index = ((frame_index - x_index)*frame_window.width)/sprite.getTexture()->getSize().x;

    frame_window.left = int(x_index*frame_window.width);
    frame_window.top = int(y_index*frame_window.height);

    return frame_window;
}

// An animation object simply stores state information to track a playing animation, including the current frame
// as well as the start and end frames. Animation objects are ignorant of texture and texture size, and just need to
// be provided a sprite with a proper texture assigned to manipulate each tick. This sprite could even change from
// tick to tick, although that wouldn't make sense.

class Animation {

public:

	Animation() = default;

	Animation(int start, int end, bool looping = false)
        : start_frame(start)
        , end_frame(end)
        , is_looping(looping)
        , current_frame(start_frame)
        , time_since_last_frame(0) {}

	bool play(double dt, sf::Sprite &sprite, double fps = 16.0) {
        //this method will return true so long as it has frames to play.

        if(fps == 0){ return false;}

        bool return_signal = true;
        time_since_last_frame += dt;
        if(time_since_last_frame >= 1.0/fps){
            current_frame++;
            time_since_last_frame -= (1.0/fps);
            if(current_frame > end_frame){
                if(is_looping){
                    current_frame = start_frame;
                }
                else{
                    current_frame = end_frame;
                    return_signal = false; //signals animation complete
                }
            }
        }
        sprite.setTextureRect(getFrame(current_frame, sprite));

        return return_signal;
    }

private:

///////////////////////////////

    int start_frame = 0;
    int end_frame = 0;
    bool is_looping = false;
    int current_frame = 0;
    double time_since_last_frame = 0;
};

// An effect is a standalone sprite that exists solely to carry a single animation for a specified time before being
// cleaned up by the level. The sprite can either loop for a fixed time or determine the duration implicitly as the
// length of the provided animation strip at 16 fps. Effects can be scaled, positioned, and assigned a linear velocity
// to make them move as they play.

class Effect {

public:

    Effect() = default;

    Effect(b2Vec2 position, sf::Texture* texture, int width, int height, int start, int end)
        : sprite(*texture, sf::IntRect(0, 0, width, height))
        , animation(start, end, false)
        , max_duration((1/16.0)*(end-start))
        , duration(0) {}

    Effect(sf::Texture* texture, int width, int height, int start, int end, float max_duration)
        : sprite(*texture, sf::IntRect(0, 0, width, height))
        , animation(start, end, true)
        , max_duration(max_duration)
        , duration(0) {}

    void setPosition(sf::Vector2f position) {
        sprite.setPosition(position);
    }

    void setVelocity(b2Vec2 velocity) {
        this->velocity = velocity;
    }

    void setScale(float x, float y) {
        sprite.setScale(x, y);
    }

    void update(const float dt) {

        animation.play(dt, sprite);
        sprite.move(convertb2Vec2(velocity)*dt);
        duration += dt;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    bool alive() {

        return duration < max_duration;
    }

private:

///////////////////////////////

    sf::Sprite  sprite;
    Animation   animation;
    float       max_duration;
    float       duration;
    b2Vec2      velocity = b2Vec2(0,0);
};

#endif
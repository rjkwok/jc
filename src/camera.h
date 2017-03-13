#ifndef JC_CAMERA
#define JC_CAMERA

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

class Camera {

public:

    Camera(const sf::View& view) {

        mView = view;
        mScale = 1.0f;
        mWidth = view.getSize().x;
        mHeight = view.getSize().y;
    }

    void setPixelsPerSecondSquared(const float a) {
        mA = a;
    }

    void zoom(const float scaling) {

        mScale += scaling;

        if (mScale < 0.1f)      mScale = 0.1f;
        else if (mScale > 8.0f) mScale = 8.0f;
    }

    void track(const sf::Vector2f& point) {

        mTrackingPoint = point;
    }

    void update(const float dt) {

        sf::Vector2f a = normalize(mTrackingPoint - (mView.getCenter() + mV*0.5f))*mA;

        mV = mV + (a*dt);

        mView.setSize(mWidth*mScale, mHeight*mScale);
        mView.setCenter(mView.getCenter() + (mV*dt));

        if (mView.getCenter().x - (mWidth*mScale/2.0f) < mMin) {
            mView.setCenter(mMin + (mWidth*mScale/2.0f), mView.getCenter().y);
        }
        if (mView.getCenter().x + (mWidth*mScale/2.0f) > mMax) {
            mView.setCenter(mMax - (mWidth*mScale/2.0f), mView.getCenter().y);
        }
    }

    float mMin;
    float mMax;
    float mScale;
    float mWidth;
    float mHeight;
    sf::View mView;

private:

    sf::Vector2f mTrackingPoint;
    float mA;
    sf::Vector2f mV;

};

#endif
//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include "ofMain.h"
#include "ofxSatellite.h"


class ofApp: public ofBaseApp
{
public:
    void setup();
    void draw();

    ofSpherePrimitive earthSphere;

    ofImage colorMap;

    ofx::Geo::ElevatedCoordinate myLocation;

    std::vector<ofx::Satellite::Satellite> satellites;

    float scaler;
    float rot;

    ofEasyCam cam;

};

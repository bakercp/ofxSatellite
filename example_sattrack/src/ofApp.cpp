//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofApp.h"


void ofApp::setup()
{
    ofEnableDepthTest();
    ofEnableAlphaBlending();

    rot = 0;

    scaler = 300 / ofx::Geo::GeoUtils::EARTH_RADIUS_KM;

    colorMap.load("color_map_1024.jpg");

    earthSphere.set(ofx::Geo::GeoUtils::EARTH_RADIUS_KM, 24);
    ofQuaternion quat;
    quat.makeRotate(180, 0, 1, 0);
    earthSphere.rotate(quat);
    earthSphere.mapTexCoords(0,
                             colorMap.getTexture().getTextureData().tex_u,
                             colorMap.getTexture().getTextureData().tex_t,
                             0);

    myLocation = ofxGeo::ElevatedCoordinate(51.507406923983446,
                                            -0.12773752212524414,
                                            0.05);

    ofHttpResponse response = ofLoadURL("http://www.celestrak.com/NORAD/elements/resource.txt");

    if (200 == response.status)
    {
        satellites = ofx::Satellite::Utils::loadTLEFromBuffer(response.data);
    }
    else
    {
        ofLogError("ofApp::setup()") << "Unable to load : " << response.error;
    }


    cam.setPosition(0, 0, 0);

}


void ofApp::draw()
{
    ofBackground(0);

    cam.begin();

    ofPushMatrix();
    //rot += 1;
    //ofRotate(rot, 0, 1, 0);
    ofScale(scaler, scaler, scaler);

    ofSetColor(255);
    colorMap.bind();
    earthSphere.draw();
    colorMap.unbind();

    ofQuaternion latRot;
    ofQuaternion longRot;

    auto iter = satellites.begin();

    ofx::Geo::ElevatedCoordinate pos;

    Poco::DateTime now;

    while (iter != satellites.end())
    {
//        // Only show two satellites to keep things simple.
//        if (iter->Name().compare("AQUA") != 0 && iter->Name().compare("TERRA") != 0)
//        {
//            ++iter;
//            continue;
//        }

        try
        {
            pos = ofxSatellite::Utils::toElevatedCoordinate((*iter).find(now).ToGeodetic());
        }
        catch (...)
        {
            // If there is an exception, skip it.
            ++iter;
            continue;
        }

        ofQuaternion latRot;
        ofQuaternion longRot;

        latRot.makeRotate(pos.getLatitude(), 1, 0, 0);
        longRot.makeRotate(pos.getLongitude(), 0, 1, 0);

        //our starting point is 0,0, on the surface of our sphere, this is where the meridian and equator meet
        ofVec3f center = ofVec3f(0,0, pos.getElevation() / 1000 + ofx::Geo::GeoUtils::EARTH_RADIUS_KM);
        //multiplying a quat with another quat combines their rotations into one quat
        //multiplying a quat to a vector applies the quat's rotation to that vector
        //so to to generate our point on the sphere, multiply all of our quaternions together then multiple the centery by the combined rotation
        ofVec3f worldPoint = latRot * longRot * center;

        ofNoFill();
        ofSetColor(255, 0, 0, 255);
        ofDrawSphere(worldPoint, 30);

        ofSetColor(255);

//        // Set the bitmap text mode billboard so the points show up correctly in 3d.
//        std::stringstream ss;
//
//        ss << iter->Name() << std::endl;
//        ss << " Latitude (deg): " << pos.getLatitude() << std::endl;
//        ss << "Longitude (deg): " << pos.getLongitude() << std::endl;
//        ss << " Elevation (km): " << pos.getElevation() / 1000 << std::endl;
//
//        ofDrawBitmapString(ss.str(), worldPoint);

        ++iter;
    }

    ofPopMatrix();

    cam.end();

}

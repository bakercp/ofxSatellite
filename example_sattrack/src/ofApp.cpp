// =============================================================================
//
// Copyright (c) 2010-2014 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"


void ofApp::setup()
{
    scaler = 300 / Geo::Utils::EARTH_RADIUS_KM;

	ofSetFrameRate(30);
	ofEnableAlphaBlending();
	ofNoFill();

    myLocation = Geo::ElevatedCoordinate(51.507406923983446,
                                         -0.12773752212524414,
                                         0.05);

    Tle tle = Tle("UK-DMC 2                ",
                  "1 35683U 09041C   12289.23158813  .00000484  00000-0  89219-4 0  5863",
                  "2 35683  98.0221 185.3682 0001499 100.5295 259.6088 14.69819587172294");

    SGP4 sgp4(tle);

    satellites.push_back(sgp4);

}


void ofApp::update()
{
}


void ofApp::draw()
{
	ofBackground(0);

	ofPushMatrix();

	//translate so that the center of the screen is 0,0
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    ofScale(scaler, scaler, scaler);

	ofSetColor(255, 255, 255, 20);

	//draw a translucent wireframe sphere (ofNoFill() is on)
	ofPushMatrix();


	//add an extra spin at the rate of 1 degree per frame
	ofRotate(ofGetFrameNum(), 0, 1, 0);
	ofDrawSphere(0, 0, 0, Geo::Utils::EARTH_RADIUS_KM);
	ofPopMatrix();

	ofSetColor(255);

    std::vector<SGP4>::const_iterator iter = satellites.begin();

    Poco::DateTime now;

    while (iter != satellites.end())
    {
        Geo::ElevatedCoordinate pos = Satellite::Utils::getPosition(*iter, now);

		//three rotations
		//two to represent the latitude and lontitude of the city
		//a third so that it spins along with the spinning sphere
		ofQuaternion latRot;
        ofQuaternion longRot;
        ofQuaternion spinQuat;

		latRot.makeRotate(pos.getLatitude(), 1, 0, 0);
		longRot.makeRotate(pos.getLongitude(), 0, 1, 0);
		spinQuat.makeRotate(ofGetFrameNum(), 0, 1, 0);

		//our starting point is 0,0, on the surface of our sphere, this is where the meridian and equator meet
		ofVec3f center = ofVec3f(0,0, pos.getElevation() / 1000 + Geo::Utils::EARTH_RADIUS_KM);
		//multiplying a quat with another quat combines their rotations into one quat
		//multiplying a quat to a vector applies the quat's rotation to that vector
		//so to to generate our point on the sphere, multiply all of our quaternions together then multiple the centery by the combined rotation
		ofVec3f worldPoint = latRot * longRot * spinQuat * center;

		//draw it and label it
		ofLine(ofVec3f(0,0,0), worldPoint);

		//set the bitmap text mode billboard so the points show up correctly in 3d
		ofDrawBitmapString("Sat", worldPoint );

        ++iter;
    }



	ofPopMatrix();

}


void ofApp::keyPressed(int key)
{
}

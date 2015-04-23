// =============================================================================
//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
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
    CoordGeodetic geo(51.507406923983446, -0.12773752212524414, 0.05);
    Tle tle("GALILEO-PFM (GSAT0101)  ",
            "1 37846U 11060A   12293.53312491  .00000049  00000-0  00000-0 0  1435",
            "2 37846  54.7963 119.5777 0000994 319.0618  40.9779  1.70474628  6204");
    SGP4 sgp4(tle);

    std::cout << tle << std::endl;

    /*
     * generate 7 day schedule
     */
    DateTime start_date = DateTime::Now(true);
    DateTime end_date(start_date.AddDays(7.0));

    std::vector<ofx::Satellite::Utils::PassDetails> pass_list;

    std::cout << "Start time: " << start_date << std::endl;
    std::cout << "End time  : " << end_date << std::endl << std::endl;

    /*
     * generate passes
     */
    pass_list = ofx::Satellite::Utils::GeneratePassList(geo, sgp4, start_date, end_date, 180);

    if (pass_list.begin() == pass_list.end())
    {
        std::cout << "No passes found" << std::endl;
    }
    else
    {
        std::stringstream ss;

        ss << std::right << std::setprecision(1) << std::fixed;

        std::vector<ofx::Satellite::Utils::PassDetails>::const_iterator itr = pass_list.begin();
        do
        {
            ss  << "AOS: " << itr->aos
            << ", LOS: " << itr->los
            << ", Max El: " << std::setw(4) << Util::RadiansToDegrees(itr->max_elevation)
            << ", Duration: " << (itr->los - itr->aos)
            << std::endl;
        }
        while (++itr != pass_list.end());

        std::cout << ss.str();
    }
}


void ofApp::draw()
{
    ofBackground(255);
    ofDrawBitmapStringHighlight("See the Console", 10, 42);
}


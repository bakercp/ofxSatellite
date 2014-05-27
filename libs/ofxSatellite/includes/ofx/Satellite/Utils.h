// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
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


#pragma once


#include <vector>
#include "SGP4.h"
#include "CoordTopocentric.h"
#include "CoordGeodetic.h"
#include "Observer.h"
#include "Util.h"
#include "ofxTime.h"
#include "ofxGeo.h"


namespace ofx {
namespace Satellite {


class Satellite;


/// \brief A collection of utilities for satellite tasks.
class Utils
{
public:
    struct PassDetails
    {
        DateTime aos;
        DateTime los;
        double max_elevation;
    };

    static std::vector<Satellite> loadTLEFromFile(const std::string& filename);

    static std::vector<Satellite> loadTLEFromBuffer(const ofBuffer& buffer);

    static Geo::ElevatedCoordinate getPosition(const SGP4& satellite,
                                               const Poco::DateTime& time);

    static DateTime toDateTime(const Poco::DateTime& time);

    static Geo::ElevatedCoordinate toElevatedCoordinate(const CoordGeodetic& coord);

    static Observer toObserver(const Geo::ElevatedCoordinate& coordinate0);

    static double FindMaxElevation(const CoordGeodetic& user_geo,
                                   SGP4& sgp4,
                                   const DateTime& aos,
                                   const DateTime& los);

    static DateTime FindCrossingPoint(const CoordGeodetic& user_geo,
                                      SGP4& sgp4,
                                      const DateTime& initial_time1,
                                      const DateTime& initial_time2,
                                      bool finding_aos);


    static std::vector<PassDetails> GeneratePassList(const CoordGeodetic& user_geo,
                                                     SGP4& sgp4,
                                                     const DateTime& start_time,
                                                     const DateTime& end_time,
                                                     const int time_step);


};

} } // namespace ofx::Satellite

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


#include "ofx/Satellite/Utils.h"
#include "ofx/Satellite/Satellite.h"
#include "Poco/String.h"


namespace ofx {
namespace Satellite {


std::vector<Satellite> Utils::loadTLEFromFile(const std::string& filename)
{
    return loadTLEFromBuffer(ofBufferFromFile(filename));
}


std::vector<Satellite> Utils::loadTLEFromBuffer(const ofBuffer& buffer)
{
    std::vector<Satellite> satellites;

    ofBuffer buf(buffer);

    std::string currentLine = "";

    std::string tleName = "";
    std::string tleLineOne = "";
    std::string tleLineTwo = "";

    buf.resetLineReader();

    bool first = true;

    while (!buf.isLastLine())
    {
        currentLine = buf.getNextLine();

        Poco::trimInPlace(currentLine);

        std::cout << currentLine << std::endl;

        if (currentLine.empty())
        {
            continue;
        }
        else if (currentLine[0] == '1')
        {
            tleLineOne = currentLine;
        }
        else if (currentLine[0] == '2')
        {
            tleLineTwo = currentLine;
        }
        else
        {
            if (!tleLineOne.empty() && !tleLineTwo.empty())
            {
                satellites.push_back(Satellite(tleName, tleLineOne, tleLineTwo));
            }
            else if (first)
            {
                first = false;
            }
            else
            {
                ofLogWarning("Utils::loadTLEFromBuffer") << "Incomplete TLE";
            }

            tleName = currentLine;
            tleLineOne = "";
            tleLineTwo = "";
        }
    }

    return satellites;
}


DateTime Utils::toDateTime(const Poco::DateTime& time)
{
    DateTime dt;

    dt.Initialise(time.year(),
                  time.month(),
                  time.day(),
                  time.hour(),
                  time.minute(),
                  time.second(),
                  time.millisecond() * 1000 + time.microsecond());

    return dt;
}


Geo::ElevatedCoordinate Utils::getPosition(const SGP4& satellite,
                                           const Poco::DateTime& time)
{

    DateTime now = toDateTime(time);

    Eci eci = satellite.FindPosition(now);

    CoordGeodetic geo = eci.ToGeodetic();

    return toElevatedCoordinate(geo);
}


Geo::ElevatedCoordinate Utils::toElevatedCoordinate(const CoordGeodetic& coord)
{
    return Geo::ElevatedCoordinate(Util::RadiansToDegrees(coord.latitude),
                                   Util::RadiansToDegrees(coord.longitude),
                                   coord.altitude * 1000);
}


Observer Utils::toObserver(const Geo::ElevatedCoordinate& coordinate)
{
    return Observer(coordinate.getLatitude(),
                    coordinate.getLongitude(),
                    coordinate.getElevation() / 1000);
}


double Utils::FindMaxElevation(const CoordGeodetic& user_geo,
                               SGP4& sgp4,
                               const DateTime& aos,
                               const DateTime& los)
{
    Observer obs(user_geo);

    bool running;

    double time_step = (los - aos).TotalSeconds() / 9.0;
    DateTime current_time(aos); //! current time
    DateTime time1(aos); //! start time of search period
    DateTime time2(los); //! end time of search period
    double max_elevation; //! max elevation

    running = true;

    do
    {
        running = true;
        max_elevation = -99999999999999.0;
        while (running && current_time < time2)
        {
            /*
             * find position
             */
            Eci eci = sgp4.FindPosition(current_time);
            CoordTopocentric topo = obs.GetLookAngle(eci);

            if (topo.elevation > max_elevation)
            {
                /*
                 * still going up
                 */
                max_elevation = topo.elevation;
                /*
                 * move time along
                 */
                current_time = current_time.AddSeconds(time_step);
                if (current_time > time2)
                {
                    /*
                     * dont go past end time
                     */
                    current_time = time2;
                }
            }
            else
            {
                /*
                 * stop
                 */
                running = false;
            }
        }

        /*
         * make start time to 2 time steps back
         */
        time1 = current_time.AddSeconds(-2.0 * time_step);
        /*
         * make end time to current time
         */
        time2 = current_time;
        /*
         * current time to start time
         */
        current_time = time1;
        /*
         * recalculate time step
         */
        time_step = (time2 - time1).TotalSeconds() / 9.0;
    }
    while (time_step > 1.0);

    return max_elevation;
}


DateTime Utils::FindCrossingPoint(const CoordGeodetic& user_geo,
                                  SGP4& sgp4,
                                  const DateTime& initial_time1,
                                  const DateTime& initial_time2,
                                  bool finding_aos)
{
    Observer obs(user_geo);

    bool running;
    int cnt;

    DateTime time1(initial_time1);
    DateTime time2(initial_time2);
    DateTime middle_time;

    running = true;
    cnt = 0;
    while (running && cnt++ < 16)
    {
        middle_time = time1.AddSeconds((time2 - time1).TotalSeconds() / 2.0);
        /*
         * calculate satellite position
         */
        Eci eci = sgp4.FindPosition(middle_time);
        CoordTopocentric topo = obs.GetLookAngle(eci);

        if (topo.elevation > 0.0)
        {
            /*
             * satellite above horizon
             */
            if (finding_aos)
            {
                time2 = middle_time;
            }
            else
            {
                time1 = middle_time;
            }
        }
        else
        {
            if (finding_aos)
            {
                time1 = middle_time;
            }
            else
            {
                time2 = middle_time;
            }
        }
        
        if ((time2 - time1).TotalSeconds() < 1.0)
        {
            /*
             * two times are within a second, stop
             */
            running = false;
            /*
             * remove microseconds
             */
            int us = middle_time.Microsecond();
            middle_time = middle_time.AddMicroseconds(-us);
            /*
             * step back into the pass by 1 second
             */
            middle_time = middle_time.AddSeconds(finding_aos ? 1 : -1);
        }
    }
    
    /*
     * go back/forward 1second until below the horizon
     */
    running = true;
    cnt = 0;
    while (running && cnt++ < 6)
    {
        Eci eci = sgp4.FindPosition(middle_time);
        CoordTopocentric topo = obs.GetLookAngle(eci);
        if (topo.elevation > 0)
        {
            middle_time = middle_time.AddSeconds(finding_aos ? -1 : 1);
        }
        else
        {
            running = false;
        }
    }
    
    return middle_time;
}


std::vector<Utils::PassDetails> Utils::GeneratePassList(const CoordGeodetic& user_geo,
                                                        SGP4& sgp4,
                                                        const DateTime& start_time,
                                                        const DateTime& end_time,
                                                        const int time_step)
{
    std::vector<Utils::PassDetails> pass_list;

    Observer obs(user_geo);

    DateTime aos_time;
    DateTime los_time;

    bool found_aos = false;

    DateTime previous_time(start_time);
    DateTime current_time(start_time);

    while (current_time < end_time)
    {
        bool end_of_pass = false;

        /*
         * calculate satellite position
         */
        Eci eci = sgp4.FindPosition(current_time);
        CoordTopocentric topo = obs.GetLookAngle(eci);

        if (!found_aos && topo.elevation > 0.0)
        {
            /*
             * aos hasnt occured yet, but the satellite is now above horizon
             * this must have occured within the last time_step
             */
            if (start_time == current_time)
            {
                /*
                 * satellite was already above the horizon at the start,
                 * so use the start time
                 */
                aos_time = start_time;
            }
            else
            {
                /*
                 * find the point at which the satellite crossed the horizon
                 */
                aos_time = FindCrossingPoint(user_geo,
                                             sgp4,
                                             previous_time,
                                             current_time,
                                             true);
            }
            found_aos = true;
        }
        else if (found_aos && topo.elevation < 0.0)
        {
            found_aos = false;
            /*
             * end of pass, so move along more than time_step
             */
            end_of_pass = true;
            /*
             * already have the aos, but now the satellite is below the horizon,
             * so find the los
             */
            los_time = FindCrossingPoint(user_geo,
                                         sgp4,
                                         previous_time,
                                         current_time,
                                         false);

            struct PassDetails pd;
            pd.aos = aos_time;
            pd.los = los_time;
            pd.max_elevation = FindMaxElevation(user_geo,
                                                sgp4,
                                                aos_time,
                                                los_time);

            pass_list.push_back(pd);
        }

        /*
         * save current time
         */
        previous_time = current_time;

        if (end_of_pass)
        {
            /*
             * at the end of the pass move the time along by 30mins
             */
            current_time = current_time + TimeSpan(0, 30, 0);
        }
        else
        {
            /*
             * move the time along by the time step value
             */
            current_time = current_time + TimeSpan(0, 0, time_step);
        }
        
        if (current_time > end_time)
        {
            /*
             * dont go past end time
             */
            current_time = end_time;
        }
    };
    
    if (found_aos)
    {
        /*
         * satellite still above horizon at end of search period, so use end
         * time as los
         */
        PassDetails pd;
        pd.aos = aos_time;
        pd.los = end_time;
        pd.max_elevation = FindMaxElevation(user_geo, sgp4, aos_time, end_time);
        
        pass_list.push_back(pd);
    }
    
    return pass_list;
}

    
} } // namespace ofx::Satellite

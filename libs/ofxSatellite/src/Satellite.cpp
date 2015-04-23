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


#include "ofx/Satellite/Satellite.h"
#include "ofx/Satellite/Utils.h"


namespace ofx {
namespace Satellite {


Satellite::Satellite(const std::string& lineOne,
                     const std::string& lineTwo):
    Tle(lineOne, lineTwo),
    _sgp4(*this)
{
}


Satellite::Satellite(const std::string& name,
                     const std::string& lineOne,
                     const std::string& lineTwo):
    Tle(name, lineOne, lineTwo),
    _sgp4(*this)
{
}


Satellite::Satellite(const Tle& tle):
    Tle(tle),
    _sgp4(*this)
{
}


Satellite::~Satellite()
{
}


Eci Satellite::find(const DateTime& date) const
{
    return _sgp4.FindPosition(date);
}


Eci Satellite::find(const Poco::DateTime& date) const
{
    return _sgp4.FindPosition(Utils::toDateTime(date));
}

    
} } // namespace ofx::Satellite

//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


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

//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include <vector>
#include "SGP4.h"
#include "CoordTopocentric.h"
#include "CoordGeodetic.h"
#include "Observer.h"
#include "Tle.h"
#include "Util.h"
#include "Poco/DateTime.h"


namespace ofx {
namespace Satellite {


class Satellite: public Tle
{
public:
    Satellite(const std::string& lineOne,
              const std::string& lineTwo);

    Satellite(const std::string& name,
              const std::string& lineOne,
              const std::string& lineTwo);

    Satellite(const Tle& tle);

    virtual ~Satellite();

    Eci find(const DateTime& date) const;

    Eci find(const Poco::DateTime& time) const;


protected:
    SGP4 _sgp4;
    
};


} } // namespace ofx::Satellite

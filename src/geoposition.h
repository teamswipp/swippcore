// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GEOPOSITION_H_
#define GEOPOSITION_H_

#define _USE_MATH_DEFINES
#include <cmath>

class GeoPosition
{
private:
    double longitude;
    double latitude;

    double deg2rad(double deg) { return deg * M_PI / 180.0; }
    double circumference(double radius) { return 2 * M_PI * radius; }
public:
    double vincentyDistance(GeoPosition destination);
    double vincentyDistanceNormalized(double distance);
    double vincentyDistanceNormalized(GeoPosition destination);
};

#endif /* GEOPOSITION_H_ */

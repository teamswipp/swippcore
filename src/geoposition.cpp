// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <complex>

#include "geoposition.h"

static const double EARTH_RADIUS_KM = 6378.0;

double GeoPosition::vincentyDistance(GeoPosition destination)
{
    double lat1 = deg2rad(latitude);
    double lon1 = deg2rad(longitude);
    double lat2 = deg2rad(destination.latitude);
    double lon2 = deg2rad(destination.longitude);

    double distance_lon = abs(lon1 - lon2);

    double a = pow(cos(lat2) * sin(distance_lon), 2);
    double b = cos(lat1) * sin(lat2);
    double c = sin(lat1) * cos(lat2) * cos(distance_lon);
    double d = pow(b - c, 2);
    double e = sin(lat1) * sin(lat2);
    double f = cos(lat1) * cos(lat2) * cos(distance_lon);

    return EARTH_RADIUS_KM * atan2(sqrt(a + d), e + f);
}

double GeoPosition::vincentyDistanceNormalized(double distance)
{
    return distance / circumference(EARTH_RADIUS_KM);
}

double GeoPosition::vincentyDistanceNormalized(GeoPosition destination)
{
    double distance = GeoPosition::vincentyDistance(destination);
    return GeoPosition::vincentyDistanceNormalized(distance);
}

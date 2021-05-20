#include "MathUtils.h"

double rad2deg(double radians)
{
    return radians * 180. / M_PI;
}

double deg2rad(double degrees)
{
    return degrees * M_PI / 180.;
}

double m2geo(double meters, double latitude)
{
    // Earth mean radius
    const double r = 6371032.;
    return meters / (fabs(cos(latitude)) * r);
}
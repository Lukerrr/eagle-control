#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

template <typename T>
int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
T clamp(T val, T min, T max)
{
    if (val < min) val = min;
    else if (val > max) val = max;
    return val;
}

template <typename T>
T equal(T val1, T val2, T tol)
{
    T diff = val1 - val2;
    if (diff < 0.f) diff *= -1;
    return diff - tol <= 0.f;
}

double rad2deg(double radians);
double deg2rad(double degrees);

// Convert meters to geographical angle in radians
double m2geo(double meters, double latitude);
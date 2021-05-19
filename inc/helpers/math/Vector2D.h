#pragma once

#include "MathUtils.h"

struct Vec2
{
    double x,y;
    
    Vec2() : Vec2(0.) {};
    Vec2(double xy) : x(xy), y(xy) {};
    Vec2(double _x, double _y) : x(_x), y(_y) {};

    double GetLength()
    {
        return sqrtf(x * x + y * y);
    }

    double GetDistance(Vec2 v)
    {
        return (*this - v).GetLength();
    }

    void Normalize()
    {
        double len = GetLength();
        if (len != 1.)
        {
            if (equal(len, 0., 0.00001))
            {
                x = y = 0.;
            }
            else
            {
                x /= len;
                y /= len;
            }
        }
    }

    Vec2 GetNormalized()
    {
        double len = GetLength();
        if (equal(len, 0., 0.00001))
        {
            return Vec2(0., 0.);
        }
        else
        {
            return Vec2(x / len, y / len);
        }
    }

    double DotProduct(Vec2 other)
    {
        return x * other.x + y * other.y;
    }

    Vec2 operator-() { return Vec2(-x, -y); }

    Vec2 operator+(Vec2 other) { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(Vec2 other) { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(double val) { return Vec2(x * val, y * val); }
    Vec2 operator/(double val) { return Vec2(x / val, y / val); }

    void operator+=(Vec2 other) { x = x + other.x; y = y + other.y; }
    void operator-=(Vec2 other) { x = x - other.x; y = y - other.y; }
    void operator*=(double val) { x = x * val; y = y * val; }
    void operator/=(double val) { x = x / val; y = y / val; }

    bool operator==(Vec2 other) { return x == other.x && y == other.y; }
    bool operator!=(Vec2 other) { return x != other.x || y != other.y; }

    void operator= (Vec2 other) { x = other.x; y = other.y; }
};
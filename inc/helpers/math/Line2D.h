#pragma once

#include "Vector2D.h"

/*
* A line between two points in 2D dimention
*/
struct Line2D
{
private:
    /*
    * Line equation coefficients:
    *   Ax + By = C
    */
    double _A, _B, _C;
    Vec2 _pt[2];

public:
    Line2D() {}
    Line2D(Vec2 pt1, Vec2 pt2)
    {
        _pt[0] = pt1;
        _pt[1] = pt2;
        Recalculate();
    }

    Vec2 GetNormal()
    {
        return Vec2(_A, _B).GetNormalized();
    }

    void Shift(Vec2 delta)
    {
        _pt[0] += delta;
        _pt[1] += delta;
        Recalculate();
    }

    bool GetCrossPoint(Line2D line, Vec2& outPt)
    {
        // Get crossing point M using Cramer's rule
        double delta = _A * line.B() - _B * line.A();

        if(delta == 0)
        {
            // No solution
            return false;
        }

        double delta1 = _C * line.B() - line.C() * _B;
        double delta2 = _A * line.C() - line.A() * _C;
        outPt.x = delta1 / delta;
        outPt.y = delta2 / delta;

        return true;
    }

    bool Intersects(Line2D line, bool bCheckSegment = true)
    {
        // Checking intersection between AB (this line) and CD (other line)
        Vec2 m;
        if(!GetCrossPoint(line, m))
        {
            return false;
        }

        if(!bCheckSegment)
        {
            return true;
        }

        // Get AM, BM, CM, DM vectors to check whether AB and CD contain point M
        Vec2 am = m - _pt[0];
        Vec2 bm = m - _pt[1];
        Vec2 cm = m - line.P1();
        Vec2 dm = m - line.P2();

        // Calculate cosinus of angles AM^BM and CM^DM
        double cosAmBm = am.DotProduct(bm);
        double cosCmDm = cm.DotProduct(dm);

        // Check angles to be 180 deg. (cos = -1)
        if(cosAmBm < 0. && cosCmDm < 0.)
        {
            // AB intersects CD
            return true;
        }
        
        return false;
    }

    double GetDistanceToPoint(Vec2 c, bool bCheckSegment = true)
    {
        double dist = -1.f;

        // Get distance from point C: {x, y} to the line (or segment P1P2)

        Vec2 p1p2 = P2() - P1();
        Vec2 p1c = c - P1();
        Vec2 p2c = c - P2();

        // Lengths
        double lP1P2 = p1p2.GetLength();
        double lP1C = p1c.GetLength();
        double lP2C = p2c.GetLength();
        if(bCheckSegment)
        {
            // Check is the point C out of segment's bounds
            if (p1p2.DotProduct(p1c) < 0.)
            {
                // cos(P1P2 ^ P1C) < 0
                dist = lP1C;
            }
            else if((-p1p2).DotProduct(p2c) < 0.)
            {
                // cos(P2P1 ^ P2C) < 0
                dist = lP2C;
            }
        }

        if(dist == -1.f)
        {
            // Find triangle height
            double p = (lP1P2 + lP1C + lP2C) / 2.;
            dist = 2. * sqrt(p * (p - lP1P2) * (p - lP1C) * (p - lP2C)) / lP1P2;
        }

        return dist;
    }

    void SetP1(Vec2 pt)
    {
        _pt[0] = pt;
        Recalculate();
    }

    void SetP2(Vec2 pt)
    {
        _pt[1] = pt;
        Recalculate();
    }

    inline Vec2 P1() { return _pt[0]; }
    inline Vec2 P2() { return _pt[1]; }
    inline double A() { return _A; }
    inline double B() { return _B; }
    inline double C() { return _C; }

private:
    void Recalculate()
    {
        _A = _pt[1].y - _pt[0].y;
        _B = _pt[0].x - _pt[1].x;
        _C = _A * _pt[0].x + _B * _pt[0].y;
    }
};
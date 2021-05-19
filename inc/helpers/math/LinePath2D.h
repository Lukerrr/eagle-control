#pragma once

#include "Line2D.h"
#include <vector>

using namespace std;

// Looped path in 2D-space
class CLinePath2D
{
public:
    CLinePath2D();
    CLinePath2D(vector<Vec2> points);

    void AddPoint(Vec2 pt);
    int InsertPoint(Vec2 pt);
    void ReplacePoint(int idx, Vec2 pt);
    void RemovePoint(int idx);
    void Append(CLinePath2D path);
    void Clear();

    vector<Vec2> GetPointsCloud(Vec2 min, Vec2 max, double spacing);

    double GetLength();
    bool IntersectsSelf();

    int GetSize();
    Vec2 GetPoint(int idx);
    vector<Vec2> GetPoints();

    vector<Line2D> GetLines();

private:
    vector<Vec2> m_points;
};
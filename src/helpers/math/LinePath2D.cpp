#include "LinePath2D.h"

using namespace std;

CLinePath2D::CLinePath2D()
{
}

CLinePath2D::CLinePath2D(vector<Vec2> points)
    : m_points(points)
{
}

int CLinePath2D::InsertPoint(Vec2 pt)
{
    vector<Line2D> lines = GetLines();

    // Find the nearest line to insert the point
    int linesNum = lines.size();
    int insertId = -1;
    double minDist = -1.;

    for(int i = 0; i < linesNum; ++i)
    {
        double dist = lines[i].GetDistanceToPoint(pt);

        if(minDist == -1. || dist < minDist)
        {
            insertId = i + 1;
            minDist = dist;
        }
    }

    if(insertId == -1)
    {
        insertId = GetSize();
        m_points.push_back(pt);
    }
    else
    {
        // Insert into m_points[insertId]
        vector<Vec2>::iterator it = m_points.begin();
        advance(it, insertId);
        m_points.insert(it, pt);
    }

    return insertId;
}

void CLinePath2D::AddPoint(Vec2 pt)
{
    m_points.push_back(pt);
}

void CLinePath2D::Append(CLinePath2D path)
{
    vector<Vec2> newPoints = path.GetPoints();
    m_points.insert(m_points.end(), newPoints.begin(), newPoints.end());
}

void CLinePath2D::RemovePoint(int idx)
{
    if(idx >= 0 && idx < GetSize())
    {
        m_points.erase(m_points.begin() + idx);
    }
}

void CLinePath2D::ReplacePoint(int idx, Vec2 pt)
{
    if(idx >= 0 && idx < GetSize())
    {
        m_points[idx] = pt;
    }
}

void CLinePath2D::Clear()
{
    m_points.clear();
}

double CLinePath2D::GetLength()
{
    double length = 0.;
    for(Line2D line : GetLines())
    {
        length += (line.P2() - line.P1()).GetLength();
    }
    return length;
}

vector<Vec2> CLinePath2D::GetPointsCloud(Vec2 min, Vec2 max, double spacing)
{
    vector<Vec2> cloud;
    Vec2 pt = min;

    double ySpacing = spacing;
    for(; pt.x <= max.x; pt.x += spacing)
    {
        char pointsAdded = 0;
        for(; pt.y >= min.y && pt.y <= max.y; pt.y += ySpacing)
        {
            Line2D checkLine(pt, max * 2);
            int intersections = 0;
            for(Line2D line : GetLines())
            {
                if(line.Intersects(checkLine))
                {
                    ++intersections;
                }
            }

            if(intersections % 2 != 0)
            {
                if(pointsAdded < 2)
                {
                    cloud.push_back(pt);
                    ++pointsAdded;
                }
                else
                {
                    cloud.back() = pt;
                }
            }
        }
        pt.y -= ySpacing;
        ySpacing *= -1.;
    }

    return cloud;
}

bool CLinePath2D::IntersectsSelf()
{
    vector<Line2D> lines = GetLines();
    int size = lines.size();

    for(int i = 0; i < size; ++i)
    {
        // Don't check previous lines, this line and the next line
        // Don't check the last line if line 'i' is the first one (i == 0)
        int jLast = i == 0 ? size - 1 : size;
        for(int j = i + 2; j < jLast; ++j)
        {
            if(lines[i].Intersects(lines[j]))
            {
                return true;
            }
        }
    }

    return false;
}

Vec2 CLinePath2D::GetPoint(int idx)
{
    return m_points[idx];
}

vector<Vec2> CLinePath2D::GetPoints()
{
    return m_points;
}

int CLinePath2D::GetSize()
{
    return m_points.size();
}

vector<Line2D> CLinePath2D::GetLines()
{
    vector<Line2D> lines;

    size_t pointsNum = GetSize();
    if(pointsNum > 1)
    {
        for(int i = 1; i < pointsNum; ++i)
        {
            lines.push_back(Line2D(m_points[i-1], m_points[i]));
        }

        if(pointsNum > 2)
        {
            lines.push_back(Line2D(m_points[pointsNum-1], m_points[0]));
        }
    }

    return lines;
}
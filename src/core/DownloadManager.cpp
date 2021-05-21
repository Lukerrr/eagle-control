#include "DownloadManager.h"
#include "Log.h"

#include <fstream>

using namespace std;

CDownloadManager::CDownloadManager(int cloudSize, string outFileName)
{
    m_cloudSize = cloudSize;
    m_outFileName = outFileName;
}

CDownloadManager::~CDownloadManager()
{
    if(m_cloud.empty())
    {
        CLog::Log(LOG_WARNING, "CDownloadManager: closed with empty point cloud");
        return;
    }

    CLog::Log(LOG_INFO, "CDownloadManager: saving %d points into '%s'...", m_cloud.size(), m_outFileName.c_str());

    ofstream file(m_outFileName);

    if(!file.is_open())
    {
        CLog::Log(LOG_WARNING, "CDownloadManager: cannot open file '%s'!", m_outFileName.c_str());
        return;
    }

    for(SPointCloud::CloudPt pt : m_cloud)
    {
        file << pt.x << ";" << pt.y << ";" << pt.z << "\n";
    }

    CLog::Log(LOG_INFO, "CDownloadManager: saving is done!");
}

void CDownloadManager::AppendChunk(SPointCloud chunk)
{
    int i = 0;
    for(; m_cloud.size() <= m_cloudSize && i < chunk.cloudSize; ++i)
    {
        m_cloud.push_back(chunk.cloud[i]);
    }
    CLog::Log(LOG_INFO, "CDownloadManager: wrote a chunk with %d points | %d/%d (%d%)",
        chunk.cloudSize, m_cloud.size(), m_cloudSize, GetPercent());
}

int CDownloadManager::GetPercent()
{
    int curSize = m_cloud.size();
    if(curSize >= m_cloudSize)
    {
        return 100;
    }

    return int((float(m_cloud.size()) / float(m_cloudSize)) * 100.f);
}
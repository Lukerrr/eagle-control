#pragma once

#include "RspMsgs.h"

#include <string>
#include <vector>

class CDownloadManager
{
public:
    CDownloadManager(int cloudSize, std::string outFileName);
    ~CDownloadManager();

    void AppendChunk(SPointCloud chunk);
    int GetPercent();

private:
    int m_cloudSize;
    std::string m_outFileName;
    std::vector<SPointCloud::CloudPt> m_cloud;
};
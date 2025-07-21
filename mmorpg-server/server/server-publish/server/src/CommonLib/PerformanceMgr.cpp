#include "PerformanceMgr.h"

PerformanceMgr::PerformanceMgr()
{
    session_ = 0;
}

PerformanceMgr::~PerformanceMgr()
{

}

void PerformanceMgr::AddPerf(const char* name, UInt64 usedTime)
{
    perfRecords_[name].usedTime = usedTime;
    perfRecords_[name].session = session_;
}

void PerformanceMgr::Reset()
{
    GenSession();
}

void PerformanceMgr::ShowPerf()
{
    for (auto itr : perfRecords_)
    {
        if (itr.second.session != session_)
        {
            continue;
        }

        InfoLogStream << itr.first << " used time " << itr.second.usedTime << LogStream::eos;
    }
}

void PerformanceMgr::GenSession()
{
    session_++;
}


HashMap<std::string, UInt64> PerformanceCountMgr::m_CountMap;

PerformanceCountMgr::PerformanceCountMgr(std::string moduleName)
{
	m_ModuleName = moduleName;
	if (m_CountMap.find(m_ModuleName) == m_CountMap.end())
	{
		m_CountMap[m_ModuleName] = 0;
	}
}

PerformanceCountMgr::~PerformanceCountMgr()
{
	++m_CountMap[m_ModuleName];
}

void PerformanceCountMgr::CleanCount(std::string moduleName)
{
	m_CountMap[moduleName] = 0;
}

void PerformanceCountMgr::ShowCount(std::string moduleName)
{
	InfoLogStream << "PerformanceCountMgr::ShowCount:" << moduleName << " Count:" << m_CountMap[moduleName] << LogStream::eos;
}

void PerformanceCountMgr::SetCount(std::string moduleName, UInt64 count)
{
	m_CountMap[moduleName] = count;
}

UInt64 PerformanceCountMgr::GetCount(std::string moduleName)
{
	if (m_CountMap.find(moduleName) == m_CountMap.end())
	{
		return 0;
	}
	return m_CountMap[moduleName];
}

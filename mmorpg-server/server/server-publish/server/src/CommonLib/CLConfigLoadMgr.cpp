#include "CLConfigLoadMgr.h"

CLConfigLoadMgr::CLConfigLoadMgr()
{

}

CLConfigLoadMgr::~CLConfigLoadMgr()
{

}

void CLConfigLoadMgr::LoadNeedReloadTables(const std::string& fileName)
{
	m_NeedReloadTableVec.clear();
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(fileName, strVec, "|");
	for (auto& it : strVec)
	{
		m_NeedReloadTableVec.push_back(it);
	}
}

void CLConfigLoadMgr::ReloadDataTables(const std::string& path)
{
	if (m_NeedReloadTableVec.empty())
	{
		for (auto& it : m_ReloadDataFunMap)
		{
			ReloadTableFun tableFn = it.second.tableFn;
			if (NULL == tableFn)
				continue;

			if (tableFn(path + it.first))
			{
				for (auto& iter : it.second.extraFnVec)
				{
					iter();
				}
			}
		}
	}
	else
	{
		for (auto& it : m_NeedReloadTableVec)
		{
			auto iter = m_ReloadDataFunMap.find(it);
			if (iter == m_ReloadDataFunMap.end())
			{
				ErrorLogStream << "config reload failed " << it << " not register!" << LogStream::eos;
				continue;
			}

			ReloadTableFun tableFn = iter->second.tableFn;
			if (NULL == tableFn)
				continue;

			if (tableFn(path + iter->first))
			{
				for (auto& tt : iter->second.extraFnVec)
				{
					tt();
				}
			}
		}
	}
}

void CLConfigLoadMgr::RegisterReloadDataTableFun(const std::string& file, ReloadTableFun funPtr)
{
	ReloadDataFunMap::iterator iter = m_ReloadDataFunMap.find(file);
	if (iter != m_ReloadDataFunMap.end())
	{
		iter->second = ReloadFunData(funPtr);
		return;
	}

	m_ReloadDataFunMap.insert(ReloadDataFunMap::value_type(file, ReloadFunData(funPtr))).second;
}

void CLConfigLoadMgr::RegisterReloadDataExtraFun(const std::string& file, ReloadExtraFun funPtr)
{
	ReloadDataFunMap::iterator iter = m_ReloadDataFunMap.find(file);
	if (iter == m_ReloadDataFunMap.end())
	{
		ErrorLogStream << "register extra reload fail! not find register file" << file << LogStream::eos;
		return;
	}

	iter->second.extraFnVec.push_back(funPtr);
}
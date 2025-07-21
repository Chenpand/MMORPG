#include "LoginPushMgr.h"

#include <TimeUtil.h>
#include "WSPlayer.h"
#include "MallMgr.h"


LoginPushMgr::LoginPushMgr()
{

}

LoginPushMgr::~LoginPushMgr()
{
	for (auto ptr : m_LoginPushInfos)
	{
		if (ptr)
		{
			CL_SAFE_DELETE(ptr);
		}
	}

	m_LoginPushInfos.clear();
}

void LoginPushMgr::Init()
{
	LoadTable();
}

void LoginPushMgr::LoadTable(bool isReload)
{
	class LoginPushDataVistor : public Avalon::DataEntryVisitor<LoginPushDataEntry>
	{
	public:
		bool operator()(LoginPushDataEntry* dataEntry)
		{
			if (dataEntry != NULL)
			{
				loginPushDatas.push_back(dataEntry);
			}

			return true;
		}

		std::vector<LoginPushDataEntry*> loginPushDatas;
	};

	LoginPushDataVistor vistor;
	LoginPushDataEntryMgr::Instance()->Visit(vistor);

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto dataEntry : vistor.loginPushDatas)
	{
		if (!dataEntry) continue;

		LoginPushInfo* info = new LoginPushInfo;
		if (!info)
		{
			ErrorLogStream << "LoginPushInfo ptr is null!" << LogStream::eos;
			continue;
		}

		info->id = dataEntry->id;
		info->name = dataEntry->name;
		info->unlockLevel = dataEntry->unlockLevel;
		info->iconPath = dataEntry->iconPath;
		info->linkInfo = dataEntry->linkInfo;
		info->startTime = dataEntry->startTime;
		info->endTime = dataEntry->endTime;
		info->loadingIconPath = dataEntry->loadingIconPath;
		info->sortNum = dataEntry->sortNum;
		info->openInterval = dataEntry->openInterval;
		info->closeInterval = dataEntry->closeInterval;
		info->isShowTime = dataEntry->isShowTime;
		info->isSetNative = dataEntry->isSetNative;

		if (dataEntry->daysAfterService > 0)
		{
			UInt64 startTime = (UInt64)Avalon::Time::MSECS_OF_SEC * (GAMESTART_TIME + (dataEntry->daysAfterService - 1) * Avalon::Time::SECS_OF_DAY);
			Avalon::Date date = Avalon::Date(Avalon::Time(startTime));
			date.Hour(0);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);

			info->startTime = (UInt32)date.ToTime().Sec();
			info->endTime = info->startTime + dataEntry->continueDays * Avalon::Time::SECS_OF_DAY - 1;
		}

		if (info->endTime > 0 && info->endTime < curTime)
		{
			CL_SAFE_DELETE(info);
			continue;
		}

		if (isReload)
		{
			m_TempLoginPushInfos.push_back(info);
		}
		else
		{
			m_LoginPushInfos.push_back(info);
		}

		MallMgr::Instance()->SetLoginPushBindGoodsInfo(info);

		InfoLogStream << "Set login push(id: " << dataEntry->id << ", daysAfterService: " << dataEntry->daysAfterService << "), continueDays: " << dataEntry->continueDays << ") start time(" << TimeUtil::TimestampToStandardFormat(info->startTime) 
			<< ") end time(" << TimeUtil::TimestampToStandardFormat(info->endTime) << "), game start time(" << GAMESTART_TIME << ")." << LogStream::eos;
	}
}

void LoginPushMgr::ReloadDataTable()
{
	InfoLogStream << "Start reloading..." << LogStream::eos;

	LoadTable(true);

	if (m_TempLoginPushInfos.empty())
	{
		ErrorLogStream << "Reload datas are empty!" << LogStream::eos;
		return;
	}

	for (auto info : m_LoginPushInfos)
	{
		if (!info) continue;

		LoginPushInfo* tempInfo = NULL;
		auto tempIter = m_TempLoginPushInfos.begin();
		for (; tempIter != m_TempLoginPushInfos.end(); ++tempIter)
		{
			auto elem = *tempIter;
			if (!elem) continue;

			if (elem->id == info->id)
			{
				tempInfo = elem;
				break;
			}
		}

		if (tempInfo != NULL)
		{
			*info = *tempInfo;

			CL_SAFE_DELETE(tempInfo);
			m_TempLoginPushInfos.erase(tempIter);
		}
		else
		{
			// 现有内存中的数据在重载后的表格中找不到，则将之关闭
			info->endTime = info->startTime + 1;
			InfoLogStream << "OpActivity(" << info->id << ") will close!" << LogStream::eos;

			MallMgr::Instance()->SetLoginPushBindGoodsInfo(info);
		}
	}

	// 说明有新加的
	if (!m_TempLoginPushInfos.empty())
	{
		InfoLogStream << "Add the new OpActivities!" << LogStream::eos;

		m_LoginPushInfos.insert(m_LoginPushInfos.end(), m_TempLoginPushInfos.begin(), m_TempLoginPushInfos.end());
	}

	m_TempLoginPushInfos.clear();
}

void LoginPushMgr::AddLoginPushInfo(LoginPushInfo* info)
{
	if (!info)
	{
		ErrorLogStream << "LoginPushInfo ptr is null!" << LogStream::eos;
		return;
	}

	auto oldInfo = FindLoginPushInfo(info->id);
	if (oldInfo)
	{
		ErrorLogStream << "Repeated login push info(id: " << info->id << ")." << LogStream::eos;
		return;
	}

	m_LoginPushInfos.push_back(info);
}

void LoginPushMgr::GetLoginPushInfos(std::vector<LoginPushInfo>& infos)
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto info : m_LoginPushInfos)
	{
		if (!info)
		{
			ErrorLogStream << "LoginPushInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if ((info->startTime > 0 && curTime < info->startTime) ||
			(info->endTime > 0 && curTime > info->endTime))
		{
			continue;
		}

		infos.push_back(*info);
	}
}

LoginPushInfo* LoginPushMgr::FindLoginPushInfo(UInt8 id)
{
	for (auto info : m_LoginPushInfos)
	{
		if (!info)
		{
			ErrorLogStream << "LoginPushInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->id == id)
		{
			return info;
		}
	}

	return NULL;
}
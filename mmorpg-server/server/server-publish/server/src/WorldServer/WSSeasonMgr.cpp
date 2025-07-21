#include "WSSeasonMgr.h"
#include "GameParamMgr.h"
#include "WSNetwork.h"
#include "WSRouter.h"
#include "WSSortListMgr.h"

#include <CLSeasonDataEntry.h>
#include <CLMatchProtocol.h>


bool WSSeasonMgr::Init()
{
	_InitSeasonChangeAttrTime();
	_InitSeasonStatus();

	// 初始化后再同步一下赛季信息
	_SyncToScene();

	return true;
}

void WSSeasonMgr::OnTick(Avalon::Time now)
{
	//赛季属性发放

	bool isChangeSeasonAttr = false;
	if (now.Sec() >= m_SeasonAttrEndTime.Sec())
	{
		m_SeasonChangeAttrTime = m_SeasonAttrEndTime;
		_SaveSeasonChangeAttrTime();
		_ResetSeasonAttrTime();
		_GetSeasonSortListPlayerIds();
		isChangeSeasonAttr = true;
	}

	bool isChangeSeasonId = false;
	if (now.Sec() >= m_SeasonEndTime.Sec())
	{
		m_Id = m_Id + 1;
		m_StartTime = m_SeasonEndTime;
		_SaveSeasonStatus();
		_ResetSeasonTime();
		isChangeSeasonId = true;
	}

	if (isChangeSeasonId)
	{
		_ChangeSeason();
	}
	else if (isChangeSeasonAttr)
	{
		_SyncToScene();
	}
}

void WSSeasonMgr::SetSeasonId(UInt32 id)
{
	m_Id = id;
	_SaveSeasonStatus();
	_ChangeSeason();
}


void WSSeasonMgr::_InitSeasonChangeAttrTime()
{
	m_SeasonChangeAttrTime = (UInt64)GameParamMgr::Instance()->GetValue(SEASON_CHANGE_ATTR_TIME) * (UInt64)Avalon::Time::MSECS_OF_SEC;

	_ResetSeasonAttrTime();
	if (m_SeasonChangeAttrTime < m_SeasonAttrStartTime)
	{
		m_SeasonChangeAttrTime = m_SeasonAttrStartTime;
		_SaveSeasonChangeAttrTime();
		_GetSeasonSortListPlayerIds();
		_SyncToScene();
	}
}

void WSSeasonMgr::_SaveSeasonChangeAttrTime()
{
	GameParamMgr::Instance()->SetValue(SEASON_CHANGE_ATTR_TIME, (UInt32)m_SeasonChangeAttrTime.Sec());
}

void WSSeasonMgr::_GetSeasonSortListPlayerIds()
{
	m_SeasonSortListPlayerIds = WSSortListMgr::Instance()->GetSeasonPlayerIds(10);
}

void WSSeasonMgr::_ClearSeasonSortListPlayerIds()
{
	m_SeasonSortListPlayerIds.clear();
}

void WSSeasonMgr::_InitSeasonStatus()
{
	m_Id = GameParamMgr::Instance()->GetValue(SEASON_ID_KEY);
	UInt32 startTime = GameParamMgr::Instance()->GetValue(SEASON_START_TIME_KEY);
	m_StartTime = Avalon::Time((UInt64)startTime * Avalon::Time::MSECS_OF_SEC);

	_ResetSeasonTime();
	if (m_StartTime.Sec() < m_SeasonStartTime.Sec())
	{
		m_Id = m_Id + 1;
		m_StartTime = m_SeasonStartTime;
		_SaveSeasonStatus();
		_ChangeSeason();
	}
}

void WSSeasonMgr::_SaveSeasonStatus()
{
	GameParamMgr::Instance()->SetValue(SEASON_ID_KEY, m_Id);
	GameParamMgr::Instance()->SetValue(SEASON_START_TIME_KEY, (UInt32)m_StartTime.Sec());
}

void WSSeasonMgr::_ResetSeasonTime()
{
	Avalon::Date nowDate = Avalon::Date(Avalon::Time::CurrentTime());
	UInt64 seasonStartTime = SeasonDataEntryMgr::Instance()->GetNowSeasonTime(SeasonEventType::SEASON_EVENT_TYPE_SEASON, nowDate);
	m_SeasonStartTime = Avalon::Time(seasonStartTime);

	UInt64 seasonEndTime = SeasonDataEntryMgr::Instance()->GetNextSeasonTime(SeasonEventType::SEASON_EVENT_TYPE_SEASON, nowDate);
	m_SeasonEndTime = Avalon::Time(seasonEndTime);
}

void WSSeasonMgr::_ResetSeasonAttrTime()
{
	Avalon::Date nowDate = Avalon::Date(Avalon::Time::CurrentTime());
	UInt64 seasonAttrStartTime = SeasonDataEntryMgr::Instance()->GetNowSeasonTime(SeasonEventType::SEASON_EVENT_TYPE_SEASON_ATTR, nowDate);
	m_SeasonAttrStartTime = Avalon::Time(seasonAttrStartTime);

	UInt64 seasonAttrEndTime = SeasonDataEntryMgr::Instance()->GetNextSeasonTime(SeasonEventType::SEASON_EVENT_TYPE_SEASON_ATTR, nowDate);
	m_SeasonAttrEndTime = Avalon::Time(seasonAttrEndTime);
}

void WSSeasonMgr::_ChangeSeason()
{
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_GUIJIANSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_JIANHUN);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_KUANGZHANSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_GUIQI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_AXIULUO);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_SHENQIANGSHOU);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_MANYOU);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_QIANGPAOSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_JIXIESHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_DANYAOZHUANJIA);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_MOFASHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_YUANSUSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_ZHANDOUFASHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_ZHAOHUANSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_GEDOUJIA);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_QIGONGSHI);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_SANDA);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_JIEBA);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_ROUDAOJIA);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_SHENQIANGSHOU_NV);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_MANYOU_NV);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_QIANGPAOSHI_NV);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_JIXIESHI_NV);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_DANYAOZHUANJIA_NV);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_SHENGZHIZHE);
	WSSortListMgr::Instance()->ClearSortList(SortListType::SORTLIST_1V1_SEASON_OCCU_QUMO);

	_SyncToScene();
}

void WSSeasonMgr::_SyncToScene()
{
	CLProtocol::SceneNotifySeason protocol;
	protocol.seasonId = m_Id;
	protocol.startTime = (UInt32)m_SeasonStartTime.Sec();
	protocol.endTime = (UInt32)m_SeasonEndTime.Sec();
	protocol.seasonAttrStartTime = (UInt32)m_SeasonAttrStartTime.Sec();
	protocol.seasonAttrEndTime = (UInt32)m_SeasonAttrEndTime.Sec();
	protocol.seasonSortListPlayerIds = m_SeasonSortListPlayerIds;
	Router::BroadcastToScene(protocol);
}

void WSSeasonMgr::OnSceneConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL) return;

	CLProtocol::SceneNotifySeason notiy;
	notiy.seasonId = m_Id;
	notiy.startTime = (UInt32)m_SeasonStartTime.Sec();
	notiy.endTime = (UInt32)m_SeasonEndTime.Sec();
	notiy.seasonAttrStartTime = (UInt32)m_SeasonAttrStartTime.Sec();
	notiy.seasonAttrEndTime = (UInt32)m_SeasonAttrEndTime.Sec();
	notiy.seasonSortListPlayerIds = m_SeasonSortListPlayerIds;
	CL_SEND_PROTOCOL(conn, notiy);

}


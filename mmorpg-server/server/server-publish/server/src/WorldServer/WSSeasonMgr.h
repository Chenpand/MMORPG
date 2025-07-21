#ifndef __SEASON_MGR_H__
#define __SEASON_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include "CLSeasonDefine.h"

class WSSeasonMgr : public Avalon::Singleton<WSSeasonMgr>
{
public:
	WSSeasonMgr(){}
	~WSSeasonMgr(){}

	bool Init();

public:
	/**
	*@brief Tick
	*/
	void OnTick(Avalon::Time now);

	/**
	*@brief 设置赛季
	*/
	void SetSeasonId(UInt32 id);

private:

	/**
	*@brief 初始化属性变化时间
	*/
	void _InitSeasonChangeAttrTime();

	/**
	*@brief 保存属性变化时间
	*/
	void _SaveSeasonChangeAttrTime();

	/**
	*@brief 获取排行榜玩家id
	*/
	void _GetSeasonSortListPlayerIds();

	/**
	*@brief 获取排行榜玩家id
	*/
	void _ClearSeasonSortListPlayerIds();

	/**
	*@brief 初始化赛季状态
	*/
	void _InitSeasonStatus();

	/**
	*@brief 保存赛季状态
	*/
	void _SaveSeasonStatus();

	/**
	*@brief 初始化赛季时间
	*/
	void _ResetSeasonTime();

	/**
	*@brief 初始化赛季属性时间
	*/
	void _ResetSeasonAttrTime();

	/**
	*@brief 改变赛季
	*/
	void _ChangeSeason();

	/**
	*@brief 同步到scene
	*/
	void _SyncToScene();

public:
	void OnSceneConnected(UInt32 id);

private:
	UInt32			m_Id;

	Avalon::Time	m_StartTime;

	Avalon::Time	m_SeasonStartTime;

	Avalon::Time	m_SeasonEndTime;

	//改变属性时间 
	Avalon::Time	m_SeasonChangeAttrTime;

	Avalon::Time	m_SeasonAttrStartTime;

	Avalon::Time	m_SeasonAttrEndTime;

	

	//赛季排行榜玩家ID
	std::vector<UInt64>	m_SeasonSortListPlayerIds;

};



#endif  //__SEASON_MGR_H__
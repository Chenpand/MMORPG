#ifndef __REPLAY_MGR_H__
#define __REPLAY_MGR_H__

#include <memory> 
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include "ReplayList.h"

class ReplayMgr : public Avalon::Singleton<ReplayMgr>
{
public:
	ReplayMgr();
	~ReplayMgr();

	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief 结束
	*/
	void Final();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

public:
	/**
	*@brief 加载录像列表
	*/
	void LoadReplayList(ReplayListType type, ObjID_t ownerid, ReplayListCallback* cb);

	/**
	*@brief 获取录像列表
	*/
	ReplayList* FindReplayList(ReplayListType type, ObjID_t ownerid);

	/**
	*@brief 添加录像列表
	*/
	void AddReplayList(ReplayList* list);

	/**
	*@brief 删除录像列表
	*/
	void DelayDelReplayList(ReplayListType type, ObjID_t id, UInt64 delaySec = 10 * Avalon::Time::SECS_OF_MIN);
	void DelReplayList(ReplayListType type, ObjID_t id);

public:
	/**
	*@brief 获取录像信息
	*/
	ReplayInfoPtr FindReplayInfo(ObjID_t raceid);

	/**
	*@brief 获取录像信息
	*/
	bool AddReplayInfo(ReplayInfoPtr info);

	/**
	*@brief 插入录像到数据库
	*/
	void InsertReplayInfoToDB(ReplayInfoPtr info);

	/**
	*@brief 更新录像数据到数据库
	*/
	void UpdateReplayInfoToDB(ReplayInfoPtr info);

	/**
	*@brief 查找录像文件
	*/
	ReplayFile* FindReplayFile(ObjID_t raceid);

	/**
	*@brief 删除录像文件
	*/
	void DelReplayFile(ObjID_t raceid);

	/**
	*@brief 插入录像文件到数据库
	*/
	bool InsertReplayFileToDB(ReplayFile* replay);

public:
	/**
	*@brief 能否加入高手对决二级列表中
	*/
	bool CanJoinMasterWaitList(UInt32 score);

	/**
	*@brief 加入到高手对决二级列表中
	*/
	bool AddToMasterWaitList(ReplayInfoPtr info, ReplayFile* file);

	/**
	*@brief 加入到赏金联赛淘汰赛列表中
	*/
	bool AddToPremiumLeagueBattleList(ReplayInfoPtr info, ReplayFile* file);

	/**
	*@brief 把二级队列放到一级队列里
	*/
	void UpgradeMasterWaitList();

public:
	// 录像缓存
	/**
	*@brief 加入到录像缓存
	*/
	void AddToReplayBuffer(ObjID_t raceid, UInt8* buffer);

	/**
	*@brief 加入到录像缓存
	*/
	UInt8* GetReplayBuffer(ObjID_t raceid);

	/**
	*@brief 删除录像缓存
	*/
	void DelReplayBuffer(ObjID_t raceid);

private:
	HashMap<ObjID_t, ReplayList*>		m_ReplayLists[REPLAY_LIST_NUM];
	std::map<ObjID_t, ReplayInfoPtr>	m_ReplayInfoes;
	std::map<ObjID_t, ReplayFile*>		m_ReplayFiles;
	std::map<ObjID_t, UInt8*>			m_ReplayBuffer;

	Avalon::SimpleTimer					m_MasterListTimer;
	Avalon::SimpleTimer					m_ReplayInfoTickTimer;
	Avalon::SimpleTimer					m_ReplayListTickTimer;
};

#endif
#ifndef _WS_PLAYER_SHARE_DATA_MGR_H_
#define _WS_PLAYER_SHARE_DATA_MGR_H_

#include <CLObjectIndex.h>

#include "WSPlayerShareData.h"

class WSShareDataLoadCallback;

class WSPlayerShareDataMgr : public Avalon::Singleton<WSPlayerShareDataMgr>
{
public:
	WSPlayerShareDataMgr();
	~WSPlayerShareDataMgr();

public:
	/**
	*@brief 添加一个玩家
	*/
	bool AddPlayer(WSPlayerShareData *player);

	/**
	*@brief 判断缓存的玩家数量，如果超过一定值，就尝试删除部分玩家
	*/
	void TryRelasePlayerOfflineData();

	/**
	*@brief 查找一个玩家
	*/
	WSPlayerShareData* FindPlayer(ObjID_t id);

	/**
	*@brief 查找一个玩家，没有则穿件
	*/
	WSPlayerShareData* FindOrCreatePlayer(ObjID_t id);

	/**
	*@brief 离线加载玩家数据
	*/ 
	void LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, WSShareDataLoadCallback* cb);

	/**
	*@brief 加载玩家任务数据
	*/
	void OnLoadPlayerTaskInfo(CLRecordCallback* callback);
	
	/**
	*@brief 玩家下线
	*/
	void OnOffline(WSPlayer* player);
public:
	/**
	*@brief 处理玩家离线同步请求
	*/
	void HandleScenePlayerSyncShareData(ObjID_t guid, const PlayerSenceShareData& sharedata);

	/**
	*@brief 处理玩家玩家间师徒关系解除或出师
	*/
	void HandleRelieveMaster(ObjID_t discipleId);

private:
	//id玩家索引
	CLIDObjIndex<WSPlayerShareData>	m_Players;
};

#endif
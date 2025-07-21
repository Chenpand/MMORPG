#ifndef _EXPEDITION_MGR_H_
#define _EXPEDITION_MGR_H_

#include <functional>
#include <CLDefine.h>

class CLRecordCallback;
class WSPlayer;
class ExpeditionMap;
class ExpeditionMember;


class ExpeditionMgr
{
public:
	ExpeditionMgr();
	~ExpeditionMgr();

public:
	/**
	*@brief 加载远征地图
	*/
	void LoadExpeditionMapCallback(CLRecordCallback* callback);

	/**
	*@brief 查找远征地图
	*/
	ExpeditionMap* FindExpeditionMap(UInt8 mapId);

	/**
	*@brief 添加远征地图
	*/
	void AddExpeditionMap(ExpeditionMap* map);

	/**
	*@brief 移除远征地图
	*/
	bool RemoveExpeditionMap(UInt8 mapId);

	/**
	*@brief 访问远征地图
	*/
	void VisitExpeditionMaps(const std::function<bool(ExpeditionMap*)>& F);

public:
	/**
	*@brief 加载远征队成员
	*/
	void LoadExpeditionMemberCallback(CLRecordCallback* callback);

	/*
	*@brief 查找远征队成员
	*/
	ExpeditionMember* FindExpeditionMember(ObjID_t roleid);

	/*
	*@brief 添加远征队成员
	*/
	void AddExpeditionMember(ExpeditionMember* member);

	/*
	*@brief 访问远征队成员
	*/
	void VisitExpeditionMember(UInt8 mapid, const std::function<bool(ExpeditionMember*)>& F);

	/*
	*@brief 清空远征队成员
	*/
	void ClearExpeditionMember(UInt8 mapid);

	/*
	*@brief 获取远征队成员数
	*/
	UInt16 GetExpeditionMemberNum(UInt8 mapid);

public:
	void OnTick(const Avalon::Time& now);

	void OnPlayerOnline(WSPlayer* player);

public:
	/**
	*@brief 玩家查询远征地图
	*/
	void OnPlayerQueryExpeditionMap(WSPlayer* player, UInt8 mapId);

	/**
	*@brief 玩家查询可选远征角色
	*/
	void OnPlayerQueryOptionalExpeditionRoles(WSPlayer* player);

	/**
	*@brief 玩家派遣远征队
	*/
	void OnPlayerDispatchExpeditionTeam(WSPlayer* player, UInt8 mapId, const std::vector<ObjID_t>& members, UInt32 duration);

	/**
	*@brief 玩家取消远征
	*/
	void OnPlayerCancelExpedition(WSPlayer* player, UInt8 mapId);

	/**
	*@brief 玩家获取远征奖励
	*/
	void OnPlayerGetExpeditionRewards(WSPlayer* player, UInt8 mapId);

	/**
	*@brief 玩家查询全部远征地图
	*/
	void OnPlayerQueryAllExpeditionMaps(WSPlayer* player, const std::vector<UInt8>& mapIds);

private:
	/**
	*@brief 同步全部远征地图
	*/
	void _SyncAllExpeditionMaps();

private:
	// 远征地图集
	std::vector<ExpeditionMap*> m_ExpeditionMaps;

	// 远征队成员
	std::vector<ExpeditionMember*> m_ExpeditionMembers;
};

#endif
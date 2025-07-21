#ifndef __SCENE_CHAMPION_MGR_H__
#define __SCENE_CHAMPION_MGR_H__

#include <functional>
#include <set>

#include<AvalonSingleton.h>
#include <CLChampionDefine.h>
#include <CLChampionProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <AvalonTimer.h>

class Player;
using ChampionActFun = std::function<void(Player*, UInt64)>;

class SceneChampionMgr : public Avalon::Singleton<SceneChampionMgr>
{
public:
	/**
	*@brief 初始化
	*/
	bool Init();
	/**
	*@brief 设置冠军赛当前状态
	*/
	void SetStatus(const ChampionStatusInfo& status);
	/**
	*@brief 获取冠军赛当前状态
	*/
	const ChampionStatusInfo& GetStatus() const;
	/**
	*@brief 开始冠军赛动作
	*/
	void BeginAct(ChampionAct act, Player* player, UInt64 param = 0);
	/**
	*@brief Scene报名动作
	*/
	void Enroll(Player* player, UInt64 param);
	/**
	*@brief 收到Union确认报名成功
	*/
	void OnEnrollSuccess(Player* player);
	/**
	*@brief 复活动作
	*/
	void Relive(Player* player, UInt64 param);
	/**
	*@brief 检查复活动作
	*/
	void CheckRelive(Player* player, UInt64 param);

	/**
	*@brief 进入比赛动作
	*/
	void JoinRace(Player* player, UInt64 param);

	/**
	*@brief 查询自己冠军赛属性动作
	*/
	void QuerySelfAttributes(Player* player, UInt64 param);

	/**
	*@brief 观战动作
	*/
	void ObserveRace(Player* player, UInt64 raceID);

	/**
	*@brief 加入冠军赛场景成功
	*/
	void OnPlayerJoinRaceSuccess(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief 加入冠军赛场景失败
	*/
	void OnPlayerJoinRaceFail(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief 从冠军赛服务器断线,判断是否有资格调用应该放在外层逻辑
	*/
	void OnPlayerDisconnect(Player* player);


	/**
	*@brief 玩家下注
	*/
	void OnPlayerBet(Player* player, CLProtocol::SceneChampionGambleReq& req);
	/**
	*@brief 收到Union下注返回
	*/
	void OnPlayerBetRet(CLProtocol::SceneChampionGambleUnionRes& req);

	/**
	*@brief 判断玩家是否在冠军赛场景
	*/
	bool IsPlayerInChampion(const UInt64 roleId);

	/**
	*@brief 玩家进入冠军赛场景
	*/
	void OnPlayerEnterChampionScene(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief 玩家离开冠军赛场景
	*/
	void OnPlayerLeaveChampionScene(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief 收到Union同步16强表
	*/
	void OnUnionSyncTop16Table(std::vector<ChampionTop16Player>& vec);

	/**
	*@brief 收到Union同步竞猜信息
	*/
	void OnUnionSyncGambleInfo(std::vector<ChampionGambleInfo>& vec);

	/**
	*@brief 收到Union同步战斗记录，弃用
	*/
	void OnUnionSyncBattleRecord(Avalon::Packet* packet);

	/**
	*@brief 收到Union同步所有组的信息
	*/
	void OnUnionSyncAllGroupInfo(CLProtocol::SceneChampionAllGroupRes& req);

	/**
	*@brief 收到Union同步1组的信息
	*/
	void OnUnionSyncOneGroupInfo(CLProtocol::SceneChampionGroupStatusSync& sync);

	/**
	*@brief 广播一组信息
	*/
	void BroadcastOneGroupInfo(ChampionGroupStatus& group);

	/**
	*@brief 广播所有组信息
	*/
	void BroadcastAllGroupInfo();

	/**
	*@brief 获取16强表
	*/
	std::vector<ChampionTop16Player>& GetTop16Table();

	/**
	*@brief 进入场景时
	*/
	void OnEnterScene(Player * player);

	/**
	*@brief 获取所有组信息
	*/
	std::map<UInt32, ChampionGroupStatus>& GetAllGroupInfo();

	/**
	*@brief 获取所有竞猜项目信息
	*/
	std::vector<ChampionGambleInfo>& GetAllGambleInfo();

	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(Player* player);

	/**
	*@brief 玩家离线
	*/
	void OnPlayerOffline(Player* player);
private:
	bool GetChampionTopPlayerByPos(UInt32 pos, ChampionTop16Player& outPlayer);
private:
	//当前状态
	ChampionStatusInfo m_Status;
	//状态动作Map
	std::map<ChampionStatus, std::map<ChampionAct, ChampionActFun>> m_StatusActMap;
	//在冠军赛场景中的玩家
	std::set<UInt64> m_PlayeInChampion;
	//16强表
	std::vector<ChampionTop16Player> m_Top16PlayerVec;
	//押注信息
	std::vector<ChampionGambleInfo> m_GambleInfoVec;
	//组id->比赛顺序order->比赛
	std::map<UInt32, std::map<UInt32, ChampionRaceData>> m_BattleRecordMap;
	//组id->组信息，发给客户端用
	std::map<UInt32, ChampionGroupStatus> m_GroupStatus;
	//公告Handler
	Avalon::TimerHandler* m_SendAnnouncementTimer = nullptr;
};



#endif // !__SCENE_CHAMPION_MGR_H__






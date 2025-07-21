#ifndef _CHALLENGE_MGR_H_
#define _CHALLENGE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>

#include "GameZoneMgr.h"
#include "Challenger.h"

class ChallengeScene;

/**
 *@brief 挑战赛管理器                                                                     
 */
class ChallengeMgr : public Avalon::Singleton<ChallengeMgr>
{
public:
	//一轮的时长
	const static UInt32	ROUND_LASTTIME = 3 * 60;
	//报名时长
	const static UInt32 ROUND_SIGNIN_TIME = 60;
	//分组最小人数
	const static UInt32	MIN_GROUP_PLAYER_NUM = 100;
	//跨服挑战场景id
	const static UInt32	WORLD_CHALLENGE_SCENEID = 703;
	//跨服排行榜
	const static UInt32	WORLD_CHALLENGE_SORTLIST_NUM = 10000;

	typedef std::vector<ChallengeScene*>	SceneVec;
	typedef std::vector<std::pair<Challenger*, Challenger*> > ChallengePairVec;
	typedef std::vector<UInt32>				SceneServerVec;
	typedef std::vector<Challenger*>		ChallengerVec;	

public:
	ChallengeMgr();
	~ChallengeMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 报名和取消报名
	 */
	void Signin(GameZone* zone, ObjID_t id, const std::string& name, UInt32 gateId, UInt16 zoneId, UInt16 level, UInt8 occu, UInt32 power, const UInt8* pkdata, size_t pkdatalen);
	void Signout(GameZone* zone, ObjID_t id);

	/**
	 *@brief 发送挑战赛信息                                                                     
	 */
	void SendInfo(GameZone* zone, ObjID_t id, UInt16 level);

	/**
	 *@brief 请求排名数据                                                                     
	 */
	void SendSortList(GameZone* zone, ObjID_t playerId, UInt8 group, UInt16	start, UInt16 num);

	/**
	 *@brief 设置挑战赛结果                                                                     
	 */
	void SetChallengeResult(ObjID_t playerId, UInt8 result, UInt32 score);

	/**
	 *@brief 领取奖励                                                                     
	 */
	void GetAward(GameZone* zone, ObjID_t playerId, UInt8 type, UInt8 index, UInt32 minVal);

public:
	/**
	 *@brief 事件                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 场景创建                                                                     
	 */
	void OnSceneCreated(UInt32 nodeId, UInt32 sceneId);

	/**
	 *@brief 场景销毁                                                                     
	 */
	void OnSceneDestroyed(UInt32 sceneId);

	/**
	 *@brief 赛季开始                                                                     
	 */
	void OnSeasonBegin();

	/**
	 *@brief 赛季结束                                                                     
	 */
	void OnSeasonEnd();

	/**
	 *@brief 场景服务器连接建立                                                                     
	 */
	void OnSceneServerConnected(UInt32 nodeId);

	/**
	 *@brief 场景服务器链接断开                                                                     
	 */
	void OnSceneServerDisconnected(UInt32 nodeId);

private:
	void InitRoundTime(const Avalon::Time& now);
	void BeginRound();
	UInt32 SelectSceneServer();
	void CreateScene();
	void UpdateSortList(Challenger* challenger);
	UInt16 GetRanking(ObjID_t id);
	void BeginBattle(ChallengeScene* scene, Challenger* challenger1, Challenger* challenger2);

private:
	//挑战者列表
	CLIDObjIndex<Challenger>	m_Challengers;
	//排名列表
	std::map<UInt8, ChallengerVec>	m_SortList;
	//上周名次
	std::map<ObjID_t, UInt32>	m_Rankings;

	//场景列表
	SceneVec					m_Scenes;

	//场景服务器列表
	SceneServerVec				m_SceneServers;

	//报名列表
	std::vector<ObjID_t>		m_SigninList;

	//配对列表
	ChallengePairVec			m_Pairs;

	//本轮开始时间
	UInt32						m_RoundTime;
	//本轮状态(0报名阶段 1战斗阶段)
	UInt8						m_RoundStatus;

	//上一个场景服务器
	UInt32						m_SceneServer;

	//创建场景定时器
	Avalon::SimpleTimer			m_CreateSceneTimer;
};

#endif

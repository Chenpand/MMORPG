#ifndef _CHALLENGE_MGR_H_
#define _CHALLENGE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>

#include "GameZoneMgr.h"
#include "Challenger.h"

class ChallengeScene;

/**
 *@brief ��ս��������                                                                     
 */
class ChallengeMgr : public Avalon::Singleton<ChallengeMgr>
{
public:
	//һ�ֵ�ʱ��
	const static UInt32	ROUND_LASTTIME = 3 * 60;
	//����ʱ��
	const static UInt32 ROUND_SIGNIN_TIME = 60;
	//������С����
	const static UInt32	MIN_GROUP_PLAYER_NUM = 100;
	//�����ս����id
	const static UInt32	WORLD_CHALLENGE_SCENEID = 703;
	//������а�
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
	 *@brief ������ȡ������
	 */
	void Signin(GameZone* zone, ObjID_t id, const std::string& name, UInt32 gateId, UInt16 zoneId, UInt16 level, UInt8 occu, UInt32 power, const UInt8* pkdata, size_t pkdatalen);
	void Signout(GameZone* zone, ObjID_t id);

	/**
	 *@brief ������ս����Ϣ                                                                     
	 */
	void SendInfo(GameZone* zone, ObjID_t id, UInt16 level);

	/**
	 *@brief ������������                                                                     
	 */
	void SendSortList(GameZone* zone, ObjID_t playerId, UInt8 group, UInt16	start, UInt16 num);

	/**
	 *@brief ������ս�����                                                                     
	 */
	void SetChallengeResult(ObjID_t playerId, UInt8 result, UInt32 score);

	/**
	 *@brief ��ȡ����                                                                     
	 */
	void GetAward(GameZone* zone, ObjID_t playerId, UInt8 type, UInt8 index, UInt32 minVal);

public:
	/**
	 *@brief �¼�                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ��������                                                                     
	 */
	void OnSceneCreated(UInt32 nodeId, UInt32 sceneId);

	/**
	 *@brief ��������                                                                     
	 */
	void OnSceneDestroyed(UInt32 sceneId);

	/**
	 *@brief ������ʼ                                                                     
	 */
	void OnSeasonBegin();

	/**
	 *@brief ��������                                                                     
	 */
	void OnSeasonEnd();

	/**
	 *@brief �������������ӽ���                                                                     
	 */
	void OnSceneServerConnected(UInt32 nodeId);

	/**
	 *@brief �������������ӶϿ�                                                                     
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
	//��ս���б�
	CLIDObjIndex<Challenger>	m_Challengers;
	//�����б�
	std::map<UInt8, ChallengerVec>	m_SortList;
	//��������
	std::map<ObjID_t, UInt32>	m_Rankings;

	//�����б�
	SceneVec					m_Scenes;

	//�����������б�
	SceneServerVec				m_SceneServers;

	//�����б�
	std::vector<ObjID_t>		m_SigninList;

	//����б�
	ChallengePairVec			m_Pairs;

	//���ֿ�ʼʱ��
	UInt32						m_RoundTime;
	//����״̬(0�����׶� 1ս���׶�)
	UInt8						m_RoundStatus;

	//��һ������������
	UInt32						m_SceneServer;

	//����������ʱ��
	Avalon::SimpleTimer			m_CreateSceneTimer;
};

#endif

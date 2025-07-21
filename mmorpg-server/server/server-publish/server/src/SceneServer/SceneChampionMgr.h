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
	*@brief ��ʼ��
	*/
	bool Init();
	/**
	*@brief ���ùھ�����ǰ״̬
	*/
	void SetStatus(const ChampionStatusInfo& status);
	/**
	*@brief ��ȡ�ھ�����ǰ״̬
	*/
	const ChampionStatusInfo& GetStatus() const;
	/**
	*@brief ��ʼ�ھ�������
	*/
	void BeginAct(ChampionAct act, Player* player, UInt64 param = 0);
	/**
	*@brief Scene��������
	*/
	void Enroll(Player* player, UInt64 param);
	/**
	*@brief �յ�Unionȷ�ϱ����ɹ�
	*/
	void OnEnrollSuccess(Player* player);
	/**
	*@brief �����
	*/
	void Relive(Player* player, UInt64 param);
	/**
	*@brief ��鸴���
	*/
	void CheckRelive(Player* player, UInt64 param);

	/**
	*@brief �����������
	*/
	void JoinRace(Player* player, UInt64 param);

	/**
	*@brief ��ѯ�Լ��ھ������Զ���
	*/
	void QuerySelfAttributes(Player* player, UInt64 param);

	/**
	*@brief ��ս����
	*/
	void ObserveRace(Player* player, UInt64 raceID);

	/**
	*@brief ����ھ��������ɹ�
	*/
	void OnPlayerJoinRaceSuccess(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief ����ھ�������ʧ��
	*/
	void OnPlayerJoinRaceFail(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief �ӹھ�������������,�ж��Ƿ����ʸ����Ӧ�÷�������߼�
	*/
	void OnPlayerDisconnect(Player* player);


	/**
	*@brief �����ע
	*/
	void OnPlayerBet(Player* player, CLProtocol::SceneChampionGambleReq& req);
	/**
	*@brief �յ�Union��ע����
	*/
	void OnPlayerBetRet(CLProtocol::SceneChampionGambleUnionRes& req);

	/**
	*@brief �ж�����Ƿ��ڹھ�������
	*/
	bool IsPlayerInChampion(const UInt64 roleId);

	/**
	*@brief ��ҽ���ھ�������
	*/
	void OnPlayerEnterChampionScene(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief ����뿪�ھ�������
	*/
	void OnPlayerLeaveChampionScene(Player* player, UInt32 dstSceneId, UInt32 dstDoorId);

	/**
	*@brief �յ�Unionͬ��16ǿ��
	*/
	void OnUnionSyncTop16Table(std::vector<ChampionTop16Player>& vec);

	/**
	*@brief �յ�Unionͬ��������Ϣ
	*/
	void OnUnionSyncGambleInfo(std::vector<ChampionGambleInfo>& vec);

	/**
	*@brief �յ�Unionͬ��ս����¼������
	*/
	void OnUnionSyncBattleRecord(Avalon::Packet* packet);

	/**
	*@brief �յ�Unionͬ�����������Ϣ
	*/
	void OnUnionSyncAllGroupInfo(CLProtocol::SceneChampionAllGroupRes& req);

	/**
	*@brief �յ�Unionͬ��1�����Ϣ
	*/
	void OnUnionSyncOneGroupInfo(CLProtocol::SceneChampionGroupStatusSync& sync);

	/**
	*@brief �㲥һ����Ϣ
	*/
	void BroadcastOneGroupInfo(ChampionGroupStatus& group);

	/**
	*@brief �㲥��������Ϣ
	*/
	void BroadcastAllGroupInfo();

	/**
	*@brief ��ȡ16ǿ��
	*/
	std::vector<ChampionTop16Player>& GetTop16Table();

	/**
	*@brief ���볡��ʱ
	*/
	void OnEnterScene(Player * player);

	/**
	*@brief ��ȡ��������Ϣ
	*/
	std::map<UInt32, ChampionGroupStatus>& GetAllGroupInfo();

	/**
	*@brief ��ȡ���о�����Ŀ��Ϣ
	*/
	std::vector<ChampionGambleInfo>& GetAllGambleInfo();

	/**
	*@brief �������
	*/
	void OnPlayerOnline(Player* player);

	/**
	*@brief �������
	*/
	void OnPlayerOffline(Player* player);
private:
	bool GetChampionTopPlayerByPos(UInt32 pos, ChampionTop16Player& outPlayer);
private:
	//��ǰ״̬
	ChampionStatusInfo m_Status;
	//״̬����Map
	std::map<ChampionStatus, std::map<ChampionAct, ChampionActFun>> m_StatusActMap;
	//�ڹھ��������е����
	std::set<UInt64> m_PlayeInChampion;
	//16ǿ��
	std::vector<ChampionTop16Player> m_Top16PlayerVec;
	//Ѻע��Ϣ
	std::vector<ChampionGambleInfo> m_GambleInfoVec;
	//��id->����˳��order->����
	std::map<UInt32, std::map<UInt32, ChampionRaceData>> m_BattleRecordMap;
	//��id->����Ϣ�������ͻ�����
	std::map<UInt32, ChampionGroupStatus> m_GroupStatus;
	//����Handler
	Avalon::TimerHandler* m_SendAnnouncementTimer = nullptr;
};



#endif // !__SCENE_CHAMPION_MGR_H__






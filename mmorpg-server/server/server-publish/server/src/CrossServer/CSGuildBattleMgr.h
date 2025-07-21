#ifndef _CS_GUILD_BATTLE_MGR_H_
#define _CS_GUILD_BATTLE_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGuildDefine.h>
#include <CLGameSessionProtocol.h>
#include "CSGuildBattleTerritory.h"
#include "GameZoneMgr.h"

class CSGuildBattleMgr : public Avalon::Singleton<CSGuildBattleMgr>
{
public:
	CSGuildBattleMgr() : m_Status(GuildBattleStatus::GBS_INVALID), m_TargetStatus(GuildBattleStatus::GBS_INVALID), m_CheckWaitTime(0), m_IsByTime(true){}
	~CSGuildBattleMgr(){}

	bool Init();

public:
	/**
	*@brief ���ر�����Ϣ
	*/
	void LoadEnroll(std::map<UInt8, std::vector<CSEnrollInfo>> enrollInfo);

	/**
	*@brief ���ӱ�����Ϣ
	*/
	UInt32 AddEnroll(CSEnrollInfo info);

	/**
	*@brief ���������Ϣ
	*/
	UInt32 SetTerritoryBaseInfo(std::vector<GuildTerritoryBaseInfo> info);

	/**
	*@brief ��ȡ�����Ϣ
	*/
	UInt32 GetTerritoryBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info);

	/**
	*@brief ��ȡ���������Ϣ
	*/
	UInt32 GetTerritoryBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief һ������ս���
	*/
	UInt32 OnCrossBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);

	/**
	*@brief ���󹫻�ս��¼
	*/
	UInt32 GetBattleRecord(UInt8 terrId, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count);
	UInt32 GetBattleSelfRecrod(UInt8 terrId, ObjID_t playerId, std::vector<GuildBattleRecord>& records);

	/**
	*@brief �������а�
	*/
	UInt32 UpdateSortList(UInt8 terrId, TerritorySortListEntry& entry);

	/**
	*@brief ��ȡ���а�
	*/
	UInt32 GetSortList(GameZone* zone, UInt8 terrId, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

public:
	void OnTick(const Avalon::Time& now);

	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief ������ʼ
	*/
	void OnEnrollBegin();

	/**
	*@brief ��������
	*/
	void OnEnrollEnd();

	/**
	*@brief ս��׼��
	*/
	void OnBattlePrepare();

	/**
	*@brief ս����ʼ
	*/
	void OnBattleBegin();

	/**
	*@brief ս������
	*/
	void OnBattleEnd();

	/**
	*@brief �콱
	*/
	void OnBattleReward();

	/**
	*@brief ת����Ŀ�깫��ս״̬
	*/
	void ChangeTargetStatus(GuildBattleStatus targetStatus);

public:
	/**
	*@brief �Ƿ�������ս
	*/
	bool IsCrossBattle();

	/**
	*@brief �Ƴ�����Ҫ�ȴ�������Ϣ��ID
	*/
	void RemoveWaitLoadEnrollConnId(UInt32 id);

	/**
	*@brief ��ȡ��������
	*/
	UInt32 GetEnrollSize(UInt32 terrId);

	/**
	*@brief ����ս�Ƿ��ڿ���״̬
	*/
	bool IsGuildBattle() { return m_Status == GuildBattleStatus::GBS_BATTLE; }

	/**
	*@brief ����ս�Ƿ���Ա���
	*/
	bool IsGuildBattleEnroll() { return m_Status == GuildBattleStatus::GBS_ENROLL; }

	/**
	*@brief ���ù���ս����
	*/
	void SetGuildBattleType(GuildBattleType type) { m_Type = type; };

	/**
	*@brief ��������Ȼʱ��仯״̬
	*/
	void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	/**
	*@brief ���ù���սʱ����Ϣ
	*/
	void SetGuildBattleTimeInfo(GuildBattleTimeInfo info);

	/**
	*@brief ��ȡ����
	*/
	UInt32 GetRanking(UInt8 terrId, SortListType sortType, ObjID_t playerId, ObjID_t guildId);

private:
	bool _IsEnroll(UInt64 guildId);

	/**
	*@brief ���������Ϣ
	*/
	void _ClearTerritory();

	/**
	*@brief ���������Ϣ
	*/
	void _ClearTerritory(UInt8 terrId);

	/**
	*@brief ��ȡ�����Ϣ
	*/
	CSGuildBattleTerritory* _GetGuildBattleTerritory(UInt8 terrId);

	/**
	*@brief ��ʼ������ս״̬
	*/
	void _InitGuildBattleStatus();

	/**
	*@brief ���ù���ս״̬
	*/
	void _SetGuildBattleStatus(GuildBattleStatus status);

	/**
	*@brief ִ��״̬ת��
	*/
	void _ChangeToTargetStatus();

private:
	//������Ϣ
	HashMap<UInt8, std::vector<CSEnrollInfo>>		m_Enroll;

	//���������Ϣ
	HashMap<UInt8, CSGuildBattleTerritory>		m_Territory;

	//�ȴ����ر�����Ϣ�ķ�����
	std::vector<UInt32>							m_WaitLoadEnrollConnIds;

	//��ǰ����ս״̬
	GuildBattleStatus							m_Status;

	//Ŀ��״̬
	GuildBattleStatus							m_TargetStatus;

	//��ǰ����ս����
	GuildBattleType								m_Type;

	//����սʱ��
	GuildBattleTimeInfo							m_GuildBattleTimeInfo;

	//����ս���
	std::vector<GuildBattleEndInfo>				m_GuildBattleEndInfo;

	//���Tickʱ��
	UInt32										m_CheckWaitTime;

	//�Ƿ������Ȼʱ��
	bool										m_IsByTime;
};


#endif //_CS_GUILD_BATTLE_H_
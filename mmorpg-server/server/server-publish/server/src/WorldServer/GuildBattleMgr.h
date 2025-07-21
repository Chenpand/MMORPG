#ifndef __GUILD_BATTLE_MGR_H__
#define __GUILD_BATTLE_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLGuildProtocol.h>
#include "GuildBattleTerritory.h"

class WSPlayer;
class Guild;
class GuildMember;
class MatchServerGuildBattleRaceEnd;

class GuildBattleMgr : public Avalon::Singleton<GuildBattleMgr>
{
public:

	GuildBattleMgr() :m_Status(GuildBattleStatus::GBS_INVALID), m_TargetStatus(GuildBattleStatus::GBS_INVALID), m_IsGuildLoad(false), m_IsByTime(true){}
	~GuildBattleMgr(){}

	bool Init();

	/**
	*@brief ��ӱ�����Ϣ
	*/
	UInt32 AddEnroll(Guild* guild, UInt8 terrId, UInt64 playerId);

	/**
	*@brief �����ս��Ϣ
	*/
	UInt32 AddChallengeEnroll(Guild* guild, UInt8 terrId, UInt64 playerId);

	/**
	*@brief ��ʼ��������Ϣ
	*/
	UInt32 LoadEnroll(Guild* guild);

	/**
	*@brief ��ȡ��������
	*/
	UInt32 GetEnrollSize(UInt8 terrId);
	UInt32 GetEnrollSize(GuildBattleTerritory* terr);

	/**
	*@brief ��������Ȼʱ��仯״̬
	*/
	void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	/**
	*@brief ���ù���սʱ����Ϣ
	*/
	void SetGuildBattleTimeInfo(GuildBattleTimeInfo info);

public:
	/**
	*@brief ����
	*/
	UInt32 OnRequestEnroll(WSPlayer* player, UInt8 terrId, UInt32 itemId = 0, UInt32 itemNum = 0);

	/**
	*@brief ����
	*/
	UInt32 OnRequestInspire(WSPlayer* player, UInt8& inspire);

	/**
	*@brief ƥ��
	*/
	UInt32 OnMatchBattleStart(WSPlayer* player);

	/**
	*@brief һ������ս��ʼ
	*/
	UInt32 OnBattleRaceStart(WSPlayer* player);

	/**
	*@brief һ������ս���
	*/
	UInt32 OnBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);

	/**
	*@brief ������ȡ����
	*/
	UInt32 OnRequestReceive(WSPlayer* player, UInt32 boxId);

	/**
	*@brief ���󹫻�ս��¼
	*/
	UInt32 OnRequestBattleRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count);
	UInt32 OnRequestBattleSelfRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records);

	/**
	*@brief ��ȡ�����Ϣ
	*/
	UInt32 GetTerritroyBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info);

	/**
	*@brief ��ȡ���������Ϣ
	*/
	UInt32 GetTerritroyBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief ��ȡ����ս����
	*/
	GuildBattleType GetGuildBattleType() { return m_Type; }

	/**
	*@brief ��ȡ����ս״̬
	*/
	GuildBattleStatus GetGuildBattleStatus() { return m_Status; }

	/**
	*@brief ��ȡ����ս״̬����ʱ��
	*/
	UInt32 GetGuildBattleStatusEndTime(UInt32 status);

	/**
	*@brief ����ս����
	*/
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	/**
	*@brief ��ȡ��������
	*/
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	/**
	*@brief ��ѯ������ս��Ϣ
	*/
	UInt32 SelectGuildBattleChallengeInfo(WSPlayer* player);

	/**
	*@brief �齱
	*/
	UInt32 Lottery(WSPlayer* player);

	/**
	*@brief ���Ӽ�¼
	*/
	UInt32 AddTestRecord(WSPlayer* player);

	/**
	*@brief ��ɢ���
	*/
	bool Dismiss(UInt64 guildId);

	/**
	*@brief 
	*/
	bool IsCrossGuildBattleDungeon(Guild* guild, UInt32 dungeonId);

	/**
	*@brief ����ս�Ƿ���Ч�ڼ�
	*/
	bool IsValidGuildBattle();

	/**
	*@brief ��ȡ��������������
	*/
	UInt32 GetGuildTerritoryType(UInt64 guildId);

public:
	//�������ս

	/**
	*@brief ��ȡ�������ս������Ϣ
	*/
	UInt32 GetCrossEnrollInfo(std::map<UInt8, std::vector<CSEnrollInfo>>& enrollInfo, std::vector<GuildTerritoryBaseInfo>& baseInfos);

	/**
	*@brief ��ʼ���������ս��Ϣ
	*/
	void InitCrossSortListInfo();

	/**
	*@brief ��ȡ�������ս�����Ϣ
	*/
	UInt32 SetCrossTerritoryInfo(GuildTerritoryBaseInfo& info);

	/**
	*@brief ��ȡ�������ս�����Ϣ
	*/
	UInt32 SetCrossTerritoryInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief ����ս�Ƿ�������
	*/
	bool IsGuildLoad() { return m_IsGuildLoad; }

	/**
	*@brief �����ɢ����
	*/
	void GuildDismiss(ObjID_t id);

public:
	// ����ս�¼�

	void OnTick(Avalon::Time now);

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
	*@brief ��սս������
	*/
	void OnChallengeBattleEnd();

	/**
	*@brief �������ս����
	*/
	void OnCrossBattleEnd();

	/**
	*@brief �콱
	*/
	void OnBattleReward();

	/**
	*@brief Tick
	*/
	void OnDayChanged();

	/**
	*@brief ������Ϣ�������
	*/
	void OnGuildLoadFinish();

	/**
	*@brief ͬ���¼�¼
	*/
	void SyncBattleRecord(UInt8 terrId, CLProtocol::WorldGuildBattleRecordSync sync);

	/**
	*@brief ��ȡ���ID
	*/
	UInt8 GetTerritoryId(UInt64 guildId);

	/**
	*@brief ������ع���
	*/
	void SetTerrGuild(Guild* guild);

	/**
	*@brief ����ռ����ع���
	*/
	void SetCrossTerrGuild(Guild* guild);

	/**
	*@brief ����ս�Ƿ��ڿ���״̬
	*/
	bool IsGuildBattle();

	/**
	*@brief �����Ƿ��ڱ�����
	*/
	bool IsEnroll(Guild* guild);

	/**
	*@brief ����ս�����Ƿ���Ҫ֪ͨ
	*/
	bool IsNotifyGuildEnroll(Guild* guild);

	/**
	*@brief ת����Ŀ�깫��ս״̬
	*/
	void ChangeTargetStatus(GuildBattleStatus targetStatus);

	/**
	*@brief ���ù���ս����
	*/
	void SetGuildBattleType(GuildBattleType type) { m_Type = type; };

	/**
	*@brief �Ƿ���ͨ����ս
	*/
	bool IsNormalBattle();

	/**
	*@brief �Ƿ񹫻���ս
	*/
	bool IsChallengeBattle();

	/**
	*@brief �Ƿ�������ս
	*/
	bool IsCrossBattle();

	/**
	*@brief ��ȡ��ս���ID
	*/
	UInt8 GetGuildBattleChallengeTerrId();

	/**
	*@brief ��ȡ��ս��ƷID
	*/
	UInt32 GetGuildBattleChallengeItemId();

	/**
	*@brief ��ȡ��ս��Ʒ����
	*/
	UInt32 GetGuildBattleChallengeItemNum();

	/**
	*@brief ������ر�������
	*/
	void ClearTerritoryTransaction(UInt8 terrId);

	/**
	*@brief ͬ������ս��ս��Ϣ
	*/
	void SyncGuildBattleChallengeInfoToPlayers();

	/**
	*@brief �Ƿ�齱״̬
	*/
	bool IsGuildBattleLottery();

	/**
	*@brief �·��������ս������Ϣ
	*/
	void SyncCrossGuildBattleEndInfo(std::vector<GuildBattleEndInfo> info);

private:
	/**
	*@brief ����ս�Ƿ���Ա���
	*/
	bool _IsGuildBattleEnroll();

	/**
	*@brief �Ƿ��Ѿ�����
	*/
	bool _IsEnroll(UInt64 guildId);

	/**
	*@brief ��������Ϣ
	*/
	void _ClearEnroll();

	/**
	*@brief ���������Ϣ
	*/
	void _ClearTerritory();

	/**
	*@brief ��������ر�����Ϣ
	*/
	void _ClearTerritoryCrossEnrollInfo();

	/**
	*@brief ���������Ϣ
	*/
	void _ClearTerritory(UInt8 terrId);

	/**
	*@brief ��������ر�����Ϣ
	*/
	void _ClearTerritoryCrossEnrollInfo(UInt8 terrId);

	/**
	*@brief ��ȡ�����Ϣ
	*/
	GuildBattleTerritory* _GetGuildBattleTerritory(UInt8 terrId);

	/**
	*@brief ����ս�������Ź���
	*/
	void _BattleEndAnnouncement(GuildTerritoryType type = GuildTerritoryType::GTT_NORMAL);

	/**
	*@brief ����սһ����
	*/
	void _BattleInvalid(UInt64 playerA, UInt64 playerB, UInt8 result);

	/**
	*@brief �����вμӹ���ս�ĳ�Ա������Ϣ
	*/
	void _SendBattleMember(Avalon::Protocol &protocol);

	/**
	*@brief ��¼����ͳ��
	*/
	void _SendGuildPvpUdpLog(GuildMember* winGuildMember, UInt32 winOldGuildScore, GuildMember* loseGuildMember, UInt32 loseOldGuildScore, UInt8 result);

	/**
	*@brief ��ʼ������ս״̬
	*/
	void _InitGuildBattleStatus();

	/**
	*@brief ���湫��ս״̬
	*/
	void _SaveGuildBattleStatus();

	/**
	*@brief ��ʼ������ս��ս����
	*/
	void _InitGuildBattleChallengePrice();

	/**
	*@brief ���湫��ս��ս����
	*/
	void _SaveGuildBattleChallengePrice();

	/**
	*@brief ͬ��������ս��Ϣ
	*/
	UInt32 _SyncGuildBattleChallengeInfo(WSPlayer* player);

	/**
	*@brief ͬ������ս״̬
	*/
	void _SyncGuildBattleStatus();

	/**
	*@brief ���ù���ս״̬
	*/
	void _SetGuildBattleStatus(GuildBattleStatus status);

	/**
	*@brief ִ��״̬ת��
	*/
	void _ChangeToTargetStatus();

	/**
	*@brief ��ȡ��ս�Ĺ���
	*/
	Guild* _GetEnrollChallengeGuild();

	/**
	*@brief ������ս�ļ۸�
	*/
	void _ResetGuildBattleChallengePrice();

	/**
	*@brief ������ι���ս
	*/
	void _SkipGuildBattle();

	/**
	*@brief ÿ������콱״̬ˢ��
	*/
	void _DayMemberTerrRewardRefresh(bool isClear);

private:
	// ������Ϣ
	HashMap<UInt8, std::vector<ObjID_t> >		m_Enroll;

	// ���������Ϣ
	HashMap<UInt8, GuildBattleTerritory>		m_Territory;

	//��ǰ״̬
	GuildBattleStatus							m_Status;

	//Ŀ��״̬
	GuildBattleStatus							m_TargetStatus;

	//��ǰ����ս����
	GuildBattleType								m_Type;

	//����սʱ��
	GuildBattleTimeInfo							m_GuildBattleTimeInfo;

	//����ս���
	std::vector<GuildBattleEndInfo>				m_GuildBattleEndInfo;

	//������ս���ۼ۸�
	UInt32										m_GuildBattleChallengePrice;

	//������ս�������
	ObjID_t										m_GuildBattleChallengePlayer;

	//������սԤ�����б�,��ע�۸�仯�����
	std::vector<ObjID_t>						m_GuildBattlePreEnrollPlayers;

	//�����Ƿ�������
	bool										m_IsGuildLoad;

	//�Ƿ������Ȼʱ��
	bool										m_IsByTime;
};

#endif
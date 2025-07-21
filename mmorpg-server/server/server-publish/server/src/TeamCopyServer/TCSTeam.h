#ifndef _TCS_TEAM_H_
#define _TCS_TEAM_H_

#include <AvalonProtocol.h>
#include <CLTeamCopyDefine.h>
#include <CLItemDefine.h>

#include "TCSPlayerMgr.h"

struct TeamCopyTargetCfg;
struct TeamCopyFieldCfg;
class TeamCopyFieldDataEntry;
class TCSPlayer;
namespace CLProtocol {
	class DungenServerTeamStartRaceRes;
	class DungeonSvrDungeonRaceEnd;
};

#define TEAMINFO(T) "team(type:" << T->GetTeamType() << ",id:" << T->GetTeamId() \
	<< ",name:" << T->GetTeamName() << ",grade:" << T->GetTeamGrade() << ",model:" << T->GetModel()<< ") "

/**
*@brief �ű�����
*/
class TCSTeam
{
	AVALON_DEFINE_OBJECTPOOL(TCSTeam, Avalon::NullMutex)

public:
	TCSTeam();
	virtual ~TCSTeam();

	/**
	*@brief �ű�id
	*/
	inline void SetTeamType(UInt32 type) { m_TeamType = TeamCopyType(type); }
	inline UInt32 GetTeamType() { return m_TeamType; }
	
	/**
	*@brief ��������id
	*/
	inline void SetVoiceRoomId(const std::string& id) { m_VoiceRoomId = id; }
	inline const std::string& GetVoiceRoomId() { return m_VoiceRoomId; }

	/**
	*@brief ����id
	*/
	inline void SetTeamId(UInt32 id) { m_TeamId = id; }
	inline UInt32 GetTeamId() { return m_TeamId; }

	/**
	*@brief ����
	*/
	inline void SetTeamName(const std::string& name) { m_TeamName = name; }
	inline const std::string& GetTeamName() { return m_TeamName; }

	/**
	*@brief �Ѷ�
	*/
	inline void SetTeamGrade(UInt32 grade) { m_TeamGrade = TeamCopyTeamGrade(grade); }
	inline UInt32 GetTeamGrade() { return m_TeamGrade; }

	/**
	*@brief ģʽ
	*/
	inline void SetModel(UInt32 model) { m_Model = TeamCopyTeamModel(model); }
	inline UInt32 GetModel() { return m_Model; }

	/**
	*@brief װ������
	*/
	inline void SetEquipScore(UInt32 score) { m_EquipScore = score; }
	inline UInt32 GetEquipScore() { return m_EquipScore; }

	/**
	*@brief ����Ӷ��
	*/
	inline void SetCondCommission(UInt32 num) { m_CondCommission = num; }
	inline UInt32 GetCondCommission() { return m_CondCommission; }

	/**
	*@brief ��Ӷ��
	*/
	inline void SetTotalCommission(UInt32 num) { m_TotalCommission = num; }
	inline UInt32 GetTotalCommission() { return m_TotalCommission; }

	/**
	*@brief ״̬
	*/
	inline void SetStatus(UInt32 status) { m_Status = status; }
	inline UInt32 GetStatus() { return m_Status; }

	/**
	*@brief �ų�Id
	*/
	inline void SetChiefId(UInt64 id) { m_ChiefId = id; }
	inline UInt64 GetChiefId() { return m_ChiefId; }

	/**
	*@brief �Զ�ͬ���������
	*/
	inline void SetAutoAgreeGold(UInt32 isAgree) { m_AutoAgreeGold = isAgree; }
	inline bool GetAutoAgreeGold() { return m_AutoAgreeGold ? 1 : 0; }

	/**
	*@brief �׶�
	*/
	inline void SetCurStage(UInt32 stage) { m_CurStage = stage; }
	inline UInt32 GetCurStage() { return m_CurStage; }

	/**
	*@brief ����ģʽ
	*/
	inline void SetPlanModel(UInt32 model) { m_PlanModel = model; }
	inline UInt32 GetPlanModel() { return m_PlanModel; }

	/**
	*@brief ��Ϸ����ʱ��
	*/
	inline void SetGameOverTime(UInt32 time) { m_GameOverTime = time; }
	inline UInt32 GetGameOverTime() { return m_GameOverTime; }

	/**
	*@brief ��ļCD
	*/
	inline void SetRecruitCD(UInt32 time) { m_RecruitCD = time; }
	inline UInt32 GetRecruitCD() { return m_RecruitCD; }

	/**
	*@brief �۳����ı��
	*/
	inline void SetConsumeFlag(bool flag) { m_ConsumeFlag = flag; }
	inline bool GetConsumeFlag() { return m_ConsumeFlag; }

	/**
	*@brief ͶƱ����ʱ��
	*/
	inline void SetVoteEndTime(UInt32 time) { m_VoteEndTime = time; }
	inline UInt32 GetVoteEndTime() { return m_VoteEndTime; }
	
	/**
	*@brief ��������������ʱ��
	*/
	inline void SetCheckCondEndTime(UInt32 time) { m_CheckCondEndTime = time; }
	inline UInt32 GetCheckCondEndTime() { return m_CheckCondEndTime; }
	
	/**
	*@brief ǿ�ƽ������
	*/
	inline void SetForceEndFlag(bool flag) { m_ForceEndFlag = flag; }
	inline bool GetForceEndFlag() { return m_ForceEndFlag; }

	/**
	*@brief ǿ�ƽ���ͶƱʱ��
	*/
	inline void SetForceEndVoteTime(UInt32 time) { m_ForceEndVoteTime = time; }
	inline UInt32 GetForceEndVoteTime() { return m_ForceEndVoteTime; }
	
	/**
	*@brief ǿ�ƽ�����ȴʱ��
	*/
	inline void SetForceEndCoolTime(UInt32 time) { m_ForceEndCoolTime = time; }
	inline UInt32 GetForceEndCoolTime() { return m_ForceEndCoolTime; }

	/**
	*@brief С��
	*/
	const std::vector<TeamCopySquad>& GetSquadList() { return m_SquadVec; }
	
	/**
	*@brief �Ƿ��Ѿ���ս
	*/
	bool IsYetStartBattle();

	/**
	*@brief �Ƿ�ս����
	*/
	bool IsInBattle();

	/**
	*@brief �Ƿ����
	*/
	bool IsGameOver();

	/**
	*@brief Ӷ��ֳɶ��
	*/
	UInt32 CommissionBonusNum();

	/**
	*@brief Tick
	*/
	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief �Ӷ�����
	*/
	UInt32 CheckJoinTeam(TCSPlayer* joinPlayer, UInt32 isGold);

	/**
	*@brief ���Ӷ�Ա
	*/
	UInt32 JoinTeam(TCSPlayer* joinPlayer, UInt32 isGold = 0);
	bool QuitTeam(UInt64 playerId);

	/**
	*@brief ��ö�������
	*/
	UInt32 GetTeamMemberNum();

	/**
	*@brief ��������
	*/
	UInt32 GoldPlayerNum();

	/**
	*@brief ��鿪ս
	*/
	UInt32 CheckStartBattle();

	/**
	*@brief �����б�
	*/
	UInt32 JoinApply(UInt64 playerId, UInt32 isGold);
	void RemoveApply(UInt64 playerId);
	void ClearApply();
	void ApplyListRes(TCSPlayer* player);
	UInt32 AgreeApply(UInt64 playerId);
	UInt32 ApplyNum();

	/**
	*@brief ����С��״̬
	*/
	void SetSquadStatus(UInt32 squadId, UInt32 status);

	/**
	*@brief �������ݹ㲥
	*/
	void TeamDataBroadcast();
	void TeamDataRes(TCSPlayer* player);

	/**
	*@brief ������ж����Ա�б�
	*/
	const std::map<UInt64, UInt32>& GetMemberMap() { return m_PlayerSquad;  }

	/**
	*@brief ��սȨ��
	*/
	bool StartChallengePower(UInt64 playerId);

	/**
	*@brief ������С��id
	*/
	UInt32 GetPlayerSquadId(UInt64 playerId);

	/**
	*@brief ֪ͨδͶƱ���ͶƱ
	*/
	void NotifyPlayerVote();

	/**
	*@brief ��սͶƱ
	*/
	void HandlerVote(UInt64 playerId, UInt32 vote);

	/**
	*@brief ɾ��ͶƱ���
	*/
	void RemoveVote(UInt64 playerId);

	/**
	*@brief �ݵ㹥��С��
	*/
	void FieldAddAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId);
	void FieldDelAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId);

	/**
	*@brief ֪ͨС��״̬
	*/
	void SquadNotify(UInt32 squadId, UInt32 squadStatus);

	/**
	*@brief Ӷ�𶩵���
	*/
	void AddCommissionOrder(UInt64 playerId, UInt32 connid, UInt64 commissionOrder);

	/**
	*@brief ��λ��
	*/
	UInt32 ChangeSeat(UInt32 srcSeat, UInt32 destSeat);

	/**
	*@brief ������Ա
	*/
	UInt32 AppointmentPost(UInt64 handlerPlayer, UInt64 playerId, UInt32 post);

	/**
	*@brief ������Ʊ
	*/
	void TicketUpdate(UInt64 playerId, UInt32 isEnough);

	/**
	*@brief ����
	*/
	UInt32 KickMember(UInt64 playerId);

	/**
	*@brief ����ʱ������
	*/
	void JoinTimeLimit(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief �Ƿ����ʱ������
	*/
	bool IsJoinTimeLimit(UInt64 playerId);

	/**
	*@brief ��ȡ����ʱ������
	*/
	UInt64 GetJoinTimeLimit(UInt64 playerId);

	/**
	*@brief ���Ĵ��
	*/
	void SaveConsume(UInt64 playerId, UInt32 itemId, UInt32 itemNum, UInt32 consumeType);
	void RemoveDBConsume(UInt64 guid);
	void RemoveAllMemberConsume();

	/**
	*@brief ��Ա���ķ���
	*/
	void MemberReturnConsume(UInt64 playerId = 0);

	/**
	*@brief ǿ�ƽ���ͶƱ
	*/
	void ForceEndVote(TCSPlayer* player, UInt32 vote);

	/**
	*@brief ǿ�ƽ���ͶƱ��ʱ
	*/
	void ForceEndVoteTimeOut();

	/**
	*@brief �������ĸ�������
	*/
	void ReturnConsumeByType(UInt32 consumeType);

	/**
	*@brief ʧ�ܷ�����ս����
	*/
	void FailedReturnChallengeNum();

	/**
	*@brief ��������
	*/
	void FlopDataRes(TCSPlayer* player, UInt32 stage);

	/**
	*@brief �ݵ����Udp��־
	*/
	void UdpFieldSettle(UInt32 squadId, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime, UInt32 result);

	/**
	*@brief ���װ�����ֵ͵����
	*/
	void ClearEquipScoreLowPlayer();

	/**
	*@brief GM�޸������ݵ����
	*/
	void GMUpdateEnergyField(UInt32 rate);

	/**
	*@brief roll����
	*/
	void RollRewardPoint(TCSPlayer* player, UInt32 isNeed);

	/**
	*@brief �㲥֪ͨ
	*/
	template <typename... Args>
	void BroadcastToMember(UInt32 squadId, UInt32 id, const Args & ... args)
	{
		for (auto& iter : m_PlayerSquad)
		{
			if (squadId != 0 && iter.second != squadId)
			{
				continue;
			}

			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
			if (NULL == player)
			{
				continue;
			}

			player->SendNotify(id, args...);
		}
	}

	/**
	*@brief �㲥������Ϣ,squadIdΪ0��ʾȫ�ӹ㲥�������ʾС��ID
	*/
	void BroadcastToMember(Avalon::Protocol& protocol, UInt32 squadId = 0);
	void BroadcastToMember(Avalon::Packet& packet, UInt32 squadId = 0);

	/**
	*@brief �㲥������Ϣ, ��ĳ���֮��
	*/
	void BroadcastToExceptMember(Avalon::Protocol& protocol, UInt64 exceptPlayer = 0);
	void BroadcastToExceptMember(Avalon::Packet& packet, UInt64 exceptPlayer = 0);

protected:

	/**
	*@brief ���Ҷ�Ա
	*/
	TeamCopyMember* _FindMember(UInt64 playerId);
	TeamCopyMember* _FindMember(UInt32 seatId);

	/**
	*@brief ���Ҷӳ�
	*/
	TeamCopyMember* _FindCaptain(UInt64 playerId);

	/**
	*@brief �����ų�
	*/
	TeamCopyMember* _FindChief();

	/**
	*@brief ����С��
	*/
	TeamCopySquad* _FindSquad(UInt32 squadId);

	/**
	*@brief С���Ƿ��ж�Ա
	*/
	UInt32 _SquadMemberNum(UInt32 squadId);

	/**
	*@brief ������С��id
	*/
	UInt32 _GetPlayerSquadId(UInt64 playerId);

	/**
	*@brief ���¾ݵ�
	*/
	UInt32 _UpdateField(UInt32 fieldId);

	/**
	*@brief �׶ν���
	*/
	void _StageEnd(UInt32 stage, UInt32 param = 0);

	/**
	*@brief ���ӷ�����Ϣ
	*/
	void _AddFlop(UInt32 stage, const TeamCopyFlop& flop);

	/**
	*@brief ��ͨ�������
	*/
	UInt32 _CommonPlayerNum();

	/**
	*@brief ͶƱ֪ͨ
	*/
	void _VoteNotify(UInt64 playerId, UInt32 vote);

	/**
	*@brief ͶƱ��ɣ���ս
	*/
	void _VoteFinish();

	/**
	*@brief ͶƱ��ʱ
	*/
	void _VoteTimeOut();

	/**
	*@brief ������鳬ʱ
	*/
	void _CheckCondTimeOut();

	/**
	*@brief ��������Ӷ�� �������Ƿ񷵻�
	*/
	void _GameOverCommission(bool isReturn);

	/**
	*@brief �Ŷ�״̬֪ͨ
	*/
	void _TeamStatusNotify(UInt32 status);

	/**
	*@brief �ֳ�Ӷ��
	*/
	void _CommissionBonus(UInt32 senderId, UInt32 titleId, UInt32 contentId);

	/**
	*@brief ����ͨ�ش���
	*/
	void _AddModelPassNum(UInt32 model, UInt32 costTime);

	/**
	*@brief ��Ա֪ͨ
	*/
	void _MemberNotify(const std::string& name, UInt32 flag);

	/**
	*@brief bos�ݵ�֪ͨ�����ݵ㱻����
	*/
	bool _BossFieldAttachDestroyNotify(UInt32 dungeonId);

	/**
	*@brief �ݵ�״̬֪ͨ
	*/
	void _FieldStatusNotify(UInt32 fieldId, UInt32 status);

	/**
	*@brief ����ǿ�ƽ����ı���
	*/
	void _AddForceEndRace(UInt64 raceId);

	/**
	*@brief �Ƿ���ǿ�ƽ�������
	*/
	bool _IsForceEndRace(UInt64 raceId);

	/**
	*@brief ��սUdp��־
	*/
	void _UdpStartBattle();

	/**
	*@brief ֪ͨǿ�ƽ���
	*/
	void _NotifyForceEndFlag();

	/**
	*@brief ���������ָ��ݵ�
	*/
	void _HandleEnergyReviveField(UInt32 progress, UInt32 energyDungeonId);

	/**
	*@brief ֪ͨս�������ָ�����
	*/
	void _NotifyRaceEnergyRevive(UInt32 progress, UInt32 energyDungeonId, UInt32 needNotifyFieldType);

	/**
	*@brief ��ȡ���ֿ�ʼʱ��
	*/
	UInt32 _GetGameStartTime();

	/**
	*@brief ���ֱ����Ƿ�������
	*/
	UInt32 _GameIsCrossDate(UInt32 time);

	/**
	*@brief �Ƿ����׶ν���
	*/
	bool _StageIsSettle(UInt32 stage, UInt64 playerId);

	/**
	*@brief С��ͨ�ؾݵ�
	*/
	void _RaceSettlement(UInt32 squadId, UInt64 raceId);

	/**
	*@brief roll����
	*/
	void _RollReward();

	/**
	*@brief ��ȡroll��
	*/
	UInt32 _GetRollPoint();

	/**
	*@brief roll tick
	*/
	void _OnRollTick(const Avalon::Time& now);

	/**
	*@brief ����roll�ʼ�����
	*/
	void _SendRollMail(UInt64 playerId, UInt32 playerConnId);

public:

	/**
	*@brief С�ӹ�������֪ͨ
	*/
	virtual void SquadAttackDataNotify() {}

	/**
	*@brief ��ȡ��Ʊ
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec) {}

	/**
	*@brief ������������
	*/
	virtual UInt32 GetGoldMaxNum() { return 0; }
	virtual UInt32 GetGoldNumErrorCode() { return 0; }

	/**
	*@brief �����Ŷ�Ŀ��
	*/
	virtual bool SetTeamTarget(UInt32 stage) { return true; }

	/**
	*@brief ��������֪ͨ
	*/
	virtual void AttackDataNotify(UInt32 stage) {}
	virtual void AttackDataNotify(TCSPlayer* player) {}

	/**
	*@brief С��ͨ�ؾݵ�
	*/
	virtual void ClearField(UInt32 squadId, UInt32 fieldId) {}

	/**
	*@brief ����С���Ѷ�
	*/
	virtual void SetSquadGrade(UInt32 squadId, UInt32 grade) {}
	
	/**
	*@brief ����С��Ŀ��
	*/
	virtual void SetSquadTarget(UInt32 squadId, const TeamCopyTargetCfg* targetCfg) {}

	/**
	*@brief ��Ӿݵ�
	*/
	virtual bool AddField(const TeamCopyFieldDataEntry* fieldEntry) { return true; }

	/**
	*@brief �ݵ��������
	*/
	virtual void FieldRaceEnd(UInt32 fieldId) {}

	/**
	*@brief boss�׶�֪ͨ
	*/
	virtual void BossPhaseNotify(TCSPlayer* player) {}

	/**
	*@brief �ݵ�֪ͨ
	*/
	virtual void FieldNotify(const std::vector<UInt32>& filedVec) {}
	virtual void FieldNotify(UInt32 fieldId) {}

	/**
	*@brief �ϱ��׶�boss��Ϣ
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate){}

	/**
	*@brief ֪ͨս�������ָ�����
	*/
	virtual void NotifyRaceEnergyRevive(UInt64 raceId) {}

	/**
	*@brief ����������
	*/
	virtual void CheckCondFinish();

	/**
	*@brief ɾ��boss�����ݵ�
	*/
	virtual void DelBossAttachField() {}

	/**
	*@brief ֪ͨ����С���˳����³�
	*/
	virtual void NotifyAllSquadLeaveDungeon() {}

	/**
	*@brief ֪ͨ����С�ӵ��³Ǳ�����
	*/
	virtual void NotifyAllSquadDungeonDestory(UInt32 dungeonId) {}

	/**
	*@brief ���Ҿݵ�
	*/
	virtual TeamCopyField* FindField(UInt32 fieldId) { return NULL; }

	/**
	*@brief �����Ƿ���
	*/
	virtual bool TeamIsFull() { return true; }

	/**
	*@brief �ƶ�����
	*/
	virtual void MoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId) {}

	/**
	*@brief ��ѯ�����Ϣ��ʱ
	*/
	virtual void QueryRacePlayerInfoTimeOut(UInt32 squadId);

	/**
	*@brief ��ʼ�������
	*/
	virtual UInt32 StartRace(UInt32 squadId, UInt32 fieldId);

	/**
	*@brief ������������
	*/
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res) {}

	/**
	*@brief ��������
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime) {}

	/**
	*@brief Gmͨ��
	*/
	virtual void GMClearAllField(UInt32 squadId) {}

	/**
	*@brief ����С������
	*/
	virtual void SetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd) {}

	/**
	*@brief ������Ӷ���
	*/
	virtual void GMClearGridObj(UInt64 playerId, UInt32 gridType) {}

	/**
	*@brief ��Ϸ����
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief �Ƿ�������λ
	*/
	virtual UInt32 IsAllowChangeSeat(UInt32 srcSquadId, UInt32 destSquadId) { return 0; }

	/**
	*@brief ��λ���
	*/
	virtual void ChangeSeatFinish(UInt32 srcSquadId, bool srcIsDec, UInt32 destSquadId, bool destIsInc) {}

	/**
	*@brief ��Ա����
	*/
	virtual void MemberExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief ��Ա������
	*/
	virtual void SquadMemberNull(UInt32 squadId) {}

	/**
	*@brief �׶�Udp��־
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result) {}

protected:

	// �ű�id
	TeamCopyType m_TeamType;
	// ��������ID
	std::string m_VoiceRoomId;
	// id
	UInt32 m_TeamId;
	// ��������
	std::string m_TeamName;
	// �Ѷ�
	TeamCopyTeamGrade m_TeamGrade;
	// ģʽ
	TeamCopyTeamModel m_Model;
	// װ������
	UInt32 m_EquipScore;
	// ����Ӷ��
	UInt32 m_CondCommission;
	// ��Ӷ��
	UInt32 m_TotalCommission;
	// ״̬
	UInt32 m_Status;
	// �ų�ID
	UInt64 m_ChiefId;
	// �Զ�ͬ���������
	UInt32 m_AutoAgreeGold;
	// ��ǰ�׶�
	UInt32 m_CurStage;
	// ����ģʽ
	UInt32 m_PlanModel;
	// ��Ϸ����ʱ��
	UInt32 m_GameOverTime;
	// ��ļCDʱ��
	UInt32 m_RecruitCD;
	// �۳����ı��
	bool m_ConsumeFlag;

	// �����б�(key->���id��val->�Ƿ����)
	std::map<UInt64, UInt32> m_ApplyMap;

	// С���б�
	std::vector<TeamCopySquad> m_SquadVec;

	// ������ڵ�С��(key->���id��С��id)
	std::map<UInt64, UInt32> m_PlayerSquad;

	// ͶƱ����ʱ��
	UInt32 m_VoteEndTime;
	// ͶƱ�б�
	std::set<UInt64> m_VoteSet;

	// ������Ϣ(key->�׶�)
	std::map<UInt32, std::vector<TeamCopyFlop>> m_FlopMap;

	// ��������������ʱ��
	UInt32 m_CheckCondEndTime;

	// Ӷ���¼(key->playerId, val->Ӷ�𶩵���)
	std::map<UInt64, TeamCopyCommission> m_CommissiomMap;

	// �׶�ͨ��(key->�׶Σ�val->playerId)
	std::map<UInt32, std::set<UInt64>> m_StageSettlePlayerMap;

	// ǿ�ƽ����ı���(����Id)
	std::set<UInt64> m_ForceEndRaceSet;

	// ������ʱ������(key->playerId)
	std::map<UInt64, UInt64>	m_PlayerJoinTimeLimitMap;

	// ǿ�ƽ������
	bool m_ForceEndFlag;
	// ��ս���˶�����
	UInt32 m_BattleQuitTeamNum;
	// ǿ��ͶƱ����ʱ��
	UInt32 m_ForceEndVoteTime;
	// ǿ�ƽ�����ȴʱ��
	UInt32 m_ForceEndCoolTime;
	// ǿ��ͶƱ(key->playerId, val->�Ƿ�ͬ��)
	std::map<UInt64, bool> m_ForceEndVoteMap;

	// roll����ʱ���
	UInt32 m_RollEndStamp;
	// roll����
	ItemRewardVec m_RollRewards;
	std::vector<UInt32> m_RollPointVec;
	std::map<UInt64, TCPlayerRoll> m_RollMap;
};

#endif

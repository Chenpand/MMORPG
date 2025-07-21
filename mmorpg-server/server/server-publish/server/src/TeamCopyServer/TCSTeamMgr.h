#ifndef _TCS_TEAM_MGR_H_
#define _TCS_TEAM_MGR_H_

#include <set>
#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class TCSTeam;
class TCSTeamCheck;
class TCSTeamPlayerInfo;
class TCSPlayer;
struct TeamCopyBattlePlan;
struct DungeonRacePlayerInfo;
struct TCConsumeItem;


namespace CLProtocol{
	class TeamCopyCreateTeamReq;
	class DungenServerTeamStartRaceRes;
	class DungeonSvrDungeonRaceEnd;
}

namespace Avalon{
	class Protocol;
	class Packet;
}

/**
*@brief �Ŷӹ�����
*/
class TCSTeamMgr : public Avalon::Singleton<TCSTeamMgr>
{
public:
	TCSTeamMgr();
	~TCSTeamMgr();

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief Final
	*/
	void Final();

	/**
	*@brief �����Ƿ����
	*/
	bool IsExistTeam(UInt32 teamId);

	/**
	*@brief ����ͬ����scene
	*/
	void ValCfgSyncToScene(UInt32 connId);

	/**
	*@brief ��������
	*/
	void HandleCreateTeamReq(TCSPlayer* player, CLProtocol::TeamCopyCreateTeamReq& req);

	/**
	*@brief �����������
	*/
	void HandleTeamDataReq(TCSPlayer* player, UInt32 teamType);

	/**
	*@brief �����������
	*/
	void HandleTeamDetailReq(TCSPlayer* player, UInt32 teamId);

	/**
	*@brief ��������б�
	*/
	void HandleTeamListReq(TCSPlayer* player, UInt32 pageNum, UInt32 type, UInt32 model);

	/**
	*@brief �����������
	*/
	void HandleTeamApplyReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold);

	/**
	*@brief �˳���������
	*/
	void HandleTeamQuitReq(TCSPlayer* player);

	/**
	*@brief �����б�����
	*/
	void HandleTeamApplyListReq(TCSPlayer* player);

	/**
	*@brief ����ظ�
	*/
	void HandleTeamApplyReplyReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt64>& playerIds);

	/**
	*@brief ��ս����
	*/
	void HandleStartBattleReq(TCSPlayer* player, UInt32 planModel, std::vector<TeamCopyBattlePlan>& battlePlanVec);

	/**
	*@brief ��սͶƱ
	*/
	void HandleStartBattleVote(TCSPlayer* player, UInt32 vote);

	/**
	*@brief �ͻ��˷���ʼ��ս
	*/
	void HandleStartChallengeReq(TCSPlayer* player, UInt32 fieldId);

	/**
	*@brief ��ѯ�����Ϣ����
	*/
	void QueryPlayerRaceRes(UInt32 teamSquadId, UInt64 roleId, UInt32 result, DungeonRacePlayerInfo& playerInfo);

	/**
	*@brief ���������Ϣ��ѯ
	*/
	void CreateTeamPlayerInfo(UInt32 teamSquadId, UInt32 dungeonId, UInt32 fieldId, UInt32 tailDungeonId, std::set<UInt64> playerIds);

	/**
	*@brief ��������б�
	*/
	void HandlePlayerListReq(TCSPlayer* player);

	/**
	*@brief �������
	*/
	void HandleInvitePlayerReq(TCSPlayer* player, std::vector<UInt64>& inviteList);

	/**
	*@brief ������
	*/
	void HandleStageFlopReq(TCSPlayer* player, UInt32 stage);

	/**
	*@brief ����λ
	*/
	void HandleChangeSeatReq(TCSPlayer* player, UInt32 srcSeat, UInt32 destSeat);
	
	/**
	*@brief ����
	*/
	void HandleKickReq(TCSPlayer* player, UInt64 playerId);

	/**
	*@brief ����
	*/
	void HandleAppointmentReq(TCSPlayer* player, UInt64 playerId, UInt32 post);

	/**
	*@brief �Զ�ͬ���������
	*/
	void HandleAutoAgreeGoldReq(TCSPlayer* player, UInt32 isAutoAgree);

	/**
	*@brief ����ѡ��
	*/
	void HandleInviteChoiceReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt32> teamIds);

	/**
	*@brief ��ļ
	*/
	void HandleRecruitReq(TCSPlayer* player);

	/**
	*@brief ���Ӽ���
	*/
	void HandleLinkJoinReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold);

	/**
	*@brief ��鷵��
	*/
	void HandleCheckCondRes(UInt32 retCode, UInt32 teamId, UInt64 roldId, UInt64 commissionOrder, UInt32 connId,
		const TCConsumeItem& commission, const std::vector<TCConsumeItem>& ticket);

	/**
	*@brief ���³ǿ�ʼ��ս����
	*/
	void HandlerStartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief ��������
	*/
	void HandlerRaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end);

	/**
	*@brief �ϱ�boss�׶�
	*/
	void HandlerPhaseBoss(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief ǿ�ƽ���
	*/
	void HandleForceEndReq(TCSPlayer* player);

	/**
	*@brief ǿ�ƽ���ͶƱ
	*/
	void HandleForceEndVoteReq(TCSPlayer* player, UInt32 vote);

	/**
	*@brief �޸�װ������
	*/
	void HandleModifyEquipScore(TCSPlayer* player, UInt32 equipScore);

	/**
	*@brief �ƶ�����
	*/
	void HandleSquadMoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId);

	/**
	*@brief roll����
	*/
	void HandleRollReward(TCSPlayer* player, UInt32 isNeed);

	/**
	*@brief boss�׶ξݵ�
	*/
	void BossPhaseAddField(UInt32 teamSquadId, UInt32 fieldId);
	void BossPhaseDelField(UInt32 teamSquadId, UInt32 fieldId);

	/**
	*@brief ����˳�
	*/
	void TicketUpdate(UInt64 playerId, UInt32 isEnough);

	/**
	*@brief ����˳�
	*/
	void OnPlayerQuit(UInt64 playerId);
	
	/**
	*@brief ��Ҷ���Id
	*/
	UInt32 GetPlayerTeamId(UInt64 playerId);
	void SetPlayerTeamId(UInt64 playerId, UInt32 teamId);

	/**
	*@brief ��Ҷ���С��Id
	*/
	UInt32 GetPlayerTeamSquadId(UInt64 playerId);

	/**
	*@brief �Ƿ�����
	*/
	bool IsReconnect(UInt32 teamId);

	/**
	*@brief ͬ���������
	*/
	void SyncAllObjectToPlayer(TCSPlayer* player);

	/**
	*@brief GMͨ�ص�ǰ�׶����оݵ�
	*/
	void GMClearAllField(UInt64 playerId);

	/**
	*@brief GMͨ�ؾݵ�
	*/
	void GMClearField(UInt64 playerId, UInt32 fieldId);

	/**
	*@brief GM���С��״̬
	*/
	void GMClearSquadStatus(UInt64 playerId);

	/**
	*@brief GM�ű�ʧ��
	*/
	void GMTeamCopyFailed(UInt64 playerId);

	/**
	*@brief GM�����ָ��ݵ����
	*/
	void GMEnergyReviveRate(UInt64 playerId, UInt32 rate);

	/**
	*@brief GM����С������
	*/
	void GMSetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd);

	/**
	*@brief GM������Ӿݵ�
	*/
	void GMClearGridObj(UInt64 playerId, UInt32 gridType);

	/**
	*@brief ���鿪ս���
	*/
	TCSTeamCheck* GetTeamCheck(UInt32 teamId);
	void DelTeamCheck(UInt32 teamId);
	void AddTeamCheck(UInt32 teamId, UInt64 playerId);

	/**
	*@brief �������
	*/
	void PlayerExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief ��ȡ����ݵ�id
	*/
	UInt32 GetAttackFieldId(UInt32 teamSquadId);

	/**
	*@brief �㲥������Ϣ
	*/
	void BroadcastToTeam(Avalon::Protocol& protocol, UInt32 teamId, UInt64 exceptPlayer = 0);
	void BroadcastToTeam(Avalon::Packet* packet, UInt32 teamId, UInt64 exceptPlayer = 0);

	/**
	*@brief �㲥С����Ϣ
	*/
	void BroadcastToSquad(Avalon::Protocol& protocol, UInt32 teamId, UInt64 playerId);
	void BroadcastToSquad(Avalon::Packet* packet, UInt32 teamId, UInt64 playerId);

private:

	/**
	*@brief ��������б�
	*/
	TCSTeam* _FindTeam(UInt32 teamId);
	TCSTeam* _FindTeam(UInt64 playerId);

	/**
	*@brief ��������б�
	*/
	void _AddTeamMap(TCSTeam* team);

	/**
	*@brief �˳�����
	*/
	UInt32 _QuitTeam(UInt64 playerId, TCSPlayer* player);

	/**
	*@brief ��Ҷ���Id
	*/
	UInt32 _GetPlayerTeamId(UInt64 playerId);
	void _SetPlayerTeamId(UInt64 playerId, UInt32 teamId);

	/**
	*@brief ���ս������
	*/
	bool _CheckBattlePlan(const std::vector<TeamCopyBattlePlan>& battlePlanVec);

	/**
	*@brief �����������
	*/
	TCSTeamPlayerInfo* _FindTeamPlayerInfo(UInt32 teamSquadId);

	/**
	*@brief playerInfo ����
	*/
	void _TeamPlayerInfoEnd(UInt32 teamSquadId);

	/**
	*@brief ��������
	*/
	TCSTeam* _CreateTeam(UInt32 tcType);

private:

	typedef std::map<UInt32, TCSTeam*> TCSTeamMap;
	typedef std::map<UInt32, TCSTeamCheck> TCSTeamCheckMap;
	typedef std::map<UInt32, TCSTeamPlayerInfo> TCSTeamPlayerInfoMap;

	// ����ID����
	UInt32 m_TeamIdSeed;

	// ������ڶ���(key->playerId, val->teamId)
	std::map<UInt64, UInt32> m_PlayerTeamIdMap;

	// �����б�(key->����Id)
	TCSTeamMap	 m_TeamMap;

	// ���
	TCSTeamCheckMap		m_TeamCheck;

	// ��ѯ�������(key->teamSquadId)
	TCSTeamPlayerInfoMap	 m_TeamPlayerInfoMap;

	Avalon::SimpleTimer m_60sTimer;

};

#endif

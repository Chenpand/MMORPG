#ifndef _TCS_TEAM_TWO_H_
#define _TCS_TEAM_TWO_H_

#include <CLTeamCopyDefine.h>
#include <AvalonSimpleTimer.h>
#include "TCSTeam.h"

class TCGridObj;
class TCSquadObj;
class TCMonsterObj;
class TCMonsterTransferObj;
class TCLaboratoryObj;

/**
 *@brief �ű�2 ����
 */
class TCSTeamTwo : public TCSTeam
{
public:
	TCSTeamTwo();
	~TCSTeamTwo();

	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief �����Ƿ���
	*/
	virtual bool TeamIsFull();

	/**
	*@brief ��ȡ��Ʊ
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec);

	/**
	*@brief ������������
	*/
	virtual UInt32 GetGoldMaxNum();
	virtual UInt32 GetGoldNumErrorCode();

	/**
	*@brief ��������֪ͨ
	*/
	virtual void AttackDataNotify(UInt32 stage);
	virtual void AttackDataNotify(TCSPlayer* player);

	/**
	*@brief ����������
	*/
	virtual void CheckCondFinish();

	/**
	*@brief �ƶ�����
	*/
	virtual void MoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId);

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
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief ��������
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime);

	/**
	*@brief �ϱ��׶�boss��Ϣ
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief ��Ϸ����
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief Gmͨ��
	*/
	virtual void GMClearAllField(UInt32 squadId);

	/**
	*@brief ����С������
	*/
	virtual void SetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd);

	/**
	*@brief ������Ӷ���
	*/
	virtual void GMClearGridObj(UInt64 playerId, UInt32 gridType);

	/**
	*@brief �Ƿ�������λ
	*/
	virtual UInt32 IsAllowChangeSeat(UInt32 srcSquadId, UInt32 destSquadId);

	/**
	*@brief ��λ���
	*/
	virtual void ChangeSeatFinish(UInt32 srcSquadId, bool srcIsDec, UInt32 destSquadId, bool destIsInc);

	/**
	*@brief С�ӹ�������֪ͨ
	*/
	virtual void SquadAttackDataNotify();

	/**
	*@brief ��Ա����
	*/
	virtual void MemberExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief ��Ա������
	*/
	virtual void SquadMemberNull(UInt32 squadId);

	/**
	*@brief �׶�Udp��־
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result);

	/**
	*@brief ���Ҹ��Ӷ���
	*/
	TCGridObj* FindGridObjByGridId(UInt32 gridId);
	TCGridObj* FindGridObjByObjId(UInt32 objId);

	/**
	*@brief С������Ѱ·
	*/
	void SquadReSearchWay(UInt32 squadId, UInt32 gridId, UInt32 objId);

	/**
	*@brief ��С�ӵ���Ϣ֪ͨ������С��
	*/
	void SendCurSquadDataToOtherSquad(TCSquadObj* squadObj);

	/**
	*@brief �������
	*/
	void TransferFieldActive();

	/**
	*@brief С�ӷ���ս��
	*/
	UInt32 SquadBattle(TCGridObj* battleObj, TCSquadObj* squadObj);

	/**
	*@brief С������ͬ��
	*/
	void SendSquadObjData();
	void SendSquadObjData(TCSquadObj* squadObj, bool isSendToOtherSquad = true);

	/**
	*@brief ͬ���ݵ�����
	*/
	void SendGridObjData(TCGridObj* fieldObj);

	/**
	*@brief ͬ����������
	*/
	void SendMonsterObjData(TCMonsterObj* monsterObj);

	/**
	*@brief ͬ�����ﴫ����
	*/
	void SendMonsterTransferObjData(TCMonsterTransferObj* mTransferObj);

	/**
	*@brief С�ӷ��س�����
	*/
	void SquadReturnBorn(TCSquadObj* squadObj);

	/**
	*@brief ���󱻻���
	*/
	void GridObjDestory(TCGridObj* destoryObj, bool isKill = true);

	/**
	*@brief ʵ����֮������
	*/
	void LaboratoryLink(TCLaboratoryObj* laboratory);

	/**
	*@brief �������
	*/
	void MakeMonster(UInt32 monsterType, const std::vector<UInt32>& gridVec);

	/**
	*@brief ������ﴫ����
	*/
	void MakeMonsterTransfer(UInt32 liveTime, const std::vector<UInt32>& gridVec);

	/**
	*@brief ѡ��һ���ո���
	*/
	UInt32 SelectNullGrid(const std::vector<UInt32>& gridVec);

	/**
	*@brief ���ӹ���
	*/
	void AddMonster(TCGridObj* newObj);

	/**
	*@brief ���Ӹ������
	*/
	void AddReviveCnt(UInt32 reviveCnt);

	/**
	*@brief �޸ľݵ��cd
	*/
	void UpdateGridObjCd(UInt32 squadRecudeCd, UInt32 monsterAddCd, UInt32 makeMonsterAddCd);

	/**
	*@brief �򿪼�̳
	*/
	void OpenAltar();

	/**
	*@brief ������̳�еı���
	*/
	void EndAltarRace(bool isPhaseEnd);

	/**
	*@brief ��̳���ݻ�
	*/
	void AltarDestory(TCGridObj* altarObj);

	/**
	*@brief �ݵ�����
	*/
	void FieldReborn(UInt32 objType, UInt32 objStatus);

	/**
	*@brief boss����
	*/
	void BossLink();

	/**
	*@brief ֪ͨ�ݵ�ı����˳�
	*/
	void FieldRaceEnd(TCGridObj* gridObj, UInt64 raceId, bool isVictory);

	/**
	*@brief ֪ͨ���б����˳�
	*/
	void AllRaceEnd();

	/**
	*@brief ���������ݵ�
	*/
	void AddDeadField(TCGridObj* deadObj);

	/**
	*@brief ���ɶ���id
	*/
	UInt32 GenObjId();

	/**
	*@brief �׶�boss����
	*/
	void PhaseBossEnd();
	UInt32 GetBossPhase() { return m_BossPhase; }

	/**
	*@brief ��ȡѰ··��
	*/
	std::list<UInt32> GetSearchPath(UInt32 srcGridId, UInt32 destGridId);

	/**
	*@brief boss�ؿ��Ƿ���ս��
	*/
	bool BossObjIsBattle();

	/**
	*@brief ����λ��
	*/
	UInt32 GetMainCityGridId() { return m_MainCityGridId; }

	/**
	*@brief �Ƿ���С���ڴ���ֻ����
	*/
	bool IsSquadAttackMonster(UInt32 monsterObjId);

	/**
	*@brief ���÷�������
	*/
	void SetPassType(UInt32 type);

private:

	/**
	*@brief �������Ӷ���
	*/
	TCGridObj* _CreateGridObj(TCGridObjType type);

	/**
	*@brief ����С�Ӷ���
	*/
	TCSquadObj* _FindSquadObj(UInt32 squadId);

	/**
	*@brief ���ҹ������
	*/
	TCMonsterObj* _FindMonsterObj(UInt32 monsterObjId);

	/**
	*@brief ������������
	*/
	TCGridObj* _FindDeadObjByGridId(UInt32 gridId);
	TCGridObj* _FindDeadObjByObjId(UInt32 deadObjId);
	void _GetDeadObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList);

	/**
	*@brief ��ȡ��������б�
	*/
	void _GetObjGridVec(std::set<UInt32>& gridSet);

	/**
	*@brief ����Ȩ��
	*/
	bool _OperatorPower(TeamCopyMember* member);

	/**
	*@brief С�Ӵ��͵�������
	*/
	void _SquadTransferToGrid(TCSquadObj* squadObj, UInt32 gridId);

	/**
	*@brief ��ȡ�����б�
	*/
	void _GetObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList);

	/**
	*@brief ���ƽ���
	*/
	void _FlopReward(UInt32 stage);

	/**
	*@brief �ݵ����ӱ���
	*/
	void _AddFieldRace(UInt32 fieldObjId, UInt64 raceId);

	/**
	*@brief ��þݵ����
	*/
	void _GetFieldRace(UInt32 fieldObjId, std::set<UInt64>& raceSet);

	/**
	*@brief �ݵ��Ƿ��б���
	*/
	bool _IsHasFieldRace(UInt32 fieldObjId);

	/**
	*@brief ���ӵĸ���������͵�����
	*/
	void _SendReviveCntToRace(UInt64 raceId);

	/**
	*@brief ������������
	*/
	void _LeaveReconnect();

	/**
	*@brief ��ȡС���ƶ�����
	*/
	UInt32 _GetSquadMoveCnt(UInt32 squadId);

private:

	// ��ͼ�϶���������
	UInt32 m_ObjSeed;

	// ���ӵĸ������
	UInt32 m_AddReviveCnt;

	// ��������
	TC2PassType m_PassType;

	// boss�ؿ��׶�
	UInt32 m_BossPhase;

	// �����ݵ�id
	UInt32 m_bossLinkFieldId;

	// ���ǵĸ���id
	UInt32 m_MainCityGridId;

	// 1�붨ʱ��
	Avalon::SimpleTimer m_SecTimer;

	// �ݵ����(key->���Ӷ���id)
	std::map<UInt32, TCGridObj*> m_GridObjMap;

	// ���ݻٵľݵ�(key->���Ӷ���id)
	std::map<UInt32, TCGridObj*> m_DeadObjMap;

	// С��ս������(key->С��id)
	std::map<UInt32, TCSquadObj*> m_SquadObjMap;

	// ��������(key->�������id)
	std::map<UInt32, TCMonsterObj*> m_MonsterObjMap;

	// �ݵ���ı���(key->�ݵ����id������id)
	std::map<UInt32, std::set<UInt64>> m_FieldRaceMap;
};

#endif // !_TCS_TEAM_TWO_H_


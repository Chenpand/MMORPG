#ifndef _TCS_GRID_OBJ_H_
#define _TCS_GRID_OBJ_H_

#include <memory>
#include <CLTeamCopyDefine.h>

struct TCSGridCfg;
class TCSTeamTwo;

/**
 *@brief �ű����Ӷ���(С�ӣ�������־ݵ�)
 */
class TCGridObj
{
public:
	TCGridObj();
	virtual ~TCGridObj() {}

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2) { return true; }

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now) {}

	/**
	 *@brief �Ƿ��ܹ���
	 */
	virtual bool IsCanAttack();

	/**
	 *@brief ׼��ս��
	 */
	virtual void PreBattle();

	/**
	 *@brief ս��
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief ������Ϣ
	 */
	virtual std::string GetObjStr();

	/**
	 *@brief �Ƿ�����
	 */
	virtual bool IsDead();

	/**
	 *@brief �����id
	 */
	UInt32 GetObjId() { return m_ObjId; }
	void SetObjId(UInt32 id) { m_ObjId = id; }

	/**
	 *@brief �������ڸ��ӵ�id
	 */
	UInt32 GetGridId() { return m_GridId; }
	void SetGridId(UInt32 id) { m_GridId = id; }

	/**
	 *@brief �ݵ�id
	 */
	UInt32 GetFieldId() { return m_FieldId; }
	void SetFieldId(UInt32 id) { m_FieldId = id; }

	/**
	 *@brief ��������������
	 */
	TCGridObjType GetObjType() { return m_ObjType; }

	/**
	 *@brief ����������״̬
	 */
	TCGridObjStatus GetObjStatus() { return m_ObjStatus; }
	void SetObjStatus(TCGridObjStatus status) { m_ObjStatus = status; }

	/**
	 *@brief ����ԭ��
	 */
	TCGridObjDeadReason GetObjDeadReason() { return m_ObjDeadReason; }
	void SetObjDeadReason(TCGridObjDeadReason reason) { m_ObjDeadReason = reason; }

	/**
	 *@brief ������������
	 */
	TCSTeamTwo* GetTeam() { return m_Team; }
	void SetTeam(TCSTeamTwo* team) { m_Team = team; }

protected:

	// ����id(������)
	UInt32 m_ObjId;

	// ���ڸ���id(�������ƶ�������id��仯)
	UInt32 m_GridId;

	// ��Ӧ�ݵ�
	UInt32 m_FieldId;

	// �����ж��������
	TCGridObjType m_ObjType;

	// �����ж����״̬
	TCGridObjStatus m_ObjStatus;

	// ����ԭ��
	TCGridObjDeadReason m_ObjDeadReason;

	// ��������
	TCSTeamTwo* m_Team;
};

/**
 *@brief С��
 */
class TCSquadObj : public TCGridObj
{
public:
	TCSquadObj();
	~TCSquadObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief �Ƿ��ܹ���
	 */
	virtual bool IsCanAttack();

	/**
	 *@brief ׼��ս��
	 */
	virtual void PreBattle();

	/**
	 *@brief ս��
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief �ƶ�
	 */
	bool Move(UInt32 targetGridId, UInt32 targetObjId, std::list<UInt32>& pathList);

	/**
	 *@brief ֹͣ�ƶ�
	 */
	void StopMove();

	/**
	 *@brief ����һ��λ��
	 */
	void SetPos(UInt32 gridId);

	/**
	 *@brief Ŀ������
	 */
	void TargetDead(bool isBattleObj);

	/**
	 *@brief �����ƶ�cd
	 */
	void ReduceMoveCD(UInt32 cd);

	/**
	 *@brief С��id
	 */
	void SetSquadId(UInt32 id) { m_SquadId = id; }
	UInt32 GetSquadId() { return m_SquadId; }

	/**
	 *@brief С�ӳ�����
	 */
	void SetBornGridId(UInt32 id) { m_BornGrid = id; }
	UInt32 GetBornGridId() { return m_BornGrid; }
	
	/**
	 *@brief Ŀ����ӵ�id
	 */
	void SetTargetGridId(UInt32 id) { m_TargetGridId = id; }
	UInt32 GetTargetGridId() { return m_TargetGridId; }

	/**
	 *@brief Ŀ������id
	 */
	void SetTargetObjId(UInt32 id) { m_TargetObjId = id; }
	UInt32 GetTargetObjId() { return m_TargetObjId; }

	/**
	 *@brief С���ڸ����ϵ�״̬
	 */
	void SetSquadObjStatus(TCGridSquadStatus status) { m_Status = status; }
	TCGridSquadStatus GetSquadObjStatus() { return m_Status; }

	/**
	 *@brief ���ù���ݵ�
	 */
	void SetBattleObj(TCGridObj* objPtr) { m_BattleObj = objPtr; }
	TCGridObj* GetBattleObj() { return m_BattleObj; }

	std::list<UInt32>& GetPathList() { return m_PathList; }
	UInt32 GetMoveCnt() { return m_MoveCnt; }

private:

	/**
	 *@brief �ƶ�����һ������
	 */
	void _MoveNextGrid();

private:

	// С��id
	UInt32 m_SquadId;

	// С���ڸ����ϵ�״̬
	TCGridSquadStatus m_Status;

	// �ƶ�cd
	UInt32 m_MoveCd;

	// �ƶ�cd����ʱ���
	UInt32 m_MoveEndStamp;

	// ���������
	UInt32 m_BornGrid;

	// Ŀ�����
	UInt32 m_TargetGridId;

	// Ŀ�����id
	UInt32 m_TargetObjId;

	// Ѱ··��
	std::list<UInt32> m_PathList;
	
	// �ƶ�����
	UInt32 m_MoveCnt;

	// ���ڹ���ľݵ�
	TCGridObj* m_BattleObj;
};

/**
 *@brief ����
 */
class TCMonsterObj : public TCGridObj
{
public:
	TCMonsterObj();
	~TCMonsterObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief ս��
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief ��ΪС�ӵ�Ŀ��
	 */
	void AddSquadTarget(UInt32 squadId);

	/**
	 *@brief ɾ��С��Ŀ��
	 */
	void RemoveSquadTarget(UInt32 squadId);

	/**
	 *@brief �����ƶ�cd
	 */
	void AddMoveCD(UInt32 addCd) { m_MoveCd += addCd; }

	const std::list<UInt32>& GetMovePath() { return m_MovePathList; }

private:

	/**
	 *@brief �ƶ�һ��
	 */
	void _MoveNextGrid();

private:

	// �ƶ�cd
	UInt32 m_MoveCd;

	// �ƶ�����ʱ��
	UInt32 m_MoveEndStamp;

	// ���Բ���id
	UInt32 m_ProId;

	// �Ƿ��ʼ·��
	bool m_IsInitPath;

	// �ƶ�·��(���ù̶�·��)
	std::list<UInt32> m_MovePathList;

	// ����ЩС��Ŀ��
	std::set<UInt32> m_SquadTargetSet;
};

/**
 *@brief ������
 */
class TCTransferObj : public TCGridObj
{
public:
	TCTransferObj();
	~TCTransferObj();

	/**
	*@brief ��ʼ������
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief ����С��
	*/
	void TransferSquad(TCSquadObj* squadObj);

private:

	std::vector<UInt32> m_TransferGridVec;
};

/**
 *@brief ǰ��վ
 */
class TCFixPointObj : public TCGridObj
{
public:
	TCFixPointObj();
	~TCFixPointObj();

	/**
	*@brief ��ʼ������
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief ��ȡ����
	*/
	virtual void GetObjData(TCGridObjData& data);

	/**
	*@brief ��������
	*/
	virtual void BattleEnd(bool isDestory);

private:

	// Ѫ��
	UInt32 m_Blood;
};

/**
 *@brief ����վ�����ƺ��״̬�ǿ��Դ��͵ģ�
 */
class TCEnergyPointObj : public TCGridObj
{
public:
	TCEnergyPointObj();
	~TCEnergyPointObj();

	/**
	*@brief ��ʼ������
	*/
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	*@brief ��ȡ����
	*/
	virtual void GetObjData(TCGridObjData& data);

	/**
	*@brief tick
	*/
	virtual void OnTick(const Avalon::Time& now);

	/**
	*@brief ��������
	*/
	virtual void BattleEnd(bool isDestory);

	/**
	*@brief ����С��
	*/
	void TransferSquad(TCSquadObj* squadObj);

	/**
	*@brief ������ﴫ������Ϣ
	*/
	void ClearMonsterTransferData();

private:

	// ���ﴫ����ʱ���
	UInt32 m_MakeMonsterTransferStamp;
	// ���ﴫ���ŵ�cd
	UInt32 m_MakeMonsterTransferCd;

	// ���ﴫ���ŵ�����ʱ��
	UInt32 m_MonsterTransferLiveTime;
	
	// ���ﴫ���ŵĵ�λ
	std::vector<UInt32> m_MonsterGridVec;
	// ���͵�
	std::vector<UInt32> m_TransferGridVec;
};

/**
 *@brief ����̳
 */
class TCAltarObj : public TCGridObj
{
public:
	TCAltarObj();
	~TCAltarObj();

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief �Ƿ�����
	 */
	virtual bool IsDead();
};

/**
 *@brief �ֲ�������̳
 */
class TCTerrorAltarObj : public TCGridObj
{
public:
	TCTerrorAltarObj();
	~TCTerrorAltarObj();

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief �Ƿ�����
	 */
	virtual bool IsDead();
};

/**
 *@brief boss
 */
class TCBossObj : public TCGridObj
{
public:
	TCBossObj();
	~TCBossObj();

	/**
	 *@brief ս��
	 */
	virtual void Battle(UInt32 squadId);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);
};

/**
 *@brief ����
 */
class TCMainCityObj : public TCGridObj
{
public:
	TCMainCityObj();
	~TCMainCityObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief ��Ѫ
	 */
	void CostBlood();
	UInt32 GetBlood() { return m_Blood; }

private:

	// Ѫ��
	UInt32 m_Blood;
};

/**
 *@brief ���ʲ���
 */
class TCReviveCntObj : public TCGridObj
{
public:
	TCReviveCntObj();
	~TCReviveCntObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief �Ƿ�����
	 */
	virtual bool IsDead();

	/**
	 *@brief �������
	 */
	UInt32 GetReviveCnt() { return m_ReviveCnt; }

private:
	
	// �������
	UInt32 m_ReviveCnt;
};

/**
 *@brief ��������
 */
class TCCdDownObj : public TCGridObj
{
public:
	TCCdDownObj();
	~TCCdDownObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief �Ƿ�����
	 */
	virtual bool IsDead();

private:

	// С��������cd
	UInt32 m_SquadReduceCd;
	// �������ӵ�cd
	UInt32 m_MonsterAddCd;
	// ��Ӫ���ӵ��������cd
	UInt32 m_MakeMonsterAddCd;
};

/**
 *@brief �������
 */
class TCLaboratoryObj : public TCGridObj
{
public:
	TCLaboratoryObj();
	~TCLaboratoryObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

	/**
	 *@brief ��������
	 */
	virtual void BattleEnd(bool isDestory);

	/**
	 *@brief ���Ӹ���cd
	 */
	void AddReviveCd();

	/**
	 *@brief ���ӹ�������cd
	 */
	void AddMakeMonsterCd(UInt32 cd) { m_MakeMonsterCD += cd; }

	/**
	 *@brief �Ƿ����ڸ���
	 */
	bool IsInRevive() { return m_ReviveEndStamp != 0; }

private:

	// ����cd
	UInt32 m_ReviceCD;
	// �������cd
	UInt32 m_MakeMonsterCD;
	// ���ӵĸ���ʱ��
	UInt32 m_AddReviveCD;
	// �������ʱ��
	UInt32 m_ReviveEndStamp;
	// ����������ʱ��
	UInt32 m_MakeMonsterEndStamp;
	// ���������
	std::vector<UInt32> m_MonsterBornGridVec;
};

/**
 *@brief ���ﴫ����
 */
class TCMonsterTransferObj : public TCGridObj
{
public:
	TCMonsterTransferObj();
	~TCMonsterTransferObj();

	/**
	 *@brief ��ʼ������
	 */
	virtual bool InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2);

	/**
	 *@brief ��ȡ����
	 */
	virtual void GetObjData(TCGridObjData& data);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

private:

	// ���ﴫ���ŵ�����ʱ���
	UInt32 m_MonsterTransferLiveStamp;
	// ���ﴫ���ŵ�����ʱ��
	UInt32 m_MonsterTransferLiveTime;
};

#endif // !_TCS_GRID_OBJ_H_


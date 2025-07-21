#ifndef _SEASON_LEVEL_H_
#define _SEASON_LEVEL_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLSeasonDefine.h>
#include <CLSeasonDataEntry.h>
#include <CLGameSessionProtocol.h>
#include <CLMatchDefine.h>

class Player;

class SeasonUplevelBattle : public CLComplexProperty
{
public:
	SeasonUplevelBattle(){};
	~SeasonUplevelBattle(){};

public:
	UInt32 GetBattleWinSize();
	UInt32 GetBattleLoseSize();
	UInt32 GetBattleRecordSize();
	void IncBattleRecord(_resultBitmask result);
	void Clear();

	std::vector<UInt8> GetBattleRecords() { return m_BattleRecord; };

	void SetBattleRecrods(std::vector<UInt8> vecRecords) { m_BattleRecord = vecRecords; }

public:
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void ClearInvalidData();

private:
	std::vector<UInt8> m_BattleRecord;

};

class SeasonLevel
{
public:
	SeasonLevel() : m_SeasonLevelDataEntry(NULL) {}
	~SeasonLevel(){}

public:
	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	void Init();

	/**
	*@brief ��ȡ����id
	*/
	ObjUInt32& GetSeasonId() { return m_SeasonId; }

	/**
	*@brief ��ȡԭ������λ
	*/
	//ObjUInt32& GetOldSeasonLevel() { return m_OldSeasonLevel; }

	/**
	*@brief ��ȡ��������λ
	*/
	//ObjUInt32& GetOldSeasonStar() { return m_OldSeasonStar; }

	/**
	*@brief ��ȡ������λ
	*/
	ObjUInt32& GetSeasonLevel() { return m_SeasonLevel; }

	/**
	*@brief ��ȡ�����Ǽ�
	*/
	ObjUInt32& GetSeasonStar() { return m_SeasonStar; }

	/**
	*@brief ��ȡ��������
	*/
	ObjUInt32& GetSeasonExp() { return m_SeasonExp; }

	/**
	*@brief ��ȡ��λ�仯ʱ��
	*/
	ObjUInt32& GetSeasonChangeTime() { return m_SeasonChangeTime; }

	/**
	*@brief ��ȡ��������״̬
	*/
	ObjUInt8& GetSeasonPlayStatus() { return m_SeasonPlayStatus; }

	/**
	*@brief ��ȡ����ս��
	*/
	SeasonUplevelBattle& GetSeasonUplevelBattle() { return m_SeasonUplevelBattle; }

	/**
	*@brief ��ȡ��������id
	*/
	ObjUInt8& GetSeasonAttr() { return m_SeasonAttr; }

	/**
	*@brief ��ȡ����ӡ��
	*/
	ObjUInt16& GetSeasonKingMarkCount() { return m_SeasonKingMarkCount; }

	/**
	*@brief ��ȡ�������Ե���ʱ��
	*/
	ObjUInt32& GetChangeSeasonAttrTime() { return m_ChangeSeasonAttrTime; }

	/**
	*@brief ��ȡ�������λ
	*/
	SeasonMainLevel GetSeasonMainLevel(UInt32 level) { return (SeasonMainLevel)(level / 10000); }

	/**
	*@brief ��ȡ����С��λ
	*/
	UInt32 GetSeasonSmallLevel(UInt32 level) { return level / 100; }

public:
	void ResetSeasonPlayStatus();

	void OnOnline();

	void OnTick(Avalon::Time now);

	void OnDayChange();

	void OnRaceEnd(const CLProtocol::MatchServerRaceEnd& raceEnd, Int32& changeSeasonExp);

	void OnRoomRaceEnd(const Pk3V3PlayerRaceEndInfo& playerInfo, Int32& changeSeasonExp);

	void IncExp(UInt32 exp, bool isUplevelBattle = false);

	void DecExp(UInt32 exp);

	void UpLevel();

	void DownLevel(UInt32 level);

	//�������
	void UpdateSeason(UInt32 nowSeasonId);

	//���ڵ���
	void DayDownLevel(UInt32 intervalDay);

	//���ö�λ
	bool SetSeasonLevel(UInt32 seasonLevel, UInt32 seasonStar = 0);

	//���ý�����
	bool SetSeasonUplevelBattleRecord(std::vector<UInt8> vecRecords);

	//������������
	void SetSeasonAttr(UInt32 changeTime, UInt8 attr = 0);

	//����������Ϣ
	void UpdateSeasonInfo();

	//������������
	UInt32 CalculationSeasonExp(UInt32 winSeasonLevel, UInt32 loseSeasonLevel);


private:
	void _ResetBattleCount();

	void _IncBattleCount();

	UInt32 _GetBattleCount();

	void _UpdateSortList();

	void _SendSeasonInfo();

	void _SendSeasonReward();

	void _SetMaxSeasonLevel();

	void _ChangeMaxSeasonLevel();

	UInt32 _GetMaxSeasonLevel();

	void _IncSeasonWinStreak();

	void _ResetSeasonWinStreak();

	UInt32 _GetSeasonWinStreak();

	void _SetSeasonAttrLevel();

	UInt32 _GetSeasonAttrLevel();

	//��λ�仯
	void _OnChangeSeasonLevel(UInt32 oldSeasonLevel, UInt32 oldSeasonStar);

	//С��λ�仯
	void _OnChangeSmallSeasonLevel(UInt32 oldSeasonLevel);

	//������������
	UInt32 _GetResetMatchScore(UInt32 oldScore);

	//���������
	bool _CheckSeasonUplevel(_resultBitmask resultFlag, Int32& changeSeasonExp);

	//��������ӡ��
	void _IncSeasonKingMark();

private:

	//ӵ����
	Player*			m_pOwner;

	// ������λ
	ObjUInt32		m_SeasonLevel;

	// �����Ǽ�
	ObjUInt32		m_SeasonStar;

	// ��������
	ObjUInt32		m_SeasonExp;

	// ��������״̬
	ObjUInt8		m_SeasonPlayStatus;

	// ��������
	ObjUInt32			m_SeasonId;

	// ��λ�仯ʱ��
	ObjUInt32			m_SeasonChangeTime;

	//��������id
	ObjUInt8			m_SeasonAttr;

	//�������Խ���ʱ��
	ObjUInt32			m_ChangeSeasonAttrTime;

	//����ӡ��
	ObjUInt16			m_SeasonKingMarkCount;

	//������ս��״��
	SeasonUplevelBattle m_SeasonUplevelBattle;

	// ������λ������
	SeasonLevelDataEntry*	m_SeasonLevelDataEntry;
};


#endif //_SEASON_LEVEL_H_
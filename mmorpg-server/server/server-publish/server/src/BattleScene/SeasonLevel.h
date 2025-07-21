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
	*@brief 获取赛季id
	*/
	ObjUInt32& GetSeasonId() { return m_SeasonId; }

	/**
	*@brief 获取原赛季段位
	*/
	//ObjUInt32& GetOldSeasonLevel() { return m_OldSeasonLevel; }

	/**
	*@brief 获取现赛季段位
	*/
	//ObjUInt32& GetOldSeasonStar() { return m_OldSeasonStar; }

	/**
	*@brief 获取赛季段位
	*/
	ObjUInt32& GetSeasonLevel() { return m_SeasonLevel; }

	/**
	*@brief 获取赛季星级
	*/
	ObjUInt32& GetSeasonStar() { return m_SeasonStar; }

	/**
	*@brief 获取赛季经验
	*/
	ObjUInt32& GetSeasonExp() { return m_SeasonExp; }

	/**
	*@brief 获取段位变化时间
	*/
	ObjUInt32& GetSeasonChangeTime() { return m_SeasonChangeTime; }

	/**
	*@brief 获取赛季播放状态
	*/
	ObjUInt8& GetSeasonPlayStatus() { return m_SeasonPlayStatus; }

	/**
	*@brief 获取赛季战斗
	*/
	SeasonUplevelBattle& GetSeasonUplevelBattle() { return m_SeasonUplevelBattle; }

	/**
	*@brief 获取赛季属性id
	*/
	ObjUInt8& GetSeasonAttr() { return m_SeasonAttr; }

	/**
	*@brief 获取王者印记
	*/
	ObjUInt16& GetSeasonKingMarkCount() { return m_SeasonKingMarkCount; }

	/**
	*@brief 获取赛季属性到期时间
	*/
	ObjUInt32& GetChangeSeasonAttrTime() { return m_ChangeSeasonAttrTime; }

	/**
	*@brief 获取赛季大段位
	*/
	SeasonMainLevel GetSeasonMainLevel(UInt32 level) { return (SeasonMainLevel)(level / 10000); }

	/**
	*@brief 获取赛季小段位
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

	//赛季变更
	void UpdateSeason(UInt32 nowSeasonId);

	//日期掉段
	void DayDownLevel(UInt32 intervalDay);

	//设置段位
	bool SetSeasonLevel(UInt32 seasonLevel, UInt32 seasonStar = 0);

	//设置晋级赛
	bool SetSeasonUplevelBattleRecord(std::vector<UInt8> vecRecords);

	//设置赛季属性
	void SetSeasonAttr(UInt32 changeTime, UInt8 attr = 0);

	//更新赛季信息
	void UpdateSeasonInfo();

	//计算赛季经验
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

	//段位变化
	void _OnChangeSeasonLevel(UInt32 oldSeasonLevel, UInt32 oldSeasonStar);

	//小段位变化
	void _OnChangeSmallSeasonLevel(UInt32 oldSeasonLevel);

	//重置赛季积分
	UInt32 _GetResetMatchScore(UInt32 oldScore);

	//计算晋级赛
	bool _CheckSeasonUplevel(_resultBitmask resultFlag, Int32& changeSeasonExp);

	//增加王者印记
	void _IncSeasonKingMark();

private:

	//拥有者
	Player*			m_pOwner;

	// 赛季段位
	ObjUInt32		m_SeasonLevel;

	// 赛季星级
	ObjUInt32		m_SeasonStar;

	// 赛季经验
	ObjUInt32		m_SeasonExp;

	// 赛季播放状态
	ObjUInt8		m_SeasonPlayStatus;

	// 所在赛季
	ObjUInt32			m_SeasonId;

	// 段位变化时间
	ObjUInt32			m_SeasonChangeTime;

	//赛季属性id
	ObjUInt8			m_SeasonAttr;

	//赛季属性结束时间
	ObjUInt32			m_ChangeSeasonAttrTime;

	//王者印记
	ObjUInt16			m_SeasonKingMarkCount;

	//晋级赛战斗状况
	SeasonUplevelBattle m_SeasonUplevelBattle;

	// 赛季段位数据项
	SeasonLevelDataEntry*	m_SeasonLevelDataEntry;
};


#endif //_SEASON_LEVEL_H_
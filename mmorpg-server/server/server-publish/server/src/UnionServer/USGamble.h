#ifndef __US_GAMBLE_H__
#define __US_GAMBLE_H__

#include <AvalonSingleton.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>
#include <CLChampionDataEntry.h>
#include <CLChampionProtocol.h>

enum GambleType
{
	GT_CHAMPION = 1,				//猜冠军
	GT_1V1 = 2,						//猜单场胜负
	GT_BATTLE_COUNT = 3,			//猜总比赛数
	GT_CHAMPION_BATTLE_SCORE = 4,	//猜总决赛比分
};

/**
*@brief 押注选项信息
*/
class GambleOptionInfo
{
public:
	/**
	*@brief 初始化
	*/
	void Init(UInt64 option);
	/**
	*@brief 压入押注记录
	*/
	void Push(const UInt64 roleID, const UInt64 num);
	/**
	*@brief 获得本选项总金额
	*/
	const UInt64 GetTotalNum() const;
	/**
	*@brief 获取本选项所有玩家以及该玩家押注金额
	*/
	std::map<UInt64, UInt64>& GetPlayerBet();
	/**
	*@brief 设置赔率
	*/
	void SetOdds(UInt32 odds) { m_Data.odds = odds; }
	/**
	*@brief 获得赔率
	*/
	UInt32 GetOdds() const { return m_Data.odds; }
	/**
	*@brief 获得数据
	*/
	const ChampionGambleOption& GetData() const;
private:
	//key是玩家id，value是下注金额
	std::map<UInt64, UInt64> m_PlayerBet;
	//数据,可以用来同步
	ChampionGambleOption m_Data;
};

class GambleProjectBase : public CLSyncObject
{
public:
	GambleProjectBase(UInt32 id);
	virtual ~GambleProjectBase() {};
	/**
	*@brief 初始化
	*/
	virtual void Init();
	/**
	*@brief 获取本项目类型
	*/
	virtual const GambleType GetType() const = 0;
	/**
	*@brief 添加并获得选项信息
	*/
	GambleOptionInfo& AddInfo(const UInt64 option);
	/**
	*@brief 设置所有选项
	*/
	void SetOption(const std::set<UInt64>& optionSet);
	/**
	*@brief 是否有该选项
	*/
	const bool HasOption(const UInt64 option) const;
	/**
	*@brief 获取所有选项
	*/
	const std::map<UInt64, GambleOptionInfo>& GetOptions() const;
	/**
	*@brief 设置本项目对应配置项
	*/
	void SetDataEntry(ChampionGambleDataEntry* dataEntry);
	/**
	*@brief 获取本项目配置项
	*/
	const ChampionGambleDataEntry& GetDataEntry();
	/**
	*@brief 本项目结算
	*/
	void OnResult(const UInt64 winOption);
	/**
	*@brief 压入押注记录
	*/
	void Push(UInt64 roleID, UInt64 option, UInt64 num);
	/**
	*@brief 获取本项目总金额
	*/
	const UInt64 GetTotalNum() const;
	/**
	*@brief 是否开放押注
	*/
	bool IsOpen();
	/**
	*@brief 获取项目id
	*/
	const UInt32 GetProjectID() const;
	/**
	*@brief 获取关联组id
	*/
	const UInt32 GetGroupID() const;
	/**
	*@brief 设置关联组id
	*/
	void SetGroupID(UInt32 id);
	/**
	*@brief 获取胜利选项
	*/
	UInt64 GetWinOption() const;
	/**
	*@brief 该账号是否可以下注
	*/
	bool CanAccountBet(UInt32 accid);
protected:
	//key是下注选项option，value是选项信息
	std::map<UInt64, GambleOptionInfo> m_GambleInfoMap;
	ChampionGambleDataEntry* m_DataEntry = nullptr;
	UInt64 m_TotalNum = 0;
public:
	void SaveToDB();
	void InsertToDB();
	void SaveOptionToString();
	void LoadOptionFromString();
public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("project_id", m_ProjectID)
		CL_DBPROPERTY("type", m_Type)
		CL_DBPROPERTY("group_id", m_GroupID)
		CL_DBPROPERTY("options", m_OptionsStr)
		CL_DBPROPERTY("win_option", m_WinOption)
	CL_DBPROPERTY_END()
protected:
	ObjUInt32 m_ProjectID;
	//关联的比赛组id
	ObjUInt32 m_GroupID;
	ObjString m_OptionsStr;
	ObjUInt32 m_Type;
	ObjUInt64 m_WinOption;
};

class GambleProjectChampion : public GambleProjectBase
{
public:
	GambleProjectChampion(UInt32 id) : GambleProjectBase(id) {}
	virtual ~GambleProjectChampion() {};
	virtual const GambleType GetType() const { return GT_CHAMPION; }

};

class GambleProject1V1 : public GambleProjectBase
{
public:
	GambleProject1V1(UInt32 id) : GambleProjectBase(id) {}
	virtual ~GambleProject1V1() {};
	virtual const GambleType GetType() const { return GT_1V1; }

};

class GambleProjectBattleCount : public GambleProjectBase
{
public:
	GambleProjectBattleCount(UInt32 id) : GambleProjectBase(id) {}
	virtual ~GambleProjectBattleCount() {};
	virtual const GambleType GetType() const { return GT_BATTLE_COUNT; }

};


class GambleProjectChampionScore : public GambleProjectBase
{
public:
	GambleProjectChampionScore(UInt32 id) : GambleProjectBase(id) {}
	virtual ~GambleProjectChampionScore() {};
	virtual const GambleType GetType() const { return GT_CHAMPION_BATTLE_SCORE; }
};



class GambleRecord : public CLSyncObject
{
public:
	GambleRecord();
	void SaveToDB();
	void InsterToDB();
	void LoadDBData(CLRecordCallback* callback);

public:
	inline const UInt32 GetTime() const { return m_Time; }
	inline void SetTime(const UInt32 time) { m_Time = time; }
	inline const UInt64 GetRoleID() const { return m_RoleID; }
	inline void SetRoleID(const UInt64 id) { m_RoleID = id; }
	inline const UInt32 GetType() const { return m_Type; }
	inline void SetType(const UInt32 type) { m_Type = type; }
	inline const UInt64 GetOption() const { return m_Option; }
	inline void SetOption(const UInt64 option) { m_Option = option; }
	inline const UInt64 GetNum() const { return m_Num; }
	inline void SetNum(const UInt64 num) { m_Num = num; }
	inline const UInt32 GetGmableID() const { return m_GmableID; }
	inline void SetGmableID(const UInt32 id) { m_GmableID = id; }
	inline const UInt64 GetReward() const { return m_Reward; }
	inline void SetReward(const UInt64 num) { m_Reward = num; }
	inline const UInt32 GetZoneID() const { return m_ZoneID; }
	inline void SetZoneID(const UInt32 id) { m_ZoneID = id; }
public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("id", m_GmableID)
		CL_DBPROPERTY("type", m_Type)
		CL_DBPROPERTY("time", m_Time)
		CL_DBPROPERTY("role_id", m_RoleID)
		CL_DBPROPERTY("option", m_Option)
		CL_DBPROPERTY("num", m_Num)
		CL_DBPROPERTY("reward",m_Reward)
		CL_DBPROPERTY("zone_id", m_ZoneID)
	CL_DBPROPERTY_END()
private:
	ObjUInt32 m_GmableID;
	ObjUInt32 m_Time;
	ObjUInt64 m_RoleID;
	ObjUInt32 m_Type;
	ObjUInt64 m_Option;
	ObjUInt64 m_Num;
	ObjUInt64 m_Reward;
	ObjUInt32 m_ZoneID;
};

struct GambleRecodKey
{
	UInt64 roleID;
	UInt32 id;
	UInt64 option;
	const bool operator<(const GambleRecodKey& rsh) const
	{
		if (roleID < rsh.roleID)
		{
			return true;
		}
		else if (roleID == rsh.roleID)
		{
			if (id < rsh.id)
			{
				return true;
			}
			else if (id == rsh.id)
			{
				if (option < rsh.option)
				{
					return true;
				}
			}
		}
		return false;
	}
};

class USGambleMgr : public Avalon::Singleton<USGambleMgr>
{
public:
	/**
	*@brief 生成项目id，弃用
	*/
	UInt32 GetNewProjectID();
	/**
	*@brief 初始化
	*/
	const bool Init();
	/**
	*@brief 清除所有信息
	*/
	void Clear();
	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);
	/**
	*@brief 加载押注记录
	*/
	void OnLoadData(CLRecordCallback* cb);
	/**
	*@brief 加载竞猜项目
	*/
	void OnLoadProjectData(CLRecordCallback* cb);
	/**
	*@brief 添加押注记录
	*/
	GambleRecord* AddRecord(UInt32 zoneID, UInt64 roleID, UInt32 id, UInt64 option, UInt64 num);
	/**
	*@brief 获取押注记录
	*/
	GambleRecord* FindRecord(UInt64 roleID, UInt32 id, UInt64 option);
	/**
	*@brief 生成项目
	*/
	GambleProjectBase* GenProject(UInt32 id, GambleType type);
	/**
	*@brief 弃用
	*/
	void OnEvent(UInt32 id, UInt64 Option);
	/**
	*@brief 同步项目信息
	*/
	void SyncGamebleInfo();
	/**
	*@brief 获取同步消息
	*/
	CLProtocol::SceneChampionAllGambleInfoRes GetSyncMsg();
	/**
	*@brief 当玩家下注
	*/
	UInt32 OnPlayerBet(UInt32 zoneID, UInt64 roldID, UInt32 accid, UInt32 id, UInt64 option, UInt32 coin);
	/**
	*@brief 当一组战斗结束
	*/
	void OnGroupBattleEnd(UInt32 groupID, UInt64 option);
	/**
	*@brief 获取玩家所有下注记录
	*/
	std::vector<GambleRecord*> GetRecord(UInt64 roleID);
	/**
	*@brief 获取项目
	*/
	GambleProjectBase* GetProject(UInt32 id) const;

	/**
	*@brief 获取所有玩家所有下注记录
	*/
	std::vector<GambleRecord*> GetAllRecord();
	
private:
	//key是竞猜id，value是对应的竞猜项目
	std::map<UInt32, GambleProjectBase*> m_GambleProjectMap;
	//玩家下注记录 玩家id->项目id->选项id->下注记录
	std::map<UInt64, std::map<UInt32, std::map<UInt64, GambleRecord*>>> m_GambleRecordMap;
	// 保存数据库定时器
	Avalon::SimpleTimer	m_SaveTimer;
	UInt32 m_GamebleProjectID = 0;
	Avalon::SimpleTimer m_SyncTimer;
};



#endif



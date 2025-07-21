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
	GT_CHAMPION = 1,				//�¹ھ�
	GT_1V1 = 2,						//�µ���ʤ��
	GT_BATTLE_COUNT = 3,			//���ܱ�����
	GT_CHAMPION_BATTLE_SCORE = 4,	//���ܾ����ȷ�
};

/**
*@brief Ѻעѡ����Ϣ
*/
class GambleOptionInfo
{
public:
	/**
	*@brief ��ʼ��
	*/
	void Init(UInt64 option);
	/**
	*@brief ѹ��Ѻע��¼
	*/
	void Push(const UInt64 roleID, const UInt64 num);
	/**
	*@brief ��ñ�ѡ���ܽ��
	*/
	const UInt64 GetTotalNum() const;
	/**
	*@brief ��ȡ��ѡ����������Լ������Ѻע���
	*/
	std::map<UInt64, UInt64>& GetPlayerBet();
	/**
	*@brief ��������
	*/
	void SetOdds(UInt32 odds) { m_Data.odds = odds; }
	/**
	*@brief �������
	*/
	UInt32 GetOdds() const { return m_Data.odds; }
	/**
	*@brief �������
	*/
	const ChampionGambleOption& GetData() const;
private:
	//key�����id��value����ע���
	std::map<UInt64, UInt64> m_PlayerBet;
	//����,��������ͬ��
	ChampionGambleOption m_Data;
};

class GambleProjectBase : public CLSyncObject
{
public:
	GambleProjectBase(UInt32 id);
	virtual ~GambleProjectBase() {};
	/**
	*@brief ��ʼ��
	*/
	virtual void Init();
	/**
	*@brief ��ȡ����Ŀ����
	*/
	virtual const GambleType GetType() const = 0;
	/**
	*@brief ��Ӳ����ѡ����Ϣ
	*/
	GambleOptionInfo& AddInfo(const UInt64 option);
	/**
	*@brief ��������ѡ��
	*/
	void SetOption(const std::set<UInt64>& optionSet);
	/**
	*@brief �Ƿ��и�ѡ��
	*/
	const bool HasOption(const UInt64 option) const;
	/**
	*@brief ��ȡ����ѡ��
	*/
	const std::map<UInt64, GambleOptionInfo>& GetOptions() const;
	/**
	*@brief ���ñ���Ŀ��Ӧ������
	*/
	void SetDataEntry(ChampionGambleDataEntry* dataEntry);
	/**
	*@brief ��ȡ����Ŀ������
	*/
	const ChampionGambleDataEntry& GetDataEntry();
	/**
	*@brief ����Ŀ����
	*/
	void OnResult(const UInt64 winOption);
	/**
	*@brief ѹ��Ѻע��¼
	*/
	void Push(UInt64 roleID, UInt64 option, UInt64 num);
	/**
	*@brief ��ȡ����Ŀ�ܽ��
	*/
	const UInt64 GetTotalNum() const;
	/**
	*@brief �Ƿ񿪷�Ѻע
	*/
	bool IsOpen();
	/**
	*@brief ��ȡ��Ŀid
	*/
	const UInt32 GetProjectID() const;
	/**
	*@brief ��ȡ������id
	*/
	const UInt32 GetGroupID() const;
	/**
	*@brief ���ù�����id
	*/
	void SetGroupID(UInt32 id);
	/**
	*@brief ��ȡʤ��ѡ��
	*/
	UInt64 GetWinOption() const;
	/**
	*@brief ���˺��Ƿ������ע
	*/
	bool CanAccountBet(UInt32 accid);
protected:
	//key����עѡ��option��value��ѡ����Ϣ
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
	//�����ı�����id
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
	*@brief ������Ŀid������
	*/
	UInt32 GetNewProjectID();
	/**
	*@brief ��ʼ��
	*/
	const bool Init();
	/**
	*@brief ���������Ϣ
	*/
	void Clear();
	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);
	/**
	*@brief ����Ѻע��¼
	*/
	void OnLoadData(CLRecordCallback* cb);
	/**
	*@brief ���ؾ�����Ŀ
	*/
	void OnLoadProjectData(CLRecordCallback* cb);
	/**
	*@brief ���Ѻע��¼
	*/
	GambleRecord* AddRecord(UInt32 zoneID, UInt64 roleID, UInt32 id, UInt64 option, UInt64 num);
	/**
	*@brief ��ȡѺע��¼
	*/
	GambleRecord* FindRecord(UInt64 roleID, UInt32 id, UInt64 option);
	/**
	*@brief ������Ŀ
	*/
	GambleProjectBase* GenProject(UInt32 id, GambleType type);
	/**
	*@brief ����
	*/
	void OnEvent(UInt32 id, UInt64 Option);
	/**
	*@brief ͬ����Ŀ��Ϣ
	*/
	void SyncGamebleInfo();
	/**
	*@brief ��ȡͬ����Ϣ
	*/
	CLProtocol::SceneChampionAllGambleInfoRes GetSyncMsg();
	/**
	*@brief �������ע
	*/
	UInt32 OnPlayerBet(UInt32 zoneID, UInt64 roldID, UInt32 accid, UInt32 id, UInt64 option, UInt32 coin);
	/**
	*@brief ��һ��ս������
	*/
	void OnGroupBattleEnd(UInt32 groupID, UInt64 option);
	/**
	*@brief ��ȡ���������ע��¼
	*/
	std::vector<GambleRecord*> GetRecord(UInt64 roleID);
	/**
	*@brief ��ȡ��Ŀ
	*/
	GambleProjectBase* GetProject(UInt32 id) const;

	/**
	*@brief ��ȡ�������������ע��¼
	*/
	std::vector<GambleRecord*> GetAllRecord();
	
private:
	//key�Ǿ���id��value�Ƕ�Ӧ�ľ�����Ŀ
	std::map<UInt32, GambleProjectBase*> m_GambleProjectMap;
	//�����ע��¼ ���id->��Ŀid->ѡ��id->��ע��¼
	std::map<UInt64, std::map<UInt32, std::map<UInt64, GambleRecord*>>> m_GambleRecordMap;
	// �������ݿⶨʱ��
	Avalon::SimpleTimer	m_SaveTimer;
	UInt32 m_GamebleProjectID = 0;
	Avalon::SimpleTimer m_SyncTimer;
};



#endif



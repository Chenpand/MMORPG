#ifndef __AVENTURE_PASS_INFO_H__
#define __AVENTURE_PASS_INFO_H__

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <bitset>
#include <AvalonSimpleTimer.h>
#include <CLRecordClient.h>
#include <CLAdventurePassDataEntry.h>

class WSPlayer;

const UInt32 ADVENTURE_PASS_MAX_LEVEL = 300;

enum AdventurePassType
{
	ADVENTURE_PASS_TYPE_NORMAL = 0,
	ADVENTURE_PASS_TYPE_KING = 1,
	//ADVENTURE_PASS_TYPE_TOPKING = 2,
};

enum AdventurePassExtraExpType
{
	ADVENTURE_PASS_EXTRA_EXP_TYPE_UNLOCK = 0,
	ADVENTURE_PASS_EXTRA_EXP_TYPE_UNUSE = 1,
	ADVENTURE_PASS_EXTRA_EXP_TYPE_USE = 2,
};



class AdventurePassInfo : public CLSyncObject
{
public:
	AdventurePassInfo(UInt32 accid);
	~AdventurePassInfo();

	void OnTick(const AdventurePassSeasonDataEntry* season, const Avalon::Time &now);
	void OnWeekChange();
	void OnDayChanged();
	void Reset(WSPlayer *player, UInt32 seasonID);
	void LoadData(CLRecordCallback* callback);
	void InsertToDB();
	void OnOnline(WSPlayer *player);
	void SendResetMail(WSPlayer *player);

public:
	std::string GetNormalRewardStatus() { return m_NormalReward; } 
	void SetNormalReward(UInt32 lv);
	UInt32 GetNormalRewardNum(UInt32 lv);
	std::string GetHighRewardStatus() { return m_HighReward; }
	void SetHighReward(UInt32 lv);
	UInt32 GetHighRewardNum(UInt32 lv);

	inline UInt32 GetAccid() { return m_Accid; }
	inline void SetAccid(UInt32 accid) { m_Accid = accid; }
	inline UInt32 GetLevel() { return m_Level; }
	void SetLevel(UInt32 lv);
	inline UInt8 GetPassType() { return m_PassType; }
	void SetPassType(UInt32 type);
	inline UInt32 GetExp() { return m_Exp; }
	bool IncExp(UInt32 exp);
	inline UInt8 GetActivity() { return m_Activity; }
	inline void SetActivity(UInt32 activity) { m_Activity = activity; }
	inline bool IsUnLock() { return m_Unlock == 1 ? true : false; }
	void UnLock();
	inline UInt32 GetSeasonID() { return m_SeasonID; }
	inline AdventurePassExtraExpType GetExtraExp() { return static_cast<AdventurePassExtraExpType>(UInt8(m_ExtraExp)); }
	inline void SetExtraExp(AdventurePassExtraExpType type) { m_ExtraExp = type; }
	UInt32 GetMaxLevel();

public:

	void SaveToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("level", m_Level)
		CL_DBPROPERTY("exp", m_Exp)
		CL_DBPROPERTY("passtype", m_PassType)
		CL_DBPROPERTY("normalreward", m_NormalReward)
		CL_DBPROPERTY("highreward", m_HighReward)
		CL_DBPROPERTY("activity", m_Activity)
		CL_DBPROPERTY("unlock", m_Unlock)
		CL_DBPROPERTY("seasonid", m_SeasonID)
		CL_DBPROPERTY("extraexp", m_ExtraExp)
	CL_DBPROPERTY_END()

private:
	//账号ID
	ObjUInt32 m_Accid;
	//通行证等级
	ObjUInt32 m_Level;
	//通行证经验
	ObjUInt32 m_Exp;
	//通行证类型
	ObjUInt8 m_PassType;
	//普通奖励领取标记
	ObjString m_NormalReward;
	//高级奖励领取标记
	ObjString m_HighReward;
	//账号最高活跃度
	ObjUInt8 m_Activity;
	//功能是否解锁
	ObjUInt8 m_Unlock;
	//重置时间
	ObjUInt32 m_SeasonID;
	//额外经验包领取标志
	ObjUInt8 m_ExtraExp;
	// 保存数据库定时器
	Avalon::SimpleTimer	m_SaveTimer;

};



/**
*@brief 加载角色等级信息
*/
class SelectRoleLevelCallback : public CLRecordCallback
{
public:
	SelectRoleLevelCallback(AdventurePassInfo* pass) {
		m_Pass = pass;
	}

	void OnFinished()
	{
		if (NextRow())
		{
			m_Pass->UnLock();
		}
		else
		{
			InfoLogStream << m_Pass->GetAccid() << "do not have ok role" << LogStream::eos;
		}

	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << m_Pass->GetAccid() << "SelectRoleLevelCallback faild" << errorcode << LogStream::eos;
	}

private:
	AdventurePassInfo * m_Pass;
};

#endif
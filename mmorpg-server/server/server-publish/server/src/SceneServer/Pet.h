#ifndef _PET_H_
#define _PET_H_

#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLErrorCode.h>
#include <CLPetDataEntry.h>

class Player;

class Pet : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Pet, Avalon::NullMutex)
public:
	Pet(Player* player, UInt32 _dataId, ObjID_t _id, UInt16 _level, UInt32 _exp, UInt16 _hunger, UInt8 _skillIndex, UInt8 _goldFeedCount, UInt8 _pointFeedCount, UInt8 _selectSkillCount);
	Pet(Player* player, UInt32 _dataId, UInt16 _hungry);
	~Pet();

public:
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 初始化宠物
	*/
	UInt32 Init();

	/**
	*@brief 获取数据项
	*/
	PetDataEntry* GetDataEntry() { return m_pDataEntry; }

	/**
	*@brief 获取表ID
	*/
	UInt32 GetDataId() const { return m_DataId; };

	/**
	*@brief 获取宠物品阶
	*/
	PetQuality GetQuality() const { return m_pDataEntry->quality; }

	/**
	*@brief 获取宠物类型
	*/
	PetType GetType() const { return m_pDataEntry->type; }

	/**
	*@brief 获取出售奖励
	*/
	std::vector<ItemReward> GetSellReward() const { return m_pLevelDataEntry->sellRewards; }

	/**
	*@brief 获取宠物蛋ID
	*/
	UInt32 GetItemId() const { return m_pDataEntry->itemId; }

	/**
	*@brief 获取宠物等级
	*/
	UInt16 GetLevel() const { return m_Level; }

	/**
	*@brief 获取宠物经验
	*/
	UInt32 GetExp() const { return m_Exp; }

	/**
	*@brief 获取宠物饥饿值
	*/
	UInt16 GetHunger() const { return m_Hunger; }

	/**
	*@brief 获取技能索引
	*/
	UInt8 GetSkillIndex() const { return m_SkillIndex; }

	/**
	*@brief 获取金币喂养次数
	*/
	UInt8 GetGoldFeedCount() const { return m_GoldFeedCount; }

	/**
	*@brief 技能重选次数
	*/
	UInt8 GetSelectSkillCount() const { return m_SelectSkillCount; }

	/**
	*@brief 增加金币喂养次数
	*/
	void IncGoldFeedCount() { m_GoldFeedCount = m_GoldFeedCount + 1; }

	/**
	*@brief 获取点卷喂养次数
	*/
	UInt8 GetPointFeedCount() const { return m_PointFeedCount; }

	/**
	*@brief 增加点卷喂养次数
	*/
	void IncPointFeedCount() { m_PointFeedCount = m_PointFeedCount + 1; }

	/**
	*@brief 获取宠物信息
	*/
	void GetPetInfo(PetInfo& info);

	/**
	*@brief 是否满级
	*/
	bool IsFullLevel(){ return m_pLevelDataEntry->upLevelExp == 0; }

	/**
	*@brief 被吞噬产出的经验
	*/
	UInt32 GetDevourExp();

public:
	/**
	*@brief 消耗疲劳事件
	*/
	void OnRemoveFatigue(UInt16 fatigue);

public:
	/**
	*@brief 喂食宠物
	*/
	UInt32 Feed();

	/**
	*@brief 设置宠物技能索引
	*/
	UInt32 SetSkillIndex(UInt8 index);

	/**
	*@brief 增加经验
	*/
	void IncExp(UInt32 exp);

	/**
	*@brief 消耗饥饿度
	*/
	void ConsumeHunger(UInt16 value);

	/**
	*@brief 增加饥饿度
	*/
	void AddHunger(UInt16 value);

	/**
	*@brief 是否满饱食度
	*/
	bool IsFullHunger();

	/**
	*@brief 重置喂食次数
	*/
	void ResetFeedCount();

private:
	/**
	*@brief 升级
	*/
	void UpLevel();

public:// 评分

	/**
	*@brief 设置获取评分
	*/
	void SetPetScore(UInt32 score) { m_PetScore = score; }
	UInt32 GetPetScore() { return m_PetScore; }

	/**
	*@brief 计算评分
	*/
	void CalcPetScore();

public:

	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
	CL_SYNC_PROPERTY(m_Level, POA_LEVEL, SST_SELF)
	CL_SYNC_PROPERTY(m_Exp, POA_EXP, SST_SELF)
	CL_SYNC_PROPERTY(m_Hunger, POA_HUNGER, SST_SELF)
	CL_SYNC_PROPERTY(m_SkillIndex, POA_SKILL_INDEX, SST_SELF)
	CL_SYNC_PROPERTY(m_GoldFeedCount, POA_GOLD_FEED_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_PointFeedCount, POA_POINT_FEED_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_SelectSkillCount, POA_SELECT_SKILL_COUNT, SST_SELF)
	CL_SYNC_PROPERTY(m_PetScore, POA_PET_SCORE, SST_SELF)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("dataid", m_DataId)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("exp", m_Exp)
	CL_DBPROPERTY("hunger", m_Hunger)
	CL_DBPROPERTY("skill_index", m_SkillIndex)
	CL_DBPROPERTY("gold_feed_count", m_GoldFeedCount)
	CL_DBPROPERTY("point_feed_count", m_PointFeedCount)
	CL_DBPROPERTY("select_skill_count", m_SelectSkillCount)
	CL_DBPROPERTY_END()

	void InsertPet();

	void DeletePet();

	void WatchData(Avalon::NetOutputStream& stream);

	void SyncToDB(bool bFlush);

	void SyncProperty(bool bDirty = true);

private:
	//拥有者
	Player*	m_pOwner;

	//数据ID
	ObjUInt32 m_DataId;

	//宠物等级
	ObjUInt16 m_Level;

	//宠物经验
	ObjUInt32 m_Exp;

	//宠物技能索引
	ObjUInt8 m_SkillIndex;

	//宠物饥饿度
	ObjUInt16 m_Hunger;

	//宠物金币喂食次数
	ObjUInt8 m_GoldFeedCount;

	//宠物点卷喂食次数
	ObjUInt8 m_PointFeedCount;

	//技能重选次数
	ObjUInt8 m_SelectSkillCount;

	//数据项
	PetDataEntry* m_pDataEntry;

	//升级数据项
	PetLevelDataEntry* m_pLevelDataEntry;

	//评分
	ObjUInt32	m_PetScore;
};

#endif //_PET_H_
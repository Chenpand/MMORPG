#ifndef _PET_MGR_H_
#define _PET_MGR_H_

#include <CLPetDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "Pet.h"

class BattlePetMap : public CLComplexProperty
{
public:
	BattlePetMap();
	~BattlePetMap();

public:
	UInt32 SetBattlePet(PetType petType, UInt64 petId);

	std::vector<UInt64> GetBattlePetList();

	UInt64 GetBattlePetId(PetType petType);

public:
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

public:
	std::map<PetType, UInt64> m_BattlePetMap;

};

class PetMgr
{
public:
	typedef std::map<ObjID_t, Pet*> PetMap;

public:
	PetMgr();
	~PetMgr();

public:
	void SetOwner(Player* player) { m_pOwner = player; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 根据ID查找一个宠物
	*/
	Pet* FindPet(ObjID_t id);

	/**
	*@brief 获取宠物个数
	*/
	UInt32 GetPetSize() { return m_PetMap.size(); }

	/**
	*@brief 获取出战宠物
	*/
	BattlePetMap& GetBattlePetMap() { return m_BattlePetMap; };

	/**
	*@brief 获取跟随的宠物ID
	*/
	ObjUInt64& GetFollowPetId() { return m_FollowPetId; }

	/**
	*@brief 获取跟随的宠物
	*/
	//ObjScenePet& GetFollowPet(){ return m_ScenePet; }
	ObjUInt32& GetFollowPetDataId() { return m_FollowPetDataId; }

	/**
	*@brief 获取战斗宠物信息
	*/
	void GetRacePets(std::vector<RacePet>& pets);

	/**
	*@brief 获取宠物基本信息
	*/
	void GetPetBaseInfos(std::vector<PetBaseInfo>& pets);

public:
	/**
	*@brief 增加宠物
	*/
	UInt32 AddPet(UInt32 itemId);

	/**
	*@brief 设置宠物槽位
	*/
	UInt32 SetPetSlot(PetType petType, UInt64 petId);

	/**
	*@brief 获取宠物数量
	*/
	UInt32 GetPetSize(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief 获取宠物槽位
	*/
	std::vector<UInt64> GetBattlePets() { return m_BattlePetMap.GetBattlePetList(); }

	/**
	*@brief 喂食宠物
	*/
	UInt32 FeedPet(ObjID_t id, PetFeedType petFeedType, UInt8& isCritical, UInt32& value);

	/**
	*@brief 吞噬宠物
	*/
	UInt32 DevourPet(ObjID_t id, std::vector<ObjID_t> petIds, UInt32& exp);

	/**
	*@brief 出售宠物
	*/
	UInt32 SellPet(ObjID_t id);

	/**
	*@brief 变更宠物技能
	*/
	UInt32 ChangePetSkill(ObjID_t id, UInt8 skillIndex);

	/**
	*@brief 设置跟随的宠物
	*/
	UInt32 SetPetFollow(ObjID_t id);

public:	//事件
	/**
	*@brief 上线事件
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief 离线事件
	*/
	void OnOffline();

	/**
	*@brief 进入逻辑第二天(6点)
	*/
	void OnLogicDayChanged();

	/**
	*@brief 查询回调
	*/
	void OnSelectDataRet(CLRecordCallback* callback);

	/**
	*@brief 疲劳消耗事件
	*/
	void OnRemoveFatigue(UInt16 fatigue);

	/**
	*@brief 切换场景
	*/
	void OnEnterScene(UInt32 type);

	/**
	*@brief 进入地下城场景
	*/
	void OnEnterDungeonRace();

	/**
	*@brief Tick
	*/
	void OnTick(Avalon::Time now);

	/**
	*@brief 出战宠物休息
	*/
	void OnBattlePetRest();

	/**
	*@brief 出战宠物战斗
	*/
	void OnBattlePetBattle();

	/**
	*@brief 宠物休息
	*/
	void OnPetRest();

public:
	/**
	*@brief 扣除宠物饥饿值
	*/
	void RemovePetHunger(UInt16 hunger);

	/**
	*@brief 增加宠物饥饿值
	*/
	void AddPetHunger(UInt16 hunger);

	/**
	*@brief 清理宠物
	*/
	void Clear();

private:

	/**
	*@brief 加载宠物
	*/
	UInt32 LoadPet(Pet* pet);

	/**
	*@brief 删除宠物
	*/
	UInt32 DeletePet(ObjID_t id);

	/**
	*@brief 获取随从列表
	*/
	std::vector<PetInfo> GetPetList();

	/**
	*@brief 下发随从列表
	*/
	void SyncPetList();

	/**
	*@brief 下发删除宠物
	*/
	void SyncDeletePet(Pet* pet);

	/**
	*@brief 同步城镇随从
	*/
	void SyncScenePet(Pet* pet);

	/**
	*@brief 是否宠物休息场景
	*/
	bool IsPetRestScene(UInt32 type);

	/**
	*@brief 重置喂食次数
	*/
	void ResetFeedCount();

	/**
	*@brief 初始化出战宠物
	*/
	void InitBattlePet();

	/**
	*@brief 判断宠物是否出战
	*/
	bool IsBattlePet(UInt64 petId);

	/**
	*@brief 宠物登陆日志
	*/
	void SendPetLoginUdpLog();

private:
	//拥有者
	Player*	m_pOwner;

	//宠物MAP
	PetMap m_PetMap;

	//出战宠物
	BattlePetMap m_BattlePetMap;

	//跟随的随从
	ObjUInt64 m_FollowPetId;

	//城镇的宠物
	//ObjScenePet m_ScenePet;
	ObjUInt32 m_FollowPetDataId;

	//宠物休息时间
	UInt32 m_BattlePetRestSec;

	//宠物战斗时间
	UInt32 m_BattlePetBattleSec;

	//非出战宠物休息开始时间
	UInt32 m_PetStartTime;

	//宠物休息开始时间
	UInt32 m_BattlePetRestStartTime;

	//宠物战斗开始时间
	UInt32 m_BattlePetBattleStartTime;

	//宠物休息间隔时间
	UInt32 m_BattlePetRestIntervalSec;

	//宠物战斗间隔时间
	UInt32 m_BattlePetBattleIntervalSec;

	//休息间隔时间
	UInt32 m_PetIntervalSec;

};

#endif //_PET_MGR_H_
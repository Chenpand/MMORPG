#ifndef _WARP_STONE_H_
#define _WARP_STONE_H_

#include <CLDefine.h>
#include <CLWarpStoneDataEntry.h>

class Player;

struct EnergyStoneInfo
{

public:
	EnergyStoneInfo(UInt32 _id, UInt32 _num) :id(_id), num(_num)
	{
		dataEntry = EnergyStoneDataEntryMgr::Instance()->FindEntry(id);
	}

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & num;
	}

public:
	//能量石ID
	UInt32 id;
	//能量石数量
	UInt32 num;
	//能量石数据
	EnergyStoneDataEntry* dataEntry;

};

class WarpStone 
{

public:
	WarpStone();
	WarpStone(Player* owner, UInt32 id, UInt32 level = 0, UInt32 energy = 0);
	~WarpStone();

public:
	void SetOwner(Player* owner);
	Player* GetOwner() const { return m_pOwner; }

	bool Init();

public:

	/**
	*@brief 解锁
	*/
	UInt32 Unlock();

	/**
	*@brief 充能
	*/
	UInt32 Charge(std::vector<EnergyStoneInfo> energyStone);


public:
	UInt32 GetId() { return m_Id; }

	UInt32 GetLevel() { return m_Level; }

	UInt32 GetEnergy() { return m_Energy; }

	WarpStoneType GetType();

	void SetDirty() { m_IsDirty = true; }

	void ClearDirty() { m_IsDirty = false; }

	bool IsDirty() { return m_IsDirty; }

	void SetLevel(UInt32 level);

public://边界码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id;
		if (m_Id != 0)
		{
			UInt8 level = (UInt8)m_Level;
			stream & level & m_Energy;
		}
	}

private:

	/**
	*@brief 增加能量
	*/
	void AddEnergy(UInt32 energy);

	/**
	*@brief 检查充能
	*/
	UInt32 CheckCharge(std::vector<EnergyStoneInfo> energyStone);

	/**
	*@brief 升级
	*/
	void UpLevel();

private:
	// id
	UInt32	m_Id;

	//位置
	//UInt32	m_Index;

	//次元石类型
	//WarpStoneType	m_Type;

	//能量
	UInt32	m_Energy;

	//等级
	UInt32	m_Level;

	//是否解锁
	//bool	m_IsUnlock;

	//是否脏数据
	bool m_IsDirty;

	//拥有人
	Player*	m_pOwner;


	WarpStoneLevelDataEntry* m_pLevelDataEntry;
	WarpStoneDataEntry* m_pDataEntry;

};



#endif //_WARP_STONE_H_

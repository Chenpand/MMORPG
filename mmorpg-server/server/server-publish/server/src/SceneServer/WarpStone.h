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
	//����ʯID
	UInt32 id;
	//����ʯ����
	UInt32 num;
	//����ʯ����
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
	*@brief ����
	*/
	UInt32 Unlock();

	/**
	*@brief ����
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

public://�߽���
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
	*@brief ��������
	*/
	void AddEnergy(UInt32 energy);

	/**
	*@brief ������
	*/
	UInt32 CheckCharge(std::vector<EnergyStoneInfo> energyStone);

	/**
	*@brief ����
	*/
	void UpLevel();

private:
	// id
	UInt32	m_Id;

	//λ��
	//UInt32	m_Index;

	//��Ԫʯ����
	//WarpStoneType	m_Type;

	//����
	UInt32	m_Energy;

	//�ȼ�
	UInt32	m_Level;

	//�Ƿ����
	//bool	m_IsUnlock;

	//�Ƿ�������
	bool m_IsDirty;

	//ӵ����
	Player*	m_pOwner;


	WarpStoneLevelDataEntry* m_pLevelDataEntry;
	WarpStoneDataEntry* m_pDataEntry;

};



#endif //_WARP_STONE_H_

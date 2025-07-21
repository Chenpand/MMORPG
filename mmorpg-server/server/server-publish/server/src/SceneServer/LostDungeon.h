#ifndef __LOST_DUNGEON_H__
#define __LOST_DUNGEON_H__

#include <CLDefine.h>
#include <CLLostDungeonDefine.h>
#include <CLItemDefine.h>
#include <CLRecordClient.h>
#include <CLOwnerObjectProperty.h>

struct ActivityInfo;
class Player;

class LostDungeon 
{
public:
	LostDungeon();
	~LostDungeon();

public:
	void SetOwner(Player* player) { m_owner = player; }
	Player* GetOwner() { return m_owner; }

	void OnOnline();

	void OnActOpen(ActivityInfo* info);
	void OnActClose();

	void	LoadDataFromDb(CLRecordCallback* pCallback);
	UInt32 OnOpen();
	bool GenerateFloors();
	LostDungeonFloorData* GetFloorData(UInt32 floor);
	UInt32 OnPlayerChallengeReq(UInt32 floor, UInt8 hardType);
	void GetFloorDatas(std::vector<LostDungeonFloorData>& floorDatas);
	UInt32 OnPlayerOpenBox(UInt32 floor, UInt32 boxId, std::vector<ItemReward>& itemVec);
	UInt32	OnPlayerChoiceBoxItem(UInt32 itemId, ItemReward& item);
	void SaveData();
	void SaveDirtyData();
	void InsertData();
	std::string GetFloorDataStr();
	void LoadFloorDataFromDB(std::string str);
	void UpdateOneFloor(LostDungeonFloorData* floor);
	//����һ��ս��
	void SettleFloor(UInt32 floor, UInt8 result, UInt32 score, UInt8 btState = 0);
	UInt32 GetReward();
	std::map<UInt32, LostDungeonFloorData>& GetFloors() {
		return m_genFloorDatas;
	}
	UInt32 OnPlayerGiveUp();

	OwnerObjUInt32& GetLostDungeonScore() { return m_lostDungeonScore; }
	inline void  SetLostDungeonScore(UInt32 score) { m_lostDungeonScore = score; }

	OwnerObjInt32& GetLostDungeonHp() { return m_LostDungeonHp; }
	void SetLostDungeonHp(Int32 hp);

	OwnerObjInt32& GetLostDungeonMp() { return m_LostDungeonMp; }
	void SetLostDungeonMp(Int32 mp);

	bool OnPlayerDeadInBattle(UInt32 floor);

	inline UInt32 GetCreateTime() { return m_createTime; }

	void SetCreateTime(UInt32 time);

	void Reset();

	UInt8  GetChallengeFloor();
private:
	Player*			m_owner;
	// 
	ObjUInt64		m_guid;
	// ����ʱ��
	ObjUInt32		m_createTime;
	//���ɵ�¥������
	std::map<UInt32, LostDungeonFloorData>	m_genFloorDatas;
	bool m_floorDataDirty;
	// ���λ���
	OwnerObjUInt32					m_lostDungeonScore;
	// ����ʣ��Ѫ��(ǧ�ֱ�)
	OwnerObjInt32					m_LostDungeonHp;
	// ����ʣ������(ǧ�ֱ�)
	OwnerObjInt32					m_LostDungeonMp;
};

#endif
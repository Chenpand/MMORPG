#ifndef __CL_MONSTER_DATA_ENTRY_H__
#define __CL_MONSTER_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum MonsterType
{
    MT_HERO,
    MT_MONSTER,
    MT_ELITE,
    MT_BOSS,
    MT_NPC,
    MT_HELL,
    MT_ACTIVITY,
	// �ʵ�
	MT_EASTER_EGG = 9,

	// ��������ڱ���У��������Լ���
	// ���ƻ���
	MT_BOX = 9999,
	
};

/**
*@brief ����������
*/
class MonsterDataEntry : public Avalon::DataEntry
{
public:
    MonsterDataEntry();
    ~MonsterDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

    static UInt32 GetMonsterTypID(UInt32 id) { return id / 10000; }

public:
    // id
    UInt32	                id;
	// ����ģʽ
	UInt8					mode;
	// type
    MonsterType				type;
    // ����
    UInt32                  exp;
    // ����ID
    std::vector<UInt32>     dropSetIds;
	// ÿ�����������
	UInt32					dailyMaxDropNum;
};

typedef Avalon::DataEntryMgr<MonsterDataEntry> MonsterDataEntryMgr;

#endif
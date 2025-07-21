#ifndef __DUNGEON_HARD_INFO_H__
#define __DUNGEON_HARD_INFO_H__

#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLDungeonDataEntry.h>
#include <CLDungeonDefine.h>

class DungeonHardInfo : public CLSyncObject
{
    AVALON_DEFINE_OBJECTPOOL(DungeonHardInfo, Avalon::NullMutex)
public:
    DungeonHardInfo();
    ~DungeonHardInfo();

public:
    /**
    *@brief ���³�ID
    */
    inline void SetDungeonID(UInt32 id) { m_dungeonId = id; }
    inline UInt32 GetDungeonID() const { return m_dungeonId; }

    /**
    *@brief �ѽ���������Ѷ�����
    */
    inline void SetUnlockedHardType(DungeonHardType type) { m_unlockedHardType = (UInt8)type; }
    inline DungeonHardType GetUnlockedHardType() const { return (DungeonHardType)(UInt8)m_unlockedHardType; }

    SceneDungeonHardInfo GetSceneDungeonHardInfo();

public:
    //��������
    CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
    CL_SYNC_PROPERTY(m_dungeonId, DHA_ID, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY(m_unlockedHardType, DHA_UNLOCKED_HARD_TYPE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY_END()

    CL_DBPROPERTY_BEGIN(CLSyncObject)
    CL_DBPROPERTY("dungeon_id", m_dungeonId)
    CL_DBPROPERTY("unlocked_hard_type", m_unlockedHardType)
    CL_DBPROPERTY_END()

public:
    static DungeonHardInfo* CreateDungeonHardInfo(UInt32 dungeonId);

private:
    // ���³�id
    ObjUInt32	            m_dungeonId;
   
    // �Ѷ�����(DungeonHardType)
    ObjUInt8                m_unlockedHardType;
};

#endif
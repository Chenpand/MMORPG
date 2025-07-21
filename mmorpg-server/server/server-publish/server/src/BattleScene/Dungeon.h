#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLDungeonDataEntry.h>
#include <CLDungeonDefine.h>

class Dungeon : public CLSyncObject
{
    AVALON_DEFINE_OBJECTPOOL(Dungeon, Avalon::NullMutex)
public:
    Dungeon();
    ~Dungeon();

public:
    /**
    *@brief 设置数据项
    */
    inline void SetDataEntry(DungeonDataEntry* dataEntry) { m_pDataEntry = dataEntry; }
    inline DungeonDataEntry* GetDataEntry() const { return m_pDataEntry; }

    /**
    *@brief 获取数据表id
    */
    inline void SetDataID(UInt32 id) { m_DataId = id; }
    inline UInt32 GetDataID() const { return m_DataId; }

    /**
    *@brief 地下城次数
    */
    inline void SetNum(UInt32 num) { m_Num = num; }
    inline UInt32 GetNum() const { return m_Num; }
    inline void IncNum() { SetNum(GetNum() + 1); }

    /**
    *@brief 最佳评分
    */
    inline void SetBestScore(LevelScore score) { m_BestScore = (UInt8)score; }
    inline LevelScore GetBestScore() const { return (LevelScore)((UInt8)m_BestScore); }

    /**
    *@brief 最快通关时间
    */
    inline void SetBestRecordTime(UInt32 sec) { m_BestRecordTime = sec; }
    inline UInt32 GetBestRecordTime() const { return m_BestRecordTime; }

    SceneDungeonInfo GetSceneDungeonInfo();

public:
    //编解码相关
    CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
    CL_SYNC_PROPERTY(m_DataId, DA_ID, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY(m_Num, DA_NUM, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY(m_BestScore, DA_BEST_SCORE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY(m_BestRecordTime, DA_BEST_RECORD_TIME, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
    CL_SYNC_PROPERTY_END()

    CL_DBPROPERTY_BEGIN(CLSyncObject)
    CL_DBPROPERTY("dungeon_id", m_DataId)
    CL_DBPROPERTY("num", m_Num)
    CL_DBPROPERTY("best_score", m_BestScore)
    CL_DBPROPERTY("best_record_time", m_BestRecordTime)
    CL_DBPROPERTY_END()

public:
    static Dungeon* CreateDungeon(UInt32 dungeonId);

private:
    // 地下城id
    ObjUInt32	            m_DataId;
    // 表格数据
    DungeonDataEntry*       m_pDataEntry;
    // 次数
    ObjUInt32               m_Num;
    // 最佳评分
    ObjUInt8                m_BestScore;
    // 最快通关时间(ms)
    ObjUInt32               m_BestRecordTime;
};
#endif

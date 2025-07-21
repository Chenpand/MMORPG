#ifndef __PK_INFO_H__
#define __PK_INFO_H__

#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLMatchDefine.h>
#include <CLJobDataEntry.h>
#include "PKOccuRecord.h"


#define MAX_RECENT_RECORD_NUM 10

class PkRecentRecord : public CLComplexProperty
{
public:
    PkRecentRecord()
    {
        memset(m_record, 0, sizeof(m_record));
    }

    virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
    virtual void Decode(Avalon::NetInputStream& stream);

    void PushRecentRecord(PkRaceResult result);

    virtual void DecodeString(std::istringstream& is);

    virtual void EncodeString(std::ostringstream& os);

public:
    UInt8       m_record[MAX_RECENT_RECORD_NUM];
};

class PkStatistic : public CLSyncObject
{
    AVALON_DEFINE_OBJECTPOOL(PkStatistic, Avalon::NullMutex)
public:
    PkStatistic();
    ~PkStatistic();

public:
    PkType GetType() { return (PkType)(UInt8)m_type; }
    void SetType(PkType type) { m_type = (UInt8)type; }

	UInt32 GetTotalWinNum() const { return m_totalWinNum; }
	UInt32 GetTotalLoseNum() const { return m_totalLoseNum; }
	UInt32 GetTotalNum() const { return m_totalNum; }
	UInt32 GetMaxWinStreak() const { return m_maxWinStreak; }
	UInt32 GetCurWinStreak() const { return m_curWinStreak; }
	UInt32 GetCurLoseStreak() const { return m_curLoseStreak; }

	void Clear();

    void OnUpdatePkStatistic(Occupation opponentOccu, PkRaceResult result);
    
public:
    //编解码相关
    CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
    CL_SYNC_PROPERTY(m_type, PKIA_TYPE, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY(m_totalWinNum, PKIA_TOTAL_WIN_NUM, SST_SELF |  SST_WORLD)
    CL_SYNC_PROPERTY(m_totalLoseNum, PKIA_TOTAL_LOSE_NUM, SST_SELF |  SST_WORLD)
    CL_SYNC_PROPERTY(m_totalNum, PKIA_TOTAL_NUM, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY(m_occuRecordMgr, PKIA_DETAIL_RECORD_INFO, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY(m_recentRecord, PKIA_RECENT_RECORD, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY(m_maxWinStreak, PKIA_MAX_WIN_STEAK, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY(m_curWinStreak, PKIA_CUR_WIN_STEAK, SST_SELF | SST_WORLD)
    CL_SYNC_PROPERTY_END()

    CL_DBPROPERTY_BEGIN(CLSyncObject)
    CL_DBPROPERTY("type", m_type)
    CL_DBPROPERTY("total_win_num", m_totalWinNum)
    CL_DBPROPERTY("total_lose_num", m_totalLoseNum)
    CL_DBPROPERTY("total_num", m_totalNum)
    CL_DBPROPERTY("detail_record_info", m_occuRecordMgr)
    CL_DBPROPERTY("recent_record", m_recentRecord)
    CL_DBPROPERTY("max_win_streak", m_maxWinStreak)
    CL_DBPROPERTY("cur_win_streak", m_curWinStreak)
	CL_DBPROPERTY("cur_lose_streak", m_curLoseStreak)
    CL_DBPROPERTY_END()

public:
    static PkStatistic* CreatePkStatistic(PkType type);

private:
    // 类型
    ObjUInt8	            m_type;
    
    // 总胜场
    ObjUInt32               m_totalWinNum;

    // 总负场
    ObjUInt32               m_totalLoseNum;

    // 总场
    ObjUInt32               m_totalNum;

    // 对战不同的职业的战绩
    PkOccuRecordMgr         m_occuRecordMgr;

    // 近期战绩
    PkRecentRecord          m_recentRecord;

    // 最多连胜场数
    ObjUInt32               m_maxWinStreak;

    // 当前连胜场数
    ObjUInt32               m_curWinStreak;

	// 当前连败场数
	ObjUInt32				m_curLoseStreak;
};
#endif

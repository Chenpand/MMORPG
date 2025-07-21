#ifndef _CL_SCENEOBJECT_H_
#define _CL_SCENEOBJECT_H_

#include "CLSyncObject.h"
#include "CLObjectDefine.h"
#include "CLObjectProperty.h"
#include "CLOwnerObjectProperty.h"

/**
 *@brief 场景同步类型
 */
enum SceneSyncType
{
	SST_MOVE		= 1 << 0,	//移动同步
	SST_SCENE		= 1 << 1,	//同步给其他场景
	SST_SELF		= 1 << 2,	//同步给自身
	SST_AROUND		= 1 << 3,	//同步给周围
	SST_SELECTED	= 1 << 4,	//同步给选中的
	SST_TEAM		= 1 << 5,	//同步给队伍
	SST_WORLD		= 1	<< 6,	//同步到worldServer
	SST_WATCH		= 1	<< 7,	//观察
	SST_CHALLENGE	= 1 << 8,	//跨服挑战
	SST_SKILLBUFF   = 1 << 9,	//技能和buff

    SST_TEST        = 1 << 10,  //测试用的，代替原来的SELF
	SST_BATTLE		= 1 << 11,	//战场
	SST_LOSTDUNGEON = 1 << 12,  //地牢
	SST_LOSTDUNG_ENTER_SYNC = 1 << 13,  //进入地牢需要同步数据
	SST_SCENE_ENTER  = 1 << 14,	// 进入场景时同步
	//所有
	SST_ALL			= DB_SYNC_TYPE | SST_MOVE | SST_SCENE | SST_SELF | SST_AROUND | SST_SELECTED | SST_TEAM | SST_WORLD | SST_WATCH | SST_CHALLENGE | SST_SCENE_ENTER
};

#define INVALID_GRID_INDEX UInt32(-1)


/**
 *@brief 使用这个宏序列化除了同步属性外的其他属性
 */
#define DEFINE_CHANGESCENE_SERIALIZATION(SuperClass) \
	void Input(Avalon::NetInputStream& stream) { \
		SuperClass::Input(stream); \
		Serialize(stream); \
	} \
	void Output(Avalon::NetOutputStream& stream) { \
		SuperClass::Output(stream); \
		Serialize(stream); \
	} \
	template<typename StreamT> \
	void Serialize(StreamT& stream)

class CLSceneObject : public CLSyncObject
{
	friend class CLGrid;
public:
	CLSceneObject()
		:m_GridIndex(INVALID_GRID_INDEX),m_OldGridIndex(INVALID_GRID_INDEX)
	{
		m_SceneID = 0;
		m_Next = m_Prev = NULL;
	}
	virtual ~CLSceneObject()
	{
	}

	virtual SceneObjectType GetType() const = 0;

	void SetSceneID(UInt32 id){ m_SceneID = id; }
	UInt32 GetSceneID() const { return m_SceneID; }

	bool IsInScene() const { return m_GridIndex != INVALID_GRID_INDEX; }

	void SetGridIndex(UInt32 index)
	{
		if(m_GridIndex != index)
		{
			m_OldGridIndex = m_GridIndex;
			m_GridIndex = index;
		}
	}
	UInt32 GetGridIndex() const { return m_GridIndex; }
	UInt32 GetOldGridIndex() const { return m_OldGridIndex; }

	virtual void SetPos(const CLPosition& pos) { m_Pos = pos; }
	const CLPosition& GetPos() const { return m_Pos; }

	void SetDir(UInt8 dir) { m_Dir = dir; }
	UInt8 GetDir() const { return m_Dir; }

	UInt16 Distance(CLSceneObject* obj){ return GetPos().Distance(obj->GetPos()); }

	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
	//CL_SYNC_PROPERTY(m_Pos,	SOA_POSITION,	SST_MOVE)
	//CL_SYNC_PROPERTY(m_Dir,	SOA_DIR,		SST_MOVE)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY_END()
	
	virtual void Input(Avalon::NetInputStream& stream){}
	virtual void Output(Avalon::NetOutputStream& stream){}

private:
	//场景id
	UInt32		m_SceneID;
	//场景区索引
	UInt32		m_GridIndex;
	//场景区旧索引
	UInt32		m_OldGridIndex;
	//位置
	ObjPosition	m_Pos;
	//方向
	ObjUInt8	m_Dir;

	//next
	CLSceneObject* m_Next;
	//prev
	CLSceneObject* m_Prev;
};

typedef CLVisitor<CLSceneObject> CLSceneObjectVisitor;

#endif

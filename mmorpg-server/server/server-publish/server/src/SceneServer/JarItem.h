#ifndef _JAR_ITEM_H_
#define _JAR_ITEM_H_
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "DropItemMgr.h"
#include "CLItemDataEntry.h"

class Player;
enum SyncDBState
{
	SDS_NONE = 0,
	SDS_INSERT,				//插入
	SDS_UPDATE,				//更新
	SDS_DELETE,
};

class JarItem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(JarItem, Avalon::NullMutex)

public:
	JarItem();
	~JarItem();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("itemid", m_itemId)
		CL_DBPROPERTY("owner", m_owner)
		CL_DBPROPERTY("occus", m_occus)
		CL_DBPROPERTY("jartype", m_jarType)
		CL_DBPROPERTY("missnum", m_missNum)
		CL_DBPROPERTY("getnum", m_getNum)
		CL_DBPROPERTY("minusfixnum", m_minusFixNum)
	CL_DBPROPERTY_END()

	inline void SetOwner(UInt64 owner)	{ m_owner = owner; }
	inline UInt32 GetItemId()		{ return m_itemId; }
	inline std::string GetOccus()	{ return m_occus; }
	inline UInt32 GetType()			{ return m_jarType; }
	inline JarItemPoolData* GetData()	{ return m_data; }
	inline FakeWeightBase* GetFakeWeight()	{ return &m_fakeWeight; }
	inline void IncGetNum()			{ m_getNum = m_getNum + 1; }
	bool HasRestNum();

	void SetData(JarItemPoolData* data);
	inline void SetDBState(SyncDBState state)	{ m_dbState = state; }
	bool SyncDB(Player* player);
private:

	void UpdateDB(Player* player);
	void InsertDB(Player* player);

	ObjUInt64	m_owner;
	//道具id
	ObjUInt32	m_itemId;
	//职业
	ObjString	m_occus;
	//开罐类型
	ObjUInt32	m_jarType;
	//获得数量
	ObjUInt32	m_getNum;
	//没随中次数
	ObjUInt32	m_missNum;
	//减益修正次数
	ObjUInt32	m_minusFixNum;
	//表格数据
	JarItemPoolData* m_data;
	//db状态
	SyncDBState	m_dbState;
	//伪随对象
	FakeWeightBase	m_fakeWeight;
};

#endif
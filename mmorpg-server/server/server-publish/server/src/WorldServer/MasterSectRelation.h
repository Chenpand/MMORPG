#ifndef _MASTER_SECT_RELATION_H
#define _MASTER_SECT_RELATION_H

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLMasterSystemDefine.h>
class WSPlayer;

/*
 *@brief 同门关系
 */
class MasterSectRelation : public CLSyncObject
{
public:
	MasterSectRelation(UInt8 type);
	MasterSectRelation(UInt8 type, WSPlayer* player);
	~MasterSectRelation(){};

public:
	/**
	*@brief 设置获取等级
	*/
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	*@brief 获取类型
	*/
	inline UInt8 GetType() const{ return m_Type; }
	inline void SetType(UInt8 type)	{ m_Type = type; }

	/**
	*@brief 设置获取唯一id
	*/
	void SetGuid(ObjID_t guid){ m_Guid = guid; }
	ObjID_t GetGuid() const{ return m_Guid; }

	/**
	*@brief 玩家状态，对应枚举FriendMatchStatus
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	*@brief 设置获取是否超级蓝钻
	*/
	void SetVipLv(UInt8 bSuperBlue){ m_vipLv = bSuperBlue; }
	UInt8 GetVipLv() const{ return m_vipLv; }

	/**
	*@brief 设置获取职业
	*/
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	*@brief 是否在线
	*/
	bool IsOnline() const;

	/**
	*@brief 设置出师标志
	*/
	void SetFinSchMark(bool isFinSch);

public://编解码
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(m_Name, MSRA_NAME, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Level, MSRA_LEVEL, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Occu, MSRA_OCCU, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Type, MSRA_TYPE, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_vipLv, MSRA_VIPLV, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Status, MSRA_STATUS, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
		CL_SYNC_PROPERTY(m_IsFinshSchool, MSRA_ISFINSCH, MSRST_MASTER | MSRST_BROTHER | MSRST_DISCIPLE)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("type", m_Type)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("occu", m_Occu)
	CL_DBPROPERTY("viplv", m_vipLv)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY("addtime", m_AddTime)
	CL_DBPROPERTY("isFinSch", m_IsFinshSchool)
	CL_DBPROPERTY_END()

	/**
	*@brief 数据库操作
	*/
	void LoadDBData(CLRecordCallback* callback);
	void InsertToDB(ObjID_t owner);
	void SyncToDB();
	void DeleteFromDB();

private:
	//类型
	ObjUInt8	m_Type;
	//唯一id
	ObjID_t		m_Guid;
	//对方账号id
	UInt32		m_AccId;
	//对方等级
	ObjUInt16	m_Level;
	//对方职业
	ObjUInt8	m_Occu;
	//是否超级蓝钻
	ObjUInt8	m_vipLv;
	//对方名字
	ObjString	m_Name;
	//关系建立时间
	ObjUInt32	m_AddTime;
	//玩家状态(对应枚举FriendMatchStatus)
	ObjUInt8	m_Status;
	//关系中是否存在人已经出师
	ObjUInt8    m_IsFinshSchool;
};

typedef CLVisitor<MasterSectRelation> MasterSectRelationVisitor;

#endif
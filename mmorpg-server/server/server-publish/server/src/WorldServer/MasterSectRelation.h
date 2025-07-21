#ifndef _MASTER_SECT_RELATION_H
#define _MASTER_SECT_RELATION_H

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLMasterSystemDefine.h>
class WSPlayer;

/*
 *@brief ͬ�Ź�ϵ
 */
class MasterSectRelation : public CLSyncObject
{
public:
	MasterSectRelation(UInt8 type);
	MasterSectRelation(UInt8 type, WSPlayer* player);
	~MasterSectRelation(){};

public:
	/**
	*@brief ���û�ȡ�ȼ�
	*/
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	*@brief ��ȡ����
	*/
	inline UInt8 GetType() const{ return m_Type; }
	inline void SetType(UInt8 type)	{ m_Type = type; }

	/**
	*@brief ���û�ȡΨһid
	*/
	void SetGuid(ObjID_t guid){ m_Guid = guid; }
	ObjID_t GetGuid() const{ return m_Guid; }

	/**
	*@brief ���״̬����Ӧö��FriendMatchStatus
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	*@brief ���û�ȡ�Ƿ񳬼�����
	*/
	void SetVipLv(UInt8 bSuperBlue){ m_vipLv = bSuperBlue; }
	UInt8 GetVipLv() const{ return m_vipLv; }

	/**
	*@brief ���û�ȡְҵ
	*/
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	*@brief �Ƿ�����
	*/
	bool IsOnline() const;

	/**
	*@brief ���ó�ʦ��־
	*/
	void SetFinSchMark(bool isFinSch);

public://�����
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
	*@brief ���ݿ����
	*/
	void LoadDBData(CLRecordCallback* callback);
	void InsertToDB(ObjID_t owner);
	void SyncToDB();
	void DeleteFromDB();

private:
	//����
	ObjUInt8	m_Type;
	//Ψһid
	ObjID_t		m_Guid;
	//�Է��˺�id
	UInt32		m_AccId;
	//�Է��ȼ�
	ObjUInt16	m_Level;
	//�Է�ְҵ
	ObjUInt8	m_Occu;
	//�Ƿ񳬼�����
	ObjUInt8	m_vipLv;
	//�Է�����
	ObjString	m_Name;
	//��ϵ����ʱ��
	ObjUInt32	m_AddTime;
	//���״̬(��Ӧö��FriendMatchStatus)
	ObjUInt8	m_Status;
	//��ϵ���Ƿ�������Ѿ���ʦ
	ObjUInt8    m_IsFinshSchool;
};

typedef CLVisitor<MasterSectRelation> MasterSectRelationVisitor;

#endif
#ifndef _MALL_GIFT_PACK_H_
#define _MALL_GIFT_PACK_H_


#include <CLMallDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

#include "WSPlayer.h"


//�̳���ʱ�������״̬
enum MallGiftPackActivateState
{
	MGPAT_NONE = 0,			//δ����
	MGPAT_OPEN,				//�ѿ���
	MGPAT_CLOSED,			//�ѹر�
};


// �̳���ʱ���
class MallGiftPack : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(MallGiftPack, Avalon::NullMutex);

public:
	MallGiftPack();
	~MallGiftPack();

public:
	static MallGiftPack* Create();
	static void Destroy(MallGiftPack*& item);

public:
	//��ȡ�����˺�id
	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	//��ȡ���ý�ɫid
	UInt64 GetPlayerId() { return m_PlayerId; }
	void SetPlayerId(UInt64 playerId) { m_PlayerId = playerId; }

	//��ȡ������ʱ���ID
	UInt32 GetGiftPackID() { return m_GiftPackID; }
	void SetGiftPackID(UInt32 giftPackId) { m_GiftPackID = giftPackId; }
	
	//��ȡ������ʱ�����ʼʱ��
	UInt32 GetActivateStartTime() { return m_ActivateStartTime; }
	void SetActivateStartTime(UInt32 startTime) { m_ActivateStartTime = startTime; }

	//��ȡ������ʱ�������ʱ��
	UInt32 GetActivateEndTime() { return m_ActivateEndTime; }
	void SetActivateEndTime(UInt32 endTime) { m_ActivateEndTime = endTime; }

	//��ȡ������ʱ���ʣ������
	UInt16 GetRestNum() { return m_RestNum; }
	void SetRestNum(UInt16 num) { m_RestNum = num; }

	//��ȡ������ʱ�����������
	UInt8 GetActivateCond() { return m_ActivateCond; }
	void SetActivateCond(UInt8 activateCond) { m_ActivateCond = activateCond; }

	//��ȡ������ʱ�������״̬
	UInt8 GetActivateState() { return m_ActivateState; }
	void SetActivateState(MallGiftPackActivateState actState) { m_ActivateState = (UInt8)actState; }

	//���û�ȡ��ʱ����������
	UInt16 GetActivateCounter() { return m_ActivateCounter; }
	void SetActivateCounter(UInt16 actCounter);

public:
	/**
	*@brief �������ʱ����Ƿ����
	*/
	bool IsExpired(UInt32 curTime);

public:
	//���ݿ����
	void UpdateToDB(WSPlayer* player, bool bFlush = true);
	void InsertProperty(WSPlayer* player);

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("roleid", m_PlayerId)
		CL_DBPROPERTY("gift_pack_id", m_GiftPackID)
		CL_DBPROPERTY("activate_start_time", m_ActivateStartTime)
		CL_DBPROPERTY("activate_end_time", m_ActivateEndTime)
		CL_DBPROPERTY("rest_num", m_RestNum)
		CL_DBPROPERTY("activate_cond", m_ActivateCond)
		CL_DBPROPERTY("activate_state", m_ActivateState)
		CL_DBPROPERTY("activate_counter", m_ActivateCounter)
	CL_DBPROPERTY_END()

private:
	//�˺�id
	ObjUInt32 m_AccId;

	//��ɫid
	ObjUInt64 m_PlayerId;

	//��ʱ���id
	ObjUInt32 m_GiftPackID;

	//���ʼʱ��
	ObjUInt32 m_ActivateStartTime;

	//�����ֹʱ��
	ObjUInt32 m_ActivateEndTime;

	//ʣ������
	ObjUInt16 m_RestNum;

	//��������
	ObjUInt8 m_ActivateCond;

	//����״̬
	ObjUInt8 m_ActivateState;

	//�������
	ObjUInt16 m_ActivateCounter;
};

#endif
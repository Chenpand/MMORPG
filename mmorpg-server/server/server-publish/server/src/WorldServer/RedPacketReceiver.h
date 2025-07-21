#ifndef __RED_PACKET_RECEIVER_H__
#define __RED_PACKET_RECEIVER_H__

/**
*@brief �����ȡ�߽ṹ
*/

#include <CLDefine.h>
#include <CLSceneObject.h>
#include <CLRedPacketDefine.h>

class WSPlayer;
class RedPacket;

class RedPacketReceiver : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(RedPacketReceiver, Avalon::NullMutex)
public:
	RedPacketReceiver();
	~RedPacketReceiver();
	
	/**
	*@brief ��ȡ��ID
	*/
	inline ObjID_t GetReceiverID() const { return m_ReceiverId; }

	/**
	*@brief ���ID
	*/
	inline ObjID_t GetRedPacketID() const { return m_RedPacketId; }

	/**
	*@brief ��ȡ��ְҵ
	*/
	inline UInt8 GetReceiverOccu() const { return m_Occu; }

	/**
	*@brief ��ȡ�ߵȼ�
	*/
	inline UInt32 GetReceiverLevel() const { return m_Level; }

	/**
	*@brief ��ʼ����ȡ��
	*/
	bool Init(WSPlayer* player, ObjID_t redPacketId, UInt32 money, RedPacket& redPacket);

	/**
	*@brief ��ȡ��Ǯ
	*/
	inline UInt32 GetMoney() const { return m_Money; }

	/**
	*@brief ��ȡ���˺�id
	*/
	inline ObjID_t GetReceiverAccID() const { return m_reciver_accid; }

	/**
	*@brief ��ȡ��Ϣ
	*/
	RedPacketReceiverEntry GetEntry() const;

	RedPacketRecord ToClient() const;

	void SetBset();
public:
	/* ���ݿ���� */
	void InsertToDB();
	void DelFromDB();
	void Update();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("receiver_id", m_ReceiverId)
	CL_DBPROPERTY("red_packet_id", m_RedPacketId)
	CL_DBPROPERTY("occu", m_Occu)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("money_id", m_MoneyId)
	CL_DBPROPERTY("money", m_Money)
	CL_DBPROPERTY("time", m_Time)
	CL_DBPROPERTY("red_packet_owner_name", m_RedPacketOwnerName)
	CL_DBPROPERTY("is_best", m_isBest)
	CL_DBPROPERTY("reciver_accid", m_reciver_accid)
	CL_DBPROPERTY_END()
private:
	// ��ȡ��ID
	ObjUInt64		m_ReceiverId;
	// ���ID
	ObjUInt64		m_RedPacketId;
	// ְҵ
	ObjUInt8		m_Occu;
	// �ȼ�
	ObjUInt32		m_Level;
	// ��ý���
	ObjUInt32		m_Money;
	// ��ȡʱ��
	ObjUInt32		m_Time;
	// �������������
	ObjString		m_RedPacketOwnerName;
	// ����id
	ObjUInt32		m_MoneyId;
	// �Ƿ����
	ObjUInt8        m_isBest;
	// �˺�id
	ObjUInt32		m_reciver_accid;
};

#endif
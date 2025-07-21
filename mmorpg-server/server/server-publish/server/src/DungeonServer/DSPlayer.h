#ifndef _DS_PLAYER_H_
#define _DS_PLAYER_H_

#include <AvalonProtocol.h>
#include <AvalonObjectPool.h>
#include <CLDefine.h>
#include <CLObject.h>
#include <CLObjectDefine.h>
#include <CLPacketExecutor.h> 

class DSItemTransaction;

class DSPlayer : public CLObject
{
	//��������ʱʱ��
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;
public:
	DSPlayer();
	~DSPlayer();

	void SetServiceSessionId(UInt32 serviceSessionId);

	UInt32 GetServiceSessionId() { return m_ServiceSessionId; }

	void SetSourceSceneId(UInt32 sourceSceneId);

	UInt32 GetSourceSceneId() { return m_SourceSceneId; }

	/**
	*@brief ���û�ȡԭ��id
	*/
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	*@brief ������Ϣ��scene
	*/
	bool SendToScene(Avalon::Protocol& protocol);

	/**
	*@brief  ������Ϣ�����
	*/
	bool sendToClient(Avalon::Protocol& protocol);
	
	/**
	*@brief ͬ���¼���Scene
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1 = 0, ObjID_t param2 = 0);

public: //�������
	/**
	*@brief ׼����������
	*/
	bool BeginItemTransaction(DSItemTransaction* transaction);

	/**
	*@brief ��ȡ��ǰ�ĵ�������
	*/
	DSItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	*@brief �����Ʋ�������
	*/
	void UnLockAsserts();

	/**
	*@brief �Ƴ�ָ�����ߣ��ڵ���ʹ����������
	*/
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);

protected:
	UInt32	m_ServiceSessionId; //�淨�������ڵ�id

	UInt32	m_SourceSceneId; //ԴScene�ڵ�id

	UInt32  m_SrcZoneId;     //ԭ��id

	//��������
	DSItemTransaction*	m_pItemTrans;
	//��������ʱ��ʱ
	UInt8			m_ItemTransTimeout;

public:
	static bool InitPlayerProtocols();
	/**
	 *@brief ִ��һ�������Ϣ
	 */
	static void ExecutePacket(ObjID_t playerId, Avalon::Packet* packet);

private:
	/**
	 *@brief ע�����Э��
	 */
	static void RegisterProtocols();

	//�����Ϣ��ִ����
	static CLParamPacketExecutor<DSPlayer*>	m_Executor;
};

typedef std::vector<DSPlayer*> VDSPlayer;

#endif
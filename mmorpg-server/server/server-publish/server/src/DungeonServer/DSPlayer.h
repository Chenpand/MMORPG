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
	//道具事务超时时间
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;
public:
	DSPlayer();
	~DSPlayer();

	void SetServiceSessionId(UInt32 serviceSessionId);

	UInt32 GetServiceSessionId() { return m_ServiceSessionId; }

	void SetSourceSceneId(UInt32 sourceSceneId);

	UInt32 GetSourceSceneId() { return m_SourceSceneId; }

	/**
	*@brief 设置获取原区id
	*/
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	*@brief 发送消息给scene
	*/
	bool SendToScene(Avalon::Protocol& protocol);

	/**
	*@brief  发送消息给玩家
	*/
	bool sendToClient(Avalon::Protocol& protocol);
	
	/**
	*@brief 同步事件到Scene
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1 = 0, ObjID_t param2 = 0);

public: //道具相关
	/**
	*@brief 准备道具事务
	*/
	bool BeginItemTransaction(DSItemTransaction* transaction);

	/**
	*@brief 获取当前的道具事务
	*/
	DSItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	*@brief 解锁财产流程锁
	*/
	void UnLockAsserts();

	/**
	*@brief 移除指定道具，在道具使用流程中用
	*/
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);

protected:
	UInt32	m_ServiceSessionId; //玩法服务器节点id

	UInt32	m_SourceSceneId; //源Scene节点id

	UInt32  m_SrcZoneId;     //原区id

	//道具事务
	DSItemTransaction*	m_pItemTrans;
	//道具事务超时计时
	UInt8			m_ItemTransTimeout;

public:
	static bool InitPlayerProtocols();
	/**
	 *@brief 执行一个玩家消息
	 */
	static void ExecutePacket(ObjID_t playerId, Avalon::Packet* packet);

private:
	/**
	 *@brief 注册玩家协议
	 */
	static void RegisterProtocols();

	//玩家消息包执行器
	static CLParamPacketExecutor<DSPlayer*>	m_Executor;
};

typedef std::vector<DSPlayer*> VDSPlayer;

#endif
#ifndef __CL_VERIFY_NODE_MGR_H__
#define __CL_VERIFY_NODE_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "CLVerifyNode.h"

/**
*@brief 验证服务器节点管理器
*/
class CLVerifyNodeMgr
{
public:
	CLVerifyNodeMgr();
	~CLVerifyNodeMgr();

public:
	/**
	*@brief 通过节点ID查找一个VerifyServer
	*/
	CLVerifyNode* FindNode(UInt32 nodeId);

	/**
	*@brief 更新VerifyServer的负载
	*/
	void UpdateAbality(UInt32 id, UInt32 abality);

	/**
	*@brief 新增VerifyServer
	*/
	void AddNode(CLVerifyNode* node);

	/**
	*@brief 新增VerifyServer
	*/
	void DelNode(UInt32 id);

	/**
	*@brief 获取所有的节点
	*/
	std::vector<CLVerifyNode> GetAllNodes();

	/**
	*@brief 获取负载最低的节点
	*/
	CLVerifyNode* SelectNode();

public:
	/**
	*@brief 发送消息
	*/
	bool SendProtocol(UInt32 id, Avalon::Protocol& protocol);
	void SendPacket(UInt32 id, Avalon::Packet* packet);

public:
	/**
	*@brief 修改负载事件
	*/
	virtual void OnUpdateAbility(UInt32 id, UInt32 ability) {};

	/**
	*@brief 新增node事件
	*/
	virtual void OnAddNode(CLVerifyNode* node) {};

	/**
	*@brief 新增node事件
	*/
	virtual void OnDelNode(UInt32 id) {};

protected:
	//网关会话map
	std::map<UInt32, CLVerifyNode*>		m_Nodes;
};

#endif

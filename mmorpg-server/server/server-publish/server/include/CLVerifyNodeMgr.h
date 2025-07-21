#ifndef __CL_VERIFY_NODE_MGR_H__
#define __CL_VERIFY_NODE_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "CLVerifyNode.h"

/**
*@brief ��֤�������ڵ������
*/
class CLVerifyNodeMgr
{
public:
	CLVerifyNodeMgr();
	~CLVerifyNodeMgr();

public:
	/**
	*@brief ͨ���ڵ�ID����һ��VerifyServer
	*/
	CLVerifyNode* FindNode(UInt32 nodeId);

	/**
	*@brief ����VerifyServer�ĸ���
	*/
	void UpdateAbality(UInt32 id, UInt32 abality);

	/**
	*@brief ����VerifyServer
	*/
	void AddNode(CLVerifyNode* node);

	/**
	*@brief ����VerifyServer
	*/
	void DelNode(UInt32 id);

	/**
	*@brief ��ȡ���еĽڵ�
	*/
	std::vector<CLVerifyNode> GetAllNodes();

	/**
	*@brief ��ȡ������͵Ľڵ�
	*/
	CLVerifyNode* SelectNode();

public:
	/**
	*@brief ������Ϣ
	*/
	bool SendProtocol(UInt32 id, Avalon::Protocol& protocol);
	void SendPacket(UInt32 id, Avalon::Packet* packet);

public:
	/**
	*@brief �޸ĸ����¼�
	*/
	virtual void OnUpdateAbility(UInt32 id, UInt32 ability) {};

	/**
	*@brief ����node�¼�
	*/
	virtual void OnAddNode(CLVerifyNode* node) {};

	/**
	*@brief ����node�¼�
	*/
	virtual void OnDelNode(UInt32 id) {};

protected:
	//���ػỰmap
	std::map<UInt32, CLVerifyNode*>		m_Nodes;
};

#endif

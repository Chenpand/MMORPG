#include "CLVerifyNodeMgr.h"

CLVerifyNodeMgr::CLVerifyNodeMgr()
{

}

CLVerifyNodeMgr::~CLVerifyNodeMgr()
{

}

CLVerifyNode* CLVerifyNodeMgr::FindNode(UInt32 nodeId)
{
	auto itr = m_Nodes.find(nodeId);
	return itr == m_Nodes.end() ? NULL : itr->second;
}

void CLVerifyNodeMgr::UpdateAbality(UInt32 nodeId, UInt32 abality)
{
	auto node = FindNode(nodeId);
	if (!node)
	{
		return;
	}

	if (abality == node->GetAbality())
	{
		return;
	}

	InfoLogStream << "node(" << nodeId << ") ability(" << abality << ")." << LogStream::eos;
	node->SetAbality(abality);
	OnUpdateAbility(nodeId, abality);
}

void CLVerifyNodeMgr::AddNode(CLVerifyNode* node)
{
	if (!node)
	{
		return;
	}

	m_Nodes[node->GetID()] = node;
	OnAddNode(node);
}

void CLVerifyNodeMgr::DelNode(UInt32 id)
{
	InfoLogStream << "verify node(" << id << ") offline." << LogStream::eos;

	auto itr = m_Nodes.find(id);
	if (itr == m_Nodes.end())
	{
		return;
	}

	delete itr->second;
	m_Nodes.erase(itr);
	OnDelNode(id);
}

std::vector<CLVerifyNode> CLVerifyNodeMgr::GetAllNodes()
{
	std::vector<CLVerifyNode> nodes;

	for (auto itr : m_Nodes)
	{
		auto node = itr.second;
		if (node)
		{
			nodes.push_back(*node);
		}
	}

	return nodes;
}

CLVerifyNode* CLVerifyNodeMgr::SelectNode()
{
	// 负载均衡
	// 这里选用加权随机的负载均衡方法，即能力越大的服务器被选中的概率越大

	std::vector<CLVerifyNode*> allUsefulNodes;
	UInt32 totalAbality = 0;
	for (auto itr : m_Nodes)
	{
		auto node = itr.second;
		if (!node || node->GetAbality() == 0)
		{
			continue;
		}

		totalAbality += node->GetAbality();
		allUsefulNodes.push_back(node);
	}

	UInt32 randProb = Avalon::Random::RandBetween(1, totalAbality);
	for (auto node : allUsefulNodes)
	{
		if (randProb <= node->GetAbality())
		{
			return node;
		}
		randProb -= node->GetAbality();
	}

	return allUsefulNodes.empty() ? NULL : allUsefulNodes[allUsefulNodes.size() - 1];
}

bool CLVerifyNodeMgr::SendProtocol(UInt32 id, Avalon::Protocol& protocol)
{
	auto node = FindNode(id);
	if (node)
	{
		node->SendProtocol(protocol);
		return true;
	}
	else
	{
		return false;
	}
}

void CLVerifyNodeMgr::SendPacket(UInt32 id, Avalon::Packet* packet)
{
	auto node = FindNode(id);
	if (node)
	{
		node->SendPacket(packet);
	}
}
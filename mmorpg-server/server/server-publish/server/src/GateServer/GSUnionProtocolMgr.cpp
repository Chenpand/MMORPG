#include "GSUnionProtocolMgr.h"

#include <CLProtocolDefine.h>

void GSUnionProtocolMgr::Init()
{
	RegisterUnionFunType();
}

void GSUnionProtocolMgr::RegisterUnionFunType()
{
	// ½ð±Ò¼ÄÊÛ
	m_UnionProtocolFunMap[CLProtocol::FUNCTION_GOLD_CONSIGNMENT] = USFT_GOLD_CONSIGNMENT;
}

UInt32 GSUnionProtocolMgr::GetProcotolUnionType(UInt32 protocolId)
{
	auto it = m_UnionProtocolFunMap.find(CL_GET_FUNCTIONTYPE_FROM_PROTOCOLID(protocolId));
	return it != m_UnionProtocolFunMap.end() ? it->second : USFT_NONE;
}
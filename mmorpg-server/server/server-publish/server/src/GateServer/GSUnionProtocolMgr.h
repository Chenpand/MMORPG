#ifndef _UNION_PROTOCOL_MGR_H_
#define _UNION_PROTOCOL_MGR_H_

#include <CLDefine.h>

/**
 *@brief ������union����Ϣ�������ĸ�union�����
 */
class GSUnionProtocolMgr : public Avalon::Singleton<GSUnionProtocolMgr>
{
public:
	GSUnionProtocolMgr() {}
	~GSUnionProtocolMgr() {}

	void Init();

	void RegisterUnionFunType();

	UInt32 GetProcotolUnionType(UInt32 protocolId);

private:

	HashMap<UInt32, UInt32> m_UnionProtocolFunMap;
};

#endif // !_UNION_PROTOCOL_MGR_H_


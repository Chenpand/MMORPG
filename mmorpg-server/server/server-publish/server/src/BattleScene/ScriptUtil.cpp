#include "ScriptUtil.h"

UInt32 ScriptVector64::Size()
{
	return m_vec.size();
}

ObjID_t ScriptVector64::Get(UInt32 i)
{
	if (i < m_vec.size())
	{
		return m_vec[i];
	}
	return 0;
}
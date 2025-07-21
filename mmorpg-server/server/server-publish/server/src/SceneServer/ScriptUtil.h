#ifndef _SCRIPT_UTIL_H_
#define _SCRIPT_UTIL_H_

#include <CLDefine.h>
#include <vector>

class ScriptVector64
{
public:
	ScriptVector64(std::vector<ObjID_t>& vec) : m_vec(vec) {}
	~ScriptVector64() {}
public:
	UInt32 Size();
	ObjID_t Get(UInt32 i);
private:
	std::vector<ObjID_t> m_vec;
};

#endif
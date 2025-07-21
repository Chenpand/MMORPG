#ifndef __AGENT_HEAD_H__
#define __AGENT_HEAD_H__
#include "Common/common_base.h"
#include "Common/common_codeengine.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server {

enum enAgentConnectType
{
	agent_type_connect_alive = 1,  //保持连接
	agent_type_connect_close = 2,  //断开连接
};
class CAgentHead
{
public:
	CAgentHead();
	~CAgentHead();

	int32_t encode(char* pszCode, int32_t& iCodeSize);

	int32_t decode(const char *pszCode, const int32_t iCodeSize);

	void dump();

	int32_t size();

	int32_t m_iPackageSize;	  //整个包的长度
	int8_t  m_cControlType;  //控制类型，长连还是短连
	int16_t m_shKeyLen;
	char	m_szKeyName[max_string_key_length];

};

}}//namespace KingNet { namespace Server {

#endif  //~__AGENT_HEAD_H__




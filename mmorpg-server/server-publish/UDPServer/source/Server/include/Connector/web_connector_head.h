#ifndef __WEB_CONNECTOR_HEAD_H__
#define __WEB_CONNECTOR_HEAD_H__
#include "Common/common_base.h"
#include "Common/common_codeengine.h"
#include "gameplatform_message.h"
#include "Connector/connector_head.h"
namespace KingNet { namespace Server {


//#define GROUPTRANSFORM_MAXCONNECTORNUMBER 128
//#define CONNECTOR_MESSAGE_MAXSIZE  //(connector_head_base_size+2+GROUPTRANSFORM_MAXCONNECTORNUMBER*sizeof(uint16_t))


class CWebNetHead
{
public:
	CWebNetHead();
	~CWebNetHead();

	/*
	* @method:    encode 编码
	* @fullname:  
	* @access:    public 
	* @param: char * pszCode 传出编码后的code
	* @param: int32_t & iCodeSize 传入pszCode的最大长度，传出实际code的长度 
	* @return:   int 
	* - success: 编码成功
	* - 其他: 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t encode(char* pszCode, int32_t& iCodeSize);

	/*
	* @method:    decode 解码
	* @fullname:  
	* @access:    public 
	* @param: const char * pszCode 传入待解码的code
	* @param: const int32_t iCodeSize 待解码code的长度
	* @return:   int
	* - success : 成功
	* - 其他: 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t decode(const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen=NULL);

	/*
	* @method:    dump 把头部信息dump出来（到文件，stdout等）
	* @fullname: 
	* @access:    public 
	* @return:   void
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	void dump();

	/*
	* @method:    size 计算赋值后的router head的实际长度
	* @fullname:  KingNet::Server::CWebNetHead::size
	* @access:    public 
	* @return:   KingNet::int32_t 返回头部长度()
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t size();

	int32_t m_iPackageSize;//整个包的长度
	int8_t m_cControlType;
	uint16_t m_unConnectionIndex;

	uint32_t m_iUniqueID;   //连接的唯一标识，m_unConnectionIndex有可能重用
	//m_unConnectionIndex 和m_iUniqueID组合标识一个连接
	union __U
	{
		struct stSourceIPInfo
		{
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;
		}m_stSourceIPInfo;				//head_type_start_request

		int32_t m_iTime;				//head_type_transform_request
										//( c->s :表示socket创建的时间, s->c: 是控制connect server是否断开连接, 0 表示不断开连接， >0 表示断开连接)
										
		struct __stDisconnectInfo__
		{
			int8_t m_cDisconnectType;	//取值enmForceCloseType
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;

		}m_stDisconnectInfo;			//head_type_force_close_connection . 
	
		struct stClientRequestDisconnect
		{
			int32_t m_iUin;
			int8_t m_cDisconnectReason;
		}m_stClientDisconnect;			//head_type_disconnect_notify

	}m_stControlInfo;


protected:
private:
};

}}//namespace KingNet { namespace Server {

#endif




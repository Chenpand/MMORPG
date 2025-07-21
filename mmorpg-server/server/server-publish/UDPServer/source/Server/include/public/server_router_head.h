#ifndef __SERVER_ROUTER_HEAD_H__
#define __SERVER_ROUTER_HEAD_H__
#include "Common/common_base.h"
#include "Common/common_codeengine.h"
namespace KingNet { namespace Server {

#define max_routerhead_length 519 //包头最大的长度
#define max_option_size 128
#define max_dest_count	126

#define ROUTERSERVER_HEARTBEAT_TIME 30 //心跳时间

enum
{
	router_head_basic_size = 14,			//router head的基本长度
											//sizeof(int32_t) + sizeof(int32_t) + 4* sizeof(int8_t) + sizeof(int16_t)
};
//typedef char int8_t;
//typedef unsigned char uint8_t;

/*以下定义转发类型*/
enum enmTransfType
{
    trans_type_null      = -1,          //invalid
	trans_type_p2p       = 0,			//点对点
	trans_type_p2g       = 1,			//组播
	trans_type_broadcast = 2,   		//广播
	trans_type_by_key    = 3,			//根据关键字转发
};


enum enmRouterOptionFlag
{
	router_option_flag_reserved		= 0x01,				//若设置此标识，则要保证该消息发送到db
	router_option_flag_command		= 0x02,				//若设置此标识,则表示该消息'命令消息'
};

enum enProxyCmds
{
	COMMAND_NULL	= 0,
	COMMAND_LOGIN	= 1,			//每个server登录上来连接到router,则需要发送此消息以表示身份(无需消息体)
};

class CRouterHead
{
public:
	CRouterHead();
	~CRouterHead();

 
	/*
	* @method:    size 计算赋值后的router head的实际长度
	* @fullname:  KingNet::Server::CRouterHead::size
	* @access:    public 
	* @return:   KingNet::int32_t 返回头部长度(固定长度 + option长度)
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t size();

	/*
	* @method:    encode 编码
	* @fullname:  CRouterHead::encode
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
	* @fullname:  CRouterHead::decode
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
	* @fullname:  CRouterHead::dump
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

	//协议包总长度
	int32_t m_iTotalSize;
	//源实体的ID
	int32_t m_iSourceEntityID;
	//源实体类型
	int8_t m_cSourceEntityType;
	//目的实体类型
	int8_t m_cDestEntityType;
	//转发类型，取值为enum enmTransfType
	int8_t m_cTransferType;
	union _U
	{
		int32_t m_iUin;				//m_cTransferType = trans_type_by_key有效
		int32_t m_iDestEntityID;	//m_cTransferType = trans_type_p2p 有效
		struct _tagSvrs				
		{
			int16_t m_nDestCount;
			int32_t m_aiDestsID[max_dest_count];
		} stSvrs;					//m_cTransferType = trans_type_p2g 有效
	} m_stTransferInfo;
	uint8_t m_byOpFlag;				//enmRouterOptionFlag, 按位与
	int16_t m_nOptLength;			//m_abyOpt字段实际大小
	uint8_t m_abyOpt[max_option_size];	//option
	//short m_nBodyLength;
};

}}//namespace KingNet { namespace Server {

#endif //__SERVER_ROUTER_HEAD_H__

#ifndef __LOGICSERVER_HEAD_H__
#define __LOGICSERVER_HEAD_H__
#include "Common/common_base.h"
#include "Common/common_codeengine.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server {


//#define GROUPTRANSFORM_MAXCONNECTORNUMBER 128
//#define CONNECTOR_MESSAGE_MAXSIZE  //(connector_head_base_size+2+GROUPTRANSFORM_MAXCONNECTORNUMBER*sizeof(uint16_t))

enum enmClientCloseReason
{
	enmClosedReason_Service = 0,					//Service主动关闭
	enmClosedReason_Client = 1,						//client主动关闭
	enmClosedReason_IOError = 2,					//IO读写错误
	enmClosedReason_DataError = 3,					//无法识别收到客户端的消息错误
	enmClosedReason_QueueError = 4,					//无法识别收到客户端的消息错误
	enmClosedReason_RecvBuffError = 5,				//接收缓冲错误(满)
	enmClosedReason_SendBuffError = 6,				//发送缓冲错误(满)

	enmClosedReason_ClientTimeOut = 7,				//客户端超时(已经与service进程交换过数据)
	enmClosedReason_PushServiceChannelError = 8,	//写内存管道错误
	enmClosedReason_AppError = 9,					//系统错误
	enmClosedReason_InvalidConnection = 10,			//连接无效（没有与后端server有过数据交换，不需要通知service进程进行清理动作）

	enmClosedReason_InvalidSignature = 11,			//无效的数字签名 
};

typedef enum enmForceCloseType
{
	enmForceCloseType_Now=1,
	enmForceCloseType_Delay //等数据全部发送成功后后关闭该连接.
}ForceCloseType;


typedef enum enmCtrMsgType
{
	head_type_invalid =					(int8_t)-1,

	//enmCtrMsgType_Start_Req=0x01,//方向: client-->Service,client首次发送消息给service.
	head_type_start_request =			(int8_t)0x01,

	//enmCtrMsgType_Transform_Req, //方向: client <--> Service, client与service之间互相传递消息(备:若client第一次发送消息，则用前一条指条发送
	head_type_transform_request =		(int8_t)0x02,

	//enmCtrMsgType_ForceCloseConnect_Req,//方向:client <-- Service,Service主动关闭client的链接.
	head_type_force_close_connection=	(int8_t)0x03,


	//enmCtrMsgType_DisconnectNotify_Req, //方向:client --> Service, client主动断线
	head_type_disconnect_notify		=	(int8_t)0x04,


	//enmCtrMsgType_GroupTransform_Req    //方向:client <-- Service, Service给一组用户发送消息(组播) 
	head_type_group_transform_request =		(int8_t)0x05,		//暂不支持

	//
	head_type_disconnect_by_mac		=	(int8_t)0x06,	//方向:client --> Service, client主动断线，服务器根据MAC地址hash

}CtrMsgType;


class CNetHead
{
public:
	CNetHead();
	~CNetHead();

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
	* @fullname:  Game51::Server::CNetHead::size
	* @access:    public 
	* @return:   Game51::int32_t 返回头部长度()
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
			int32_t m_iUid;
			int8_t m_cDisconnectReason;
		}m_stClientDisconnect;			//head_type_disconnect_notify

		int64_t m_lMac;					//head_type_disconnect_by_mac

	}m_stControlInfo;


protected:
private:
};

}}//namespace KingNet { namespace Server {

#endif




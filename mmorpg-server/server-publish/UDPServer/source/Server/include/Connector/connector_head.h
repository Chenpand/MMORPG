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
	enmClosedReason_Service = 0,					//Service�����ر�
	enmClosedReason_Client = 1,						//client�����ر�
	enmClosedReason_IOError = 2,					//IO��д����
	enmClosedReason_DataError = 3,					//�޷�ʶ���յ��ͻ��˵���Ϣ����
	enmClosedReason_QueueError = 4,					//�޷�ʶ���յ��ͻ��˵���Ϣ����
	enmClosedReason_RecvBuffError = 5,				//���ջ������(��)
	enmClosedReason_SendBuffError = 6,				//���ͻ������(��)

	enmClosedReason_ClientTimeOut = 7,				//�ͻ��˳�ʱ(�Ѿ���service���̽���������)
	enmClosedReason_PushServiceChannelError = 8,	//д�ڴ�ܵ�����
	enmClosedReason_AppError = 9,					//ϵͳ����
	enmClosedReason_InvalidConnection = 10,			//������Ч��û������server�й����ݽ���������Ҫ֪ͨservice���̽�����������

	enmClosedReason_InvalidSignature = 11,			//��Ч������ǩ�� 
};

typedef enum enmForceCloseType
{
	enmForceCloseType_Now=1,
	enmForceCloseType_Delay //������ȫ�����ͳɹ����رո�����.
}ForceCloseType;


typedef enum enmCtrMsgType
{
	head_type_invalid =					(int8_t)-1,

	//enmCtrMsgType_Start_Req=0x01,//����: client-->Service,client�״η�����Ϣ��service.
	head_type_start_request =			(int8_t)0x01,

	//enmCtrMsgType_Transform_Req, //����: client <--> Service, client��service֮�以�ഫ����Ϣ(��:��client��һ�η�����Ϣ������ǰһ��ָ������
	head_type_transform_request =		(int8_t)0x02,

	//enmCtrMsgType_ForceCloseConnect_Req,//����:client <-- Service,Service�����ر�client������.
	head_type_force_close_connection=	(int8_t)0x03,


	//enmCtrMsgType_DisconnectNotify_Req, //����:client --> Service, client��������
	head_type_disconnect_notify		=	(int8_t)0x04,


	//enmCtrMsgType_GroupTransform_Req    //����:client <-- Service, Service��һ���û�������Ϣ(�鲥) 
	head_type_group_transform_request =		(int8_t)0x05,		//�ݲ�֧��

	//
	head_type_disconnect_by_mac		=	(int8_t)0x06,	//����:client --> Service, client�������ߣ�����������MAC��ַhash

}CtrMsgType;


class CNetHead
{
public:
	CNetHead();
	~CNetHead();

	/*
	* @method:    encode ����
	* @fullname:  
	* @access:    public 
	* @param: char * pszCode ����������code
	* @param: int32_t & iCodeSize ����pszCode����󳤶ȣ�����ʵ��code�ĳ��� 
	* @return:   int 
	* - success: ����ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t encode(char* pszCode, int32_t& iCodeSize);

	/*
	* @method:    decode ����
	* @fullname:  
	* @access:    public 
	* @param: const char * pszCode ����������code
	* @param: const int32_t iCodeSize ������code�ĳ���
	* @return:   int
	* - success : �ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t decode(const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen=NULL);

	/*
	* @method:    dump ��ͷ����Ϣdump���������ļ���stdout�ȣ�
	* @fullname: 
	* @access:    public 
	* @return:   void
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	void dump();

	/*
	* @method:    size ���㸳ֵ���router head��ʵ�ʳ���
	* @fullname:  Game51::Server::CNetHead::size
	* @access:    public 
	* @return:   Game51::int32_t ����ͷ������()
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t size();

	int32_t m_iPackageSize;//�������ĳ���
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
										//( c->s :��ʾsocket������ʱ��, s->c: �ǿ���connect server�Ƿ�Ͽ�����, 0 ��ʾ���Ͽ����ӣ� >0 ��ʾ�Ͽ�����)
										
		struct __stDisconnectInfo__
		{
			int8_t m_cDisconnectType;	//ȡֵenmForceCloseType
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




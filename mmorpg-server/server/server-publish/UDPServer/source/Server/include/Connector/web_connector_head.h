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
	* @fullname:  KingNet::Server::CWebNetHead::size
	* @access:    public 
	* @return:   KingNet::int32_t ����ͷ������()
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

	uint32_t m_iUniqueID;   //���ӵ�Ψһ��ʶ��m_unConnectionIndex�п�������
	//m_unConnectionIndex ��m_iUniqueID��ϱ�ʶһ������
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
			int32_t m_iUin;
			int8_t m_cDisconnectReason;
		}m_stClientDisconnect;			//head_type_disconnect_notify

	}m_stControlInfo;


protected:
private:
};

}}//namespace KingNet { namespace Server {

#endif




#ifndef _CL_NETHEADER_H_
#define _CL_NETHEADER_H_

#include <AvalonDefine.h>
#include <CLDefine.h>
#include <stdint.h>

typedef enum enmControlId
{
	enmControlId_invalid	= -1,
	enmControlId_notify_start = 0x01,		//client-->Service,client�״η�����Ϣ��service.
	enmControlId_request_transmit = 0x02,	//client <--> Service, client��service֮�以�ഫ����Ϣ(��:��client��һ�η�����Ϣ������ǰһ��ָ������
	enmControlId_request_disconnect_client = 0x03,	// Service --> client,Service�����ر�client������.
	enmControlId_notify_disconnect	= 0x04,		//client --> Service, client��������
	enmControlId_notify_ping = 0x05,		// udpserver -> relayserver, ֪ͨ��ͻ���֮���pingֵ
}NetHeadControlID;

typedef enum enmDisconnectReason
{
	enmDisconnectReason_system				= 1,
	enmDisconnectReason_client_close		= 2,				//client�����ر�(��ʱ�������رյ�)
	enmDisconnectReason_service				= 3,				//service����Ҫ��ر�
	enmDisconnectReason_invalid_newclient	= 4,				//
}EDisconnectReason;

typedef enum enmForceCloseType
{
	enmForceCloseType_Now = 1,
	enmForceCloseType_Delay //������ȫ�����ͳɹ����رո�����.
}ForceCloseType;

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
	int32_t decode(const char *pszCode, const int32_t iCodeSize, int16_t *pHeadLen = NULL);

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
	* @method:    size ���㸳ֵ��� head��ʵ�ʳ���
	* @fullname:  
	* @access:    public
	* @return:    ����ͷ������()
	* @qualifier:
	* @note
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated
	*/
	int32_t size();
public:
	int32_t		m_iPackageSize;		//���ܳ���
	int8_t		m_cCtrlId;			//����id
	uint32_t	m_unAccId;			//�ʺ�id
	uint64_t	m_RoleId;			//��ɫid

	union __U
	{
		struct stSourceIPInfo
		{
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;
		}m_stSourceIPInfo;				//enmControlId_notify_start

		int32_t m_iPing;				//enmControlId_request_transmit   ( c->s :��ʾclient pingֵ, s->c: �ǿ���connect server�Ƿ�Ͽ�����, 0 ��ʾ���Ͽ����ӣ� >0 ��ʾ�Ͽ�����)

		struct __stDisconnectInfo__
		{
			int8_t m_cDisconnectType;	//ȡֵenmForceCloseType
			int32_t m_iSourceIP;
			int16_t m_nSourcePort;

		}m_stDisconnectInfo;			//enmControlId_request_disconnect_client . 
		

		struct stClientRequestDisconnect
		{
			//int32_t m_iAccId;
			int8_t m_cDisconnectReason;		//ȡֵ��enmDisconnectReason
		}m_stClientDisconnect;			//enmControlId_notify_disconnect

	}m_stControlInfo;

private:
};


#endif

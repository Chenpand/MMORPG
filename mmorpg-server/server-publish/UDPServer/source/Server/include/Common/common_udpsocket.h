#ifndef __COMMON_UDPSOCKET_H__
#define __COMMON_UDPSOCKET_H__

#include <sys/types.h>
#include "Common/common_base.h"
#include "Common/common_bitmap.h"
#include "Common/common_set.h"
#include "common_namespace_definition.h"

SERVER_BEGIN


/*	
*  ������Ҫ��װ��UDP�Ļ������ͺͽ��ղ�����Ϊ��֤ʱ������Ч�ʣ�
* �����˷�Ƭ���ͣ����ն���װ�ķ���������ÿһ��������UDPʱ��������һ��
* 4B��ΨһFlagSequence, (���ð���Ҫ��Ƭ����ÿһƬ����Ҫ������ͬ��FlagSequence)
*/
class CUDPSocket
{
public:
	enum 
	{
		max_slot_count		= 128,
		max_unit_size		= 1024,
		max_unit_count		= 64,
		max_timeout			= 3
	};

	struct __stUDPUnit
	{
		int16_t m_nUnitSize;
		char m_szUnit[max_unit_size];
	};

	//һ��slot���ڽ���һ��UDP��
	struct __stUDPSlot
	{
		u_long	m_unlSectionID;		//��UDP����unique id
		int16_t m_nUnitCount;		//��UDP���ķ�Ƭ����
		uint16_t m_unSrcPort;
		u_long	m_ulSrcIP;
		time_t	m_tmLastRcvTime;	//���һ���յ����ݵ�ʱ��
		struct __stUDPUnit m_astUnits[max_unit_count];	//���ķ�Ƭ
	} ;

public:
	//constructor
	CUDPSocket();

	//destructor
	~CUDPSocket();

	//set socket fd to object
	int32_t set_fd(int32_t fd);

	//close the udp socket object
	int32_t close_socket();

	//add the udp socket fd to the fd set which to be checked
	int32_t add_fd_set(fd_set* pfd_set);

	//check whether the socket fd was setted
	int32_t is_fd_be_setted(fd_set* pfd_set);

	/*
	* @method:    receive_data ����udp����
	* @fullname:  Game51::Server::CUDPSocket::receive_data
	* @access:    public 
	* @return:   Game51::int32_t ���ؽ��յ������������ݰ���Ŀ
	* - < 0: ʧ��
	* - >=0: ����������
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t receive_data();

	/*
	* @method:    get_one_code �ӽ��յ�������������ͷȡһ���������ݰ�
	* @fullname:  Game51::Server::CUDPSocket::get_one_code
	* @access:    public 
	* @param: char * pszBuffer
	* @param: int32_t iBufferSize
	* @param: char * pszSrcIP
	* @param: uint16_t * punSrcPort
	* @return:   Game51::int32_t	����ȡ�õ����ݰ���С
	* - <0 : ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t get_one_code(char* pszBuffer, int32_t iBufferSize, char *pszSrcIP, uint16_t* punSrcPort);

	/*
	* @method:    send_one_code ��Ƭ�������ݰ�
	* @fullname:  Game51::Server::CUDPSocket::send_one_code
	* @access:    public 
	* @param: char * pszBuffer
	* @param: int32_t iBufferSize
	* @param: char * pszDstIP
	* @param: uint16_t unDstPort
	* @return:   Game51::int32_t �����ѷ��͵����ݴ�С
	* - < 0 : ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t send_one_code(char *pszBuffer, int32_t iBufferSize, char *pszDstIP, uint16_t unDstPort);

protected:
	int32_t clear_slot_of(int32_t slot);

protected:

	u_long m_unSectionSequence; //����Ψһ��ʶһ��udp���ݰ���sequence(һ��udp�����ܻ��Ϊ��Ƭ���з��ͣ�
								//��ÿһƬ�Ķ����ڸð�(��m_unSectionSequence��ʶ))
	int32_t m_iSocketfd;
	
	CSet<int32_t, max_slot_count> m_stUsedSlotIdxsSet;	//�Ѿ�ʹ�õ�m_astSlots[]��Ԫ���±�
	CSet<int32_t, max_slot_count> m_stReadySlotIdxSet;	//�Ѿ�����������ɵ�m_astSlots[]��Ԫ���±�

	CBitmap<max_slot_count/bytesize> m_bmSlotUsed;
	
	struct __stUDPSlot m_astUDPSlots[max_slot_count];		//������װ���հ���slot, ���е�Ԫ�ط�Ϊ�������: ���е�
														//����ʹ�õģ����о��������ɵ�
};

SERVER_END

#endif //__COMMON_UDPSOCKET_H__


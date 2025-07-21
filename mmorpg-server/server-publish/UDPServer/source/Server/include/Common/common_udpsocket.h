#ifndef __COMMON_UDPSOCKET_H__
#define __COMMON_UDPSOCKET_H__

#include <sys/types.h>
#include "Common/common_base.h"
#include "Common/common_bitmap.h"
#include "Common/common_set.h"
#include "common_namespace_definition.h"

SERVER_BEGIN


/*	
*  该类主要封装了UDP的基本发送和接收操作，为保证时序和提高效率，
* 采用了分片发送，接收端组装的方法，发送每一个完整的UDP时，都带有一个
* 4B的唯一FlagSequence, (若该包需要分片，则每一片都需要带有相同的FlagSequence)
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

	//一个slot用于接收一个UDP包
	struct __stUDPSlot
	{
		u_long	m_unlSectionID;		//该UDP包的unique id
		int16_t m_nUnitCount;		//该UDP包的分片总数
		uint16_t m_unSrcPort;
		u_long	m_ulSrcIP;
		time_t	m_tmLastRcvTime;	//最近一次收到数据的时间
		struct __stUDPUnit m_astUnits[max_unit_count];	//包的分片
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
	* @method:    receive_data 接收udp数据
	* @fullname:  Game51::Server::CUDPSocket::receive_data
	* @access:    public 
	* @return:   Game51::int32_t 返回接收到的完整的数据包数目
	* - < 0: 失败
	* - >=0: 完整包个数
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t receive_data();

	/*
	* @method:    get_one_code 从接收到的完整包队列头取一个完整数据包
	* @fullname:  Game51::Server::CUDPSocket::get_one_code
	* @access:    public 
	* @param: char * pszBuffer
	* @param: int32_t iBufferSize
	* @param: char * pszSrcIP
	* @param: uint16_t * punSrcPort
	* @return:   Game51::int32_t	返回取得的数据包大小
	* - <0 : 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t get_one_code(char* pszBuffer, int32_t iBufferSize, char *pszSrcIP, uint16_t* punSrcPort);

	/*
	* @method:    send_one_code 分片发送数据包
	* @fullname:  Game51::Server::CUDPSocket::send_one_code
	* @access:    public 
	* @param: char * pszBuffer
	* @param: int32_t iBufferSize
	* @param: char * pszDstIP
	* @param: uint16_t unDstPort
	* @return:   Game51::int32_t 返回已发送的数据大小
	* - < 0 : 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t send_one_code(char *pszBuffer, int32_t iBufferSize, char *pszDstIP, uint16_t unDstPort);

protected:
	int32_t clear_slot_of(int32_t slot);

protected:

	u_long m_unSectionSequence; //用于唯一标识一个udp数据包的sequence(一个udp包可能会分为几片进行发送，
								//但每一片的都属于该包(由m_unSectionSequence标识))
	int32_t m_iSocketfd;
	
	CSet<int32_t, max_slot_count> m_stUsedSlotIdxsSet;	//已经使用的m_astSlots[]的元素下标
	CSet<int32_t, max_slot_count> m_stReadySlotIdxSet;	//已经接收数据完成的m_astSlots[]的元素下标

	CBitmap<max_slot_count/bytesize> m_bmSlotUsed;
	
	struct __stUDPSlot m_astUDPSlots[max_slot_count];		//用于组装接收包的slot, 其中的元素分为三种情况: 空闲的
														//正在使用的，还有就是组包完成的
};

SERVER_END

#endif //__COMMON_UDPSOCKET_H__


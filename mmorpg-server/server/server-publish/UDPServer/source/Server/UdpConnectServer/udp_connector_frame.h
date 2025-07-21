#ifndef __UDPCONNECTOR_FRAME_H__
#define __UDPCONNECTOR_FRAME_H__

#include "udp_connector_define.h"

#include <sys/epoll.h> 

//???????
namespace KingNet { namespace Server{

	class	CShmMemory;
	class	CCodeQueue;
	class   CConfigFile;
	class   CFlag;

}}//

namespace KingNet { namespace Server { namespace UdpConnectorServer {

	struct stSocketStatistics 
	{
		uint32_t m_unSocketCount;
	};

//singleton framework
class CUdpConnFrame
{
public:
	CUdpConnFrame();                                                    
	~CUdpConnFrame();

	//
	int32_t  initialize();                                             
	int32_t  working();		                                            

	void* operator new( size_t size)throw();                             
	void  operator delete(void* ptr);   

	static CUdpConnFrame* spawn();                             
	static int32_t size();		

private:
	CUdpConnFrame(const CUdpConnFrame& original);
	CUdpConnFrame& operator = (const CUdpConnFrame& original);

private:		
	int32_t  load_config();
	int32_t  reload_config();

	//create channel
	int32_t  make_channel();   

	//receive message
	int32_t  recv_messages();                                        

	//receive client message
	int32_t  recv_client_messages(int32_t fd);

	int  check_and_send_data();                                  
		
	int32_t  socket_send(int fd, char* data, int32_t size, struct sockaddr_in *pAddress);

	int32_t  socket_recv(int fd, char* data, int32_t size, struct sockaddr_in *pAddress);
	
protected:
	void check_flag();

	int32_t get_package_size(const char* pszInCode, const int32_t iInSize, int32_t& iPackageSize);

	int32_t process_code_from_service(const char* pszInCode, const int32_t iInCodeSize);

	void transfer_code(int32_t iIndex);

	int32_t transfer_one_code(int32_t iIndex, const char* pszCode, const int32_t iCodeSize);

//	int32_t process_request_get_ip_port(const char* pszCode, const int32_t iCodeSize);

	int32_t get_uid_from_code(const char* pszCode, const int32_t iCodeSize);


	int32_t open_epoll_socket(uint16_t port);

	//initialize epoll
	int32_t  epoll_init(void);

	//create one epoll socket
	int  epoll_socket(int domain, int type, int protocol);	

	//add one socket
	int  epoll_new_socket(int fd);

	void clear_current_socket();

	//close socket socket
	void  epoll_close(int fd);

	//destroy epoll
	void  epoll_destroy();	

	void log_statistics();

public:
	static void set_meory(CShmMemory* pstShmMemory);

private:
	static CShmMemory*        m_pstShmMemory;                      //shared memory
		
private:
	stServerConfig	m_stConfig;                      
	int	m_iSocket;	
        
	stSocketInfo m_stSocketInfo[max_socket_count];   /*socket?????ï…???????????????*/		
 	stSocketInfo* m_pstCurrentSocket;                   /*?????socket?????*/ 

	CCodeQueue* m_pstCSChannel;  //c2s channel 
	CCodeQueue* m_pstSCChannel;  //s2c channel

	CCodeQueue* m_pstACChannel[auxiliary_handler_count];
	
	char m_szMsgBuf[max_relay_pkg_size]; //
	stSocketStatistics	m_stSocketSatistics;

	//variable of epoll
	struct epoll_event* m_pevents;
	int	m_epoll_fd;
	int m_maxfd;

	struct epoll_event  m_epoll_event;

	time_t  m_tmLastStatisticTime;	//??¦Í??????????
	
private:
	
	};

	extern CShmMemory* g_pstCoreShm;
	extern CShmMemory* g_pstCSChannelShm;
	extern CShmMemory* g_pstSCChannelShm;

#ifdef __MULTI_THREAD__
	extern CShmMemory *g_apShmPipeAuxOut[auxiliary_handler_count];
#endif

	extern CFlag g_byRunFlag;

	extern stConnectorConfig g_stSvrConfig;
	 

}}}//namespace KingNet { namespace Server {  namespace UdpConnectorServer



#endif


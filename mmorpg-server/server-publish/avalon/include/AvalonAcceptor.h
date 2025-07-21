/**
 *@author	 
 *@email	 
 *@date		2011-03-19	
 *@brief	接收器
 */
#ifndef _AVALON_ACCEPTOR_H_
#define _AVALON_ACCEPTOR_H_

#include "AvalonINetEventHandler.h"
#include "AvalonSocket.h"

namespace Avalon
{
	class NetService;

	/**
	 *@brief 接收器
	 */
	class Acceptor : public INetEventHandler
	{
	public:
		Acceptor(const Socket& sock,NetService* service);
		~Acceptor();

	public:
		sock_t GetHandle() const;

		void HandleInput();

		void HandleOutput();

		void HandleException();

		void HandleClose();

		void HandleTick(const Time& now);

	public:
		//监听套接字
		Socket m_Sock;
		//网络服务
		NetService* m_pService;
	};
}

#endif

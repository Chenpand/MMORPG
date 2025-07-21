/**
 *@author	 
 *@email	 
 *@date		2011-03-19	
 *@brief	������
 */
#ifndef _AVALON_ACCEPTOR_H_
#define _AVALON_ACCEPTOR_H_

#include "AvalonINetEventHandler.h"
#include "AvalonSocket.h"

namespace Avalon
{
	class NetService;

	/**
	 *@brief ������
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
		//�����׽���
		Socket m_Sock;
		//�������
		NetService* m_pService;
	};
}

#endif

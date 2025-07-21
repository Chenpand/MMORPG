/**
 *@author	 
 *@email	 
 *@date		2011-03-26	
 *@brief	�����쳣����
 */
#ifndef _AVALON_NETEXCEPTIONS_H_
#define _AVALON_NETEXCEPTIONS_H_

#include <exception>
#include <string>

namespace Avalon
{
	/**
	 *@brief �����쳣����
	 */
	class NetException : public std::exception
	{
	public:
		NetException(const char* what) throw():m_What(what){}
		~NetException() throw(){}

		const char* what() const throw()
		{
			return m_What.c_str();
		}

	private:
		//�쳣
		std::string m_What;
	};

	/**
	 *@brief �������л��쳣
	 */
	class NetSerializeException : public NetException
	{
	public:
		NetSerializeException(const char* what):NetException(what){}
	};
}

#endif

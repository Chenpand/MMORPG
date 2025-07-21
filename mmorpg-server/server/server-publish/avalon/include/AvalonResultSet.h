/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	�����
 */
#ifndef _AVALON_RESULTSET_H_
#define _AVALON_RESULTSET_H_

#include "AvalonDBField.h"
#include <string>

namespace Avalon
{
	/**
	 *@brief ���ݿ������ӿ�
	 */
	class ResultSet
	{
	public:
		virtual ~ResultSet(){}

		/**
		 *@brief �α��Ƶ���һ��
		 */
		virtual bool NextRow() = 0;

		/**
		 *@brief ��ȡ�ֶ���
		 */
		virtual int GetFieldNum() const = 0;

		/**
		 *@brief ��ȡ�ֶ���
		 */
		virtual std::string GetFieldName(int index) const = 0;

		/**
		 *@brief ��ȡһ���ֶ�
		 */
		virtual const DBField GetField(int index) const = 0;

		/**
		 *@brief ��ȡһ���ֶ�
		 */
		virtual const DBField GetField(const char* name) const = 0; 

	private:

	};
}

#endif

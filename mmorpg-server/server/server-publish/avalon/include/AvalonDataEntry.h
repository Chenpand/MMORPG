/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	���ݱ���
 */
#ifndef _AVALON_DATA_ENTRY_H_
#define _AVALON_DATA_ENTRY_H_

#include <AvalonDefine.h>
#include <AvalonDataRow.h>

namespace Avalon
{
	/**
	 *@brief ������
	 */
	struct DataEntry
	{
		virtual ~DataEntry(){}

		/**
		 *@brief ��ȡ��ֵ
		 */
		virtual UInt32 GetKey() const = 0;

		/**
		 *@brief ��ȡ����
		 *@return true ������Ч false������Ч
		 */
		virtual bool Read(DataRow& row) = 0;
	};
}


#endif

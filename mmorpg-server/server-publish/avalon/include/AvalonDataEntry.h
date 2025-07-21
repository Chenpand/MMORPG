/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	数据表项
 */
#ifndef _AVALON_DATA_ENTRY_H_
#define _AVALON_DATA_ENTRY_H_

#include <AvalonDefine.h>
#include <AvalonDataRow.h>

namespace Avalon
{
	/**
	 *@brief 数据项
	 */
	struct DataEntry
	{
		virtual ~DataEntry(){}

		/**
		 *@brief 获取键值
		 */
		virtual UInt32 GetKey() const = 0;

		/**
		 *@brief 读取数据
		 *@return true 数据有效 false数据无效
		 */
		virtual bool Read(DataRow& row) = 0;
	};
}


#endif

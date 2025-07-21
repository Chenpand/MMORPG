/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	数据表
 */
#ifndef _AVALON_DATATABLE_H_
#define _AVALON_DATATABLE_H_

#include <AvalonDefine.h>
#include <fstream>
#include "AvalonDataRow.h"

namespace Avalon
{
	/**
	 *@brief 数据表
	 */
	class DataTable
	{
	public:
		DataTable();
		~DataTable();

	public:
		/**
		 *@brief 加载
		 */
		bool LoadFile(const std::string& filename);

		/**
		 *@brief 获取下一行
		 */
		bool GetNextRow(DataRow& row);

	private:
		//文件流
		std::fstream m_Stream;
	};
}

#endif

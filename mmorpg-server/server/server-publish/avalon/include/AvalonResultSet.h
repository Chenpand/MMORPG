/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	结果集
 */
#ifndef _AVALON_RESULTSET_H_
#define _AVALON_RESULTSET_H_

#include "AvalonDBField.h"
#include <string>

namespace Avalon
{
	/**
	 *@brief 数据库结果集接口
	 */
	class ResultSet
	{
	public:
		virtual ~ResultSet(){}

		/**
		 *@brief 游标移到下一行
		 */
		virtual bool NextRow() = 0;

		/**
		 *@brief 获取字段数
		 */
		virtual int GetFieldNum() const = 0;

		/**
		 *@brief 获取字段名
		 */
		virtual std::string GetFieldName(int index) const = 0;

		/**
		 *@brief 获取一个字段
		 */
		virtual const DBField GetField(int index) const = 0;

		/**
		 *@brief 获取一个字段
		 */
		virtual const DBField GetField(const char* name) const = 0; 

	private:

	};
}

#endif

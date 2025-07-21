/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	mysql结果集合
 */
#ifndef _AVALON_MYSQL_RESULTSET_H_
#define _AVALON_MYSQL_RESULTSET_H_

#include "mysql/mysql.h"
#include "AvalonResultSet.h"

namespace Avalon
{
	/**
	 *@brief mysql结果集
	 */
	class MysqlResultSet : public ResultSet
	{
	public:
		/**
		 *@param res mysql结果集
		 */
		MysqlResultSet(MYSQL_RES* res);
		~MysqlResultSet();

	public:
		/**
		 *@brief 游标移到下一行，若没下一样，返回false
		 */
		bool NextRow();

		int	GetFieldNum() const;

		std::string GetFieldName(int index) const;

		const DBField GetField(int index) const;

		const DBField GetField(const char* name) const;

	private:
		//mysql结果集
		MYSQL_RES*		m_pResult;
		//一行记录
		MYSQL_ROW		m_Row;
		//字段信息
		MYSQL_FIELD*	m_pFields;
		//字段数
		int				m_FieldNum;
		//字段长度
		unsigned long*	m_FieldLen;
	};
}

#endif

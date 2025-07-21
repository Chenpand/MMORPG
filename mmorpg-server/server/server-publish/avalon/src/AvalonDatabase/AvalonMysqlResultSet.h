/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	mysql�������
 */
#ifndef _AVALON_MYSQL_RESULTSET_H_
#define _AVALON_MYSQL_RESULTSET_H_

#include "mysql/mysql.h"
#include "AvalonResultSet.h"

namespace Avalon
{
	/**
	 *@brief mysql�����
	 */
	class MysqlResultSet : public ResultSet
	{
	public:
		/**
		 *@param res mysql�����
		 */
		MysqlResultSet(MYSQL_RES* res);
		~MysqlResultSet();

	public:
		/**
		 *@brief �α��Ƶ���һ�У���û��һ��������false
		 */
		bool NextRow();

		int	GetFieldNum() const;

		std::string GetFieldName(int index) const;

		const DBField GetField(int index) const;

		const DBField GetField(const char* name) const;

	private:
		//mysql�����
		MYSQL_RES*		m_pResult;
		//һ�м�¼
		MYSQL_ROW		m_Row;
		//�ֶ���Ϣ
		MYSQL_FIELD*	m_pFields;
		//�ֶ���
		int				m_FieldNum;
		//�ֶγ���
		unsigned long*	m_FieldLen;
	};
}

#endif

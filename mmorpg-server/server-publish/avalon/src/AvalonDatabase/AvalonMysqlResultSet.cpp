#include "AvalonMysqlResultSet.h"

namespace Avalon
{
	MysqlResultSet::MysqlResultSet(MYSQL_RES* res)
	{
		m_pResult = res;
		m_FieldNum = mysql_num_fields(m_pResult);
		m_pFields = mysql_fetch_fields(m_pResult);
		m_Row = NULL;
		m_FieldLen = NULL;
	}

	MysqlResultSet::~MysqlResultSet()
	{
		mysql_free_result(m_pResult);
	}

	bool MysqlResultSet::NextRow()
	{
		m_Row = mysql_fetch_row(m_pResult);
		m_FieldLen = mysql_fetch_lengths(m_pResult);
		return m_Row != NULL;
	}

	int MysqlResultSet::GetFieldNum() const
	{
		return m_FieldNum;
	}

	std::string MysqlResultSet::GetFieldName(int index) const
	{
		if(index <0 || index >= m_FieldNum) return "";
		return m_pFields[index].name;
	}

	const DBField MysqlResultSet::GetField(int index) const
	{
		const static DBField nullField;
		if(index < 0 || index >= m_FieldNum || NULL == m_Row)
		{
			return nullField;
		}

		DBField::FiledType type = DBField::INTEGER;
		switch(m_pFields[index].type)
		{
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			{
				type = DBField::FLOAT;
			}
			break;
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			{
				type = DBField::STRING;
			}
			break;
		case MYSQL_TYPE_BLOB:
			{
				type = DBField::BINARY;
			}
			break;
		default:
			{
				type = DBField::INTEGER;
			}
			break;
		}

		return DBField(type, m_Row[index], m_FieldLen[index]);
	}

	const DBField MysqlResultSet::GetField(const char* name) const
	{
		const static DBField nullField;
		if(NULL == m_Row) return nullField;

		for(int i = 0; i < m_FieldNum;++i)
		{
			if(strncmp(m_pFields[i].name, name, 64) == 0)
			{
				return GetField(i);
			}
		}
		return nullField;
	}
}


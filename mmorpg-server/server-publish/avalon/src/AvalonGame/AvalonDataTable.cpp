#include "AvalonDataTable.h"

namespace Avalon
{
	DataTable::DataTable()
	{
	}

	DataTable::~DataTable()
	{
		if(m_Stream.is_open())
		{
			m_Stream.close();
		}
	}

	bool DataTable::LoadFile(const std::string& filename)
	{
		if(m_Stream.is_open())
		{
			m_Stream.close();
		}

		m_Stream.open(filename.c_str(), std::ios_base::in);
		return m_Stream.is_open();
	}

	bool DataTable::GetNextRow(DataRow& row)
	{
		if(!m_Stream.is_open()) return false;

		row.Clear();
		if(!std::getline(m_Stream, row.m_Data)) return false;

		size_t len = row.m_Data.length();
		if(len > 0)
		{
			if(row.m_Data.at(len - 1) == '\r')
				row.m_Data = row.m_Data.substr(0, len-1);
		}
		return true;
	}
}

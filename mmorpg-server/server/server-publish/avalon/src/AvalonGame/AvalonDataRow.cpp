#include "AvalonDataRow.h"

namespace Avalon
{
	DataRow::DataRow()
	{
		m_Index = 0;
	}

	DataRow::~DataRow()
	{
	}

	void DataRow::Clear()
	{
		m_Data.clear();
		m_Index = 0;
	}

	UInt32 DataRow::GetColNum()
	{
		if (m_Data.empty()) return 0;
		std::string::size_type index = 0;
		UInt32 num = 0;
		std::string::size_type pos = m_Data.find_first_of('\t', index);
		if (pos == std::string::npos) return 1;
		while (pos != std::string::npos)
		{
			num++;
			index = pos + 1;
			pos = m_Data.find_first_of('\t', index);
		}
		return ++num;
	}

	std::string DataRow::ReadString()
	{
		std::string::size_type pos = m_Data.find_first_of('\t',m_Index);
		if(pos == std::string::npos)	//找不到了，最后一个
		{	
			return m_Data.substr(m_Index);
		}
		else
		{
			std::string ret = m_Data.substr(m_Index,pos - m_Index);
			m_Index = pos + 1;
			return ret;
		}
	}
}

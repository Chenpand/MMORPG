#include "CLRecordCallback.h"
#include "CLErrorCode.h"

CLRecordCallback::CLRecordCallback()
{
	m_Index = 0;
	m_Result = ErrorCode::SUCCESS;
	m_RequestTime = 0;
}

CLRecordCallback::~CLRecordCallback()
{
	for(std::vector<std::pair<DBKey_t, CLDatabaseData* > >::iterator
		iter = m_Rows.begin(); iter != m_Rows.end(); ++iter)
	{
		CL_SAFE_DELETE_ARRAY(iter->second);
	}
}

void CLRecordCallback::Execute()
{
	if(m_Result == ErrorCode::SUCCESS){
		OnFinished();
	}
	else{
		OnFailed(m_Result);
	}
}

bool CLRecordCallback::NextRow()
{
	if(m_Index + 1 >= (int)m_Rows.size())
		return false;
	++m_Index;
	return true;
}

DBKey_t CLRecordCallback::GetKey() const
{
	if(m_Index < 0 || m_Index >= (int)m_Rows.size()) return 0;

	return m_Rows[m_Index].first;
}

const CLDatabaseData& CLRecordCallback::GetData(const std::string& name) const
{
	static CLDatabaseData defaultData;
	if(m_Index < 0 || m_Index >= (int)m_Rows.size()) return defaultData;

	ColumnMap::const_iterator iter = m_Columns.find(name);
	if(iter != m_Columns.end())
	{
		return m_Rows[m_Index].second[iter->second];
	}
	return defaultData;
}

bool CLRecordCallback::DecodeColumns(Avalon::NetInputStream& stream)
{
	m_Columns.clear();
	m_Types.clear();
	m_Rows.clear();
	m_Index = -1;

	try
	{
		UInt16 colnum = 0;
		stream & colnum;

		size_t index = 0;
		while(colnum-- > 0)
		{
			std::string colname;
			UInt8 type = 0;

			stream & colname & type;
			m_Columns.insert(std::make_pair(colname,index++));
			m_Types.push_back(type);
		}
	}
	catch(const Avalon::NetException& e)
	{
		ErrorLogStream << e.what() << LogStream::eos;
		return false;
	}

	return true;
}

bool CLRecordCallback::DecodeRow(Avalon::NetInputStream& stream)
{
	if (m_Types.empty())
	{
		ErrorLogStream << "m_Types is empty." << LogStream::eos;
		return false;
	}

	DBKey_t key = 0;
	int index = 0;
	try
	{
		stream & key;

		m_Rows.resize(m_Rows.size() + 1);
		
		std::vector<std::pair<DBKey_t, CLDatabaseData* > >::iterator rowIter = --m_Rows.end();
		rowIter->first = key;
		rowIter->second = new CLDatabaseData[m_Types.size()];

		index = 0;
		for(TypeVec::iterator iter = m_Types.begin();iter != m_Types.end();++iter)
		{
			rowIter->second[index++].InputValue(*iter,stream);
		}
	}
	catch(const Avalon::NetException& e)
	{
		ErrorLogStream << e.what() << LogStream::eos;
		ErrorLogStream << "rowSize(" << (UInt32)m_Rows.size() << ") error index(" << index << ")." << LogStream::eos;
		return false;
	}

	return true;
}


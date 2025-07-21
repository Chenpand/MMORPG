#include "CLRecordCommand.h"
#include <AvalonLogger.h>
#include <AvalonStringUtil.h>

bool CLRecordCommand::SqlDefendInfusion(std::string& str)
{
	std::string keyWords = "`|'|and|exec|insert|select|delete|update|count|*|%|chr|mid|master|truncate|char|declare|;|or|-|+|,";
	std::vector<std::string> keyWordArray;
	Avalon::StringUtil::Split(keyWords, keyWordArray, "|");
	
	for (size_t i = 0; i < keyWordArray.size(); i++)
	{
		std::string::size_type idx = str.find(keyWordArray[i]);

		// Уќжа
		if (idx != std::string::npos)
		{
			return false;
		}
	}

	return true;
}


bool CLRecordCommand::Encode(Avalon::NetOutputStream& stream)
{
	try
	{
		stream & tablename;
	}
	catch(const Avalon::NetException& )
	{
		ErrorLogStream << "CLRecordCommand::Encode NetException!" << LogStream::eos;
		return false;
	}

	return true;
}

bool CLSaveCommand::Encode(Avalon::NetOutputStream& stream)
{
	if(!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & key & UInt16(datas.size());
		for(DataMap::iterator iter = datas.begin();	
			iter != datas.end(); ++iter)
		{
			stream & iter->first & iter->second;
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "CLSaveCommand::Encode NetException!" << LogStream::eos;
		return false;
	}

	return true;
}

bool CLUpdateCommand::Encode(Avalon::NetOutputStream& stream)
{
	if(!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & key & UInt8(bflush ? 1 : 0) & UInt16(datas.size());
		for(DataMap::iterator iter = datas.begin();iter != datas.end();++iter)
		{
			stream & iter->first & iter->second;
		}

		if(key == 0) stream & cond;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "CLUpdateCommand::Encode NetException!" << LogStream::eos;
		return false;
	}

	return true;
}

bool CLDeleteCommand::Encode(Avalon::NetOutputStream& stream)
{
	if(!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & key;
		if(key == 0) stream & cond;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "CLDeleteCommand::Encode NetException!" << LogStream::eos;
		return false;
	}
	return true;
}

bool CLSelectCommand::Encode(Avalon::NetOutputStream& stream)
{
	if(!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & UInt16(columns.size());
		for(ColumnVec::iterator iter = columns.begin();iter != columns.end();++iter)
		{
			stream & (*iter);
		}

		stream & UInt16(conds.size());
		for(CondMap::iterator iter = conds.begin();iter != conds.end();++iter)
		{
			stream & iter->condname & iter->data & iter->op;
		}

		stream & order & limit;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "CLSelectCommand::Encode NetException!" << LogStream::eos;
		return false;
	}
	return true;
}


bool CLChangeNameCommand::Encode(Avalon::NetOutputStream& stream)
{
	if(!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & key & name & isInsert;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "CLChangeNameCommand::Encode NetException!" << LogStream::eos;
		return false;
	}
	return true;
}

bool CLAutoGuidInsertCommand::Encode(Avalon::NetOutputStream& stream)
{
	if (!CLRecordCommand::Encode(stream)) return false;

	try
	{
		stream & UInt16(datas.size());
		for (DataMap::iterator iter = datas.begin();
			iter != datas.end(); ++iter)
		{
			stream & iter->first & iter->second;
		}
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "CLSaveCommand::Encode NetException!" << LogStream::eos;
		return false;
	}

	return true;
}


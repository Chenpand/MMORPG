#include "CLDatabaseDefine.h"

void CLDatabaseData::Input(Avalon::NetInputStream &stream)
{
	if(IsBlock()) CL_SAFE_DELETE_ARRAY(m_Blob);
	m_Val = 0;

	stream & m_Type;
	InputValue(m_Type,stream);
}

#define CL_CASE_SERIAL_BASE(Type) \
	case DBTypeTraits<Type>::ID: stream & m_##Type; break;

#define CL_CASE_SERIAL_ALLBASE() \
	CL_CASE_SERIAL_BASE(Int8) \
	CL_CASE_SERIAL_BASE(UInt8) \
	CL_CASE_SERIAL_BASE(Int16) \
	CL_CASE_SERIAL_BASE(UInt16) \
	CL_CASE_SERIAL_BASE(Int32) \
	CL_CASE_SERIAL_BASE(UInt32) \
	CL_CASE_SERIAL_BASE(Int64) \
	CL_CASE_SERIAL_BASE(UInt64) \
	CL_CASE_SERIAL_BASE(Real32) \
	CL_CASE_SERIAL_BASE(Real64)

void CLDatabaseData::InputValue(Avalon::UInt8 type, Avalon::NetInputStream &stream)
{
	if(IsBlock()) CL_SAFE_DELETE_ARRAY(m_Blob);
	m_Val = 0;
	m_Type = type;

	switch(type)
	{
	CL_CASE_SERIAL_ALLBASE()
	case DBT_STRING:
	case DBT_BINARY:
		UInt16 len = 0;
		stream & len;
		m_Blob = new char[len + sizeof(UInt16)];
		*(UInt16*)m_Blob = len;
		stream.SeriaArray(m_Blob + sizeof(UInt16), len);
	}
}

void CLDatabaseData::Output(Avalon::NetOutputStream &stream)
{
	stream & m_Type;
	OutputValue(stream);
}

void CLDatabaseData::OutputValue( Avalon::NetOutputStream &stream)
{
	switch(m_Type)
	{
	CL_CASE_SERIAL_ALLBASE()
	case DBT_STRING:
	case DBT_BINARY:
		stream & *(UInt16*)m_Blob;
		stream.SeriaArray(m_Blob + sizeof(UInt16), *(UInt16*)m_Blob);
	}
}

#define CL_CASE_SERIAL_TO_TYPE(Type) \
	case DBTypeTraits<Type>::ID: \
		{ Type val = *this; \
		stream & val; }\
		break;

void CLDatabaseData::OutputValue(UInt8 dataType, Avalon::NetOutputStream& stream)
{
	if(dataType == m_Type)
	{
		OutputValue(stream);
		return;
	}

	switch(dataType)
	{
	CL_CASE_SERIAL_TO_TYPE(Int8)
	CL_CASE_SERIAL_TO_TYPE(UInt8)
	CL_CASE_SERIAL_TO_TYPE(Int16)
	CL_CASE_SERIAL_TO_TYPE(UInt16)
	CL_CASE_SERIAL_TO_TYPE(Int32)
	CL_CASE_SERIAL_TO_TYPE(UInt32)
	CL_CASE_SERIAL_TO_TYPE(Int64)
	CL_CASE_SERIAL_TO_TYPE(UInt64)
	CL_CASE_SERIAL_TO_TYPE(Real32)
	CL_CASE_SERIAL_TO_TYPE(Real64)
	case DBT_STRING:
	case DBT_BINARY:
		stream & *(UInt16*)m_Blob;
		stream.SeriaArray(m_Blob + sizeof(UInt16), *(UInt16*)m_Blob);
		break;
	}
}


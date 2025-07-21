#ifndef _AVALON_DBFIELD_H_
#define _AVALON_DBFIELD_H_

#include "AvalonDefine.h"
#include "AvalonStringUtil.h"

namespace Avalon
{
	/**
	 *@brief ���ݿ��ֶ�����
	 */
	class DBField
	{
	public:
		enum FiledType
		{
			INTEGER,	//����
			FLOAT,		//����
			STRING,		//�ַ���
			BINARY,		//������
		};

	public:
		DBField()
		{
			m_Type = BINARY;
			m_Buffer = NULL;
			m_Length = 0;
		}
		DBField(FiledType type, char* buffer, size_t length)
			:m_Type(type), m_Buffer(buffer), m_Length(length)
		{
		}
		~DBField(){}

	public:
#define AVALON_DEFINE_FILED_TOINTEGER(Type) \
		Type Get##Type() const \
		{ \
			if(m_Type != INTEGER || 0 == m_Length) return 0; \
			return Avalon::StringUtil::ConvertToValue<Type>(m_Buffer); \
		}

		AVALON_DEFINE_FILED_TOINTEGER(Int8)
		AVALON_DEFINE_FILED_TOINTEGER(UInt8)
		AVALON_DEFINE_FILED_TOINTEGER(Int16)
		AVALON_DEFINE_FILED_TOINTEGER(UInt16)
		AVALON_DEFINE_FILED_TOINTEGER(Int32)
		AVALON_DEFINE_FILED_TOINTEGER(UInt32)
		AVALON_DEFINE_FILED_TOINTEGER(Int64)
		AVALON_DEFINE_FILED_TOINTEGER(UInt64)

#define AVALON_DEFINE_FILED_TOFLOAT(Type) \
		Type Get##Type() const \
		{ \
			if((m_Type != FLOAT && m_Type != INTEGER) || 0 == m_Length) return 0; \
			return Avalon::StringUtil::ConvertToValue<Type>(m_Buffer); \
		}

		AVALON_DEFINE_FILED_TOFLOAT(Real32)
		AVALON_DEFINE_FILED_TOFLOAT(Real64)

		std::string GetString() const
		{
			if(m_Type == BINARY || 0 == m_Length) return "";
			return m_Buffer;
		}

		size_t GetData(char* buf,size_t buflen) const
		{
			if(0 == m_Length) return 0;
			size_t len = buflen < m_Length ? buflen : m_Length;
			memmove(buf,m_Buffer,len);
			return len;
		}

	private:
		//��������
		FiledType		m_Type;
		//����
		char*			m_Buffer;
		//����
		size_t			m_Length;
	};
}

#endif

 

#ifndef _COMMON_CODEENGINE_H_
#define _COMMON_CODEENGINE_H_


#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "udplog_typedef.h"

class CEndian
{
public:
	CEndian()
	{
		union
		{
			UInt16	s1;
			UInt8		c2[2];
		} un;
		un.s1 = 0x0102;
		m_bBigEndian = (un.c2[0] == 0x01);
	}

	inline bool IsBigEndian()
	{
		return m_bBigEndian;
	}

	bool			m_bBigEndian;
};


class CCodeEngine
{
public:
	CCodeEngine();
	virtual ~CCodeEngine();

public:
 
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const bool data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int8 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int16 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int32 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int64 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt8 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt16 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt32 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt64 data);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt8 *data, const UInt32 length, const UInt32 maxLength);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const char *data, const UInt16 maxLength);
	static Int32 Encode(UInt8* buf, const UInt32 size, UInt32& offset, const double data);

public:
 
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, bool& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int8& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int16& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int32& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int64& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt8& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt16& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt32& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt64& data);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt8* data, const UInt32 length, const UInt32 maxLength);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, char* data, const UInt16 maxLength);
	static Int32 Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, double& data);

protected:
	static CEndian	ms_endian;
};

#endif


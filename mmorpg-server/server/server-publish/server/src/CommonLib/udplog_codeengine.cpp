
#include "udplog_codeengine.h"

#define ENCODE(buf, size, offset, data, type)\
	if ((NULL == buf) || (0 == size))\
	{\
		return -1;\
	}\
	if (offset + sizeof(type) > size)\
	{\
		return -1;\
	}\
	UInt8 *p = (UInt8*)&data;\
	if (ms_endian.IsBigEndian())\
	{\
		for(UInt32 i = 0; i < sizeof(type); ++i)\
		{\
			buf[offset++] = p[i];\
		}\
	}\
	else\
	{\
		for(UInt32 i = 0; i < sizeof(type); ++i)\
		{\
			buf[offset++] = p[sizeof(type)-i-1];\
		}\
	}\
	return UL_OK;


#define DECODE(buf, size, offset, data, type)\
	if ((NULL == buf) || (0 == size))\
	{\
		return -1;\
	}\
	if (offset + sizeof(type) > size)\
	{\
		return -1;\
	}\
	UInt8 *p = (UInt8*)&data;\
	if (ms_endian.IsBigEndian())\
	{\
		for(UInt32 i = 0; i < sizeof(type); ++i)\
		{\
			 p[i] = buf[offset++];\
		}\
	} \
	else\
	{\
		for (UInt32 i = 0; i < sizeof(type); ++i)\
		{\
			p[sizeof(type)-i-1] = buf[offset++];\
		}\
	}\
	return UL_OK;


CEndian	CCodeEngine::ms_endian;

CCodeEngine::CCodeEngine()
{

}

CCodeEngine::~CCodeEngine()
{

}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const bool data)
{
	UInt8 v = (data ? 1 : 0);
	return Encode(buf, size, offset, v);
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int8 data)
{
	ENCODE(buf, size, offset, data, Int8)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int16 data)
{
	ENCODE(buf, size, offset, data, Int16)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int32 data)
{
	ENCODE(buf, size, offset, data, Int32)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const Int64 data)
{
	ENCODE(buf, size, offset, data, Int64)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt8 data)
{
	ENCODE(buf, size, offset, data, UInt8)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt16 data)
{
	ENCODE(buf, size, offset, data, UInt16)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt32 data)
{
	ENCODE(buf, size, offset, data, UInt32)
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt64 data)
{
	ENCODE(buf, size, offset, data, UInt64)
}
Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const double data)
{
	ENCODE(buf, size, offset, data, double)
}
Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const UInt8 *data, const UInt32 length, const UInt32 maxLength)
{
	if ((NULL == buf) || (0 == size))
	{
		return -1;
	}
	if ((NULL == data) || (0 == length))
	{
		return UL_OK;
	}
	if (offset + length > size)
	{
		return -1;
	}
	if (length > maxLength)
	{
		return -1;
	}

	memcpy(buf + offset, data, length);
	offset += length;

	return UL_OK;
}

Int32 CCodeEngine::Encode(UInt8* buf, const UInt32 size, UInt32& offset, const char *data, const UInt16 maxLength)
{
	if ((NULL == buf) || (0 == size))
	{
		return -1;
	}
	if (NULL == data)
	{
		return -1;
	}
	if (0 == maxLength)
	{
		return -1;
	}

	Int32 ret = UL_OK;

	UInt16 len = (UInt16)strlen(data) + 1;
	ret = Encode(buf, size, offset, len);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, (UInt8*)data, len, (UInt32)maxLength);
	if (0 > ret)
	{
		return ret;
	}

	return UL_OK;
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, bool& data)
{
	Int32 ret = UL_OK;

	UInt8 v = 0;
	ret = Decode(buf, size, offset, v);
	if (0 > ret)
	{
		return ret;
	}

	data = (0 != v);

	return UL_OK;
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int8& data)
{
	DECODE(buf, size, offset, data, Int8)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int16& data)
{
	DECODE(buf, size, offset, data, Int16)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int32& data)
{
	DECODE(buf, size, offset, data, Int32)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, Int64& data)
{
	DECODE(buf, size, offset, data, Int64)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt8& data)
{
	DECODE(buf, size, offset, data, UInt8)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt16& data)
{
	DECODE(buf, size, offset, data, UInt16)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt32& data)
{
	DECODE(buf, size, offset, data, UInt32)
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt64& data)
{
	DECODE(buf, size, offset, data, UInt64)
}
Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, double& data)
{
	DECODE(buf, size, offset, data, double)
}
Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, UInt8* data, const UInt32 length, const UInt32 maxLength)
{
	if ((NULL == buf) || (0 == size))
	{
		return -1;
	}
	if ((NULL == data) || (0 == length))
	{
		return UL_OK;
	}
	if (offset + length > size)
	{
		return -1;
	}
	if (length > maxLength)
	{
		return -1;
	}

	memcpy(data, buf + offset, length);
	offset += length;

	return UL_OK;
}

Int32 CCodeEngine::Decode(const UInt8 *buf, const UInt32 size, UInt32& offset, char* data, const UInt16 maxLength)
{
	if (0 == maxLength)
	{
		return -1;
	}

	Int32 ret = UL_OK;

	UInt16 len = 0;
	ret = Decode(buf, size, offset, len);
	if (0 > ret)
	{
		return ret;
	}
	if (0 == len)
	{
		data[0] = '\0';
		return UL_OK;
	}
	ret = Decode(buf, size, offset, (UInt8*)data, (UInt32)len, (UInt32)maxLength);
	if (0 > ret)
	{
		return ret;
	}
	data[len - 1] = '\0';

	return UL_OK;
}



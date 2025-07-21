#include "udplog_lib.h"
#include "udplog_codeengine.h"

CCSNotifyWriteLog::CCSNotifyWriteLog()
{
	m_nUid			= 0;
	m_nTableType	= 0;
	m_szLog[0]		= '\0';
}

CCSNotifyWriteLog::~CCSNotifyWriteLog()
{
}

Int32 CCSNotifyWriteLog::Encode(UInt8 *buf, UInt32 size, UInt32 &offset)
{
	Int32 ret = UL_OK;
	ret = CCodeEngine::Encode(buf, size, offset, m_nUid);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nTableType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_szLog, sizeof(m_szLog));
	if(ret < 0)
	{
		return ret;
	}

	return UL_OK;
}
Int32 CCSNotifyWriteLog::Decode(const UInt8 *buf, const UInt32 size, UInt32 &offset)
{
	Int32 ret = UL_OK;
	ret = CCodeEngine::Decode(buf, size, offset, m_nUid);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nTableType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_szLog, sizeof(m_szLog));
	if(ret < 0)
	{
		return ret;
	}
	return UL_OK;
}

Int32 CCSNotifyWriteLog::Dump(char *buf, const UInt32 size, UInt32 &offset)
{
	return UL_OK;
}

CCSHead::CCSHead()
{
	m_nHttpHead			= 0;
	m_nPackageLength	= 0;
	m_nUid				= 0;
	m_nFlag				= 0;
	m_nOptionLength		= 0;
	m_szOption[0]		= '\0';
	m_nHeaderLength		= 0;
	m_nMessageID		= 0x10;
	m_nMessageType		= 0x03;
	m_nVersionType		= 0x03;
	m_nVersion			= 0;
	m_nPlayerID			= -1;
	m_nSequence			= 0;
}

CCSHead::~CCSHead()
{
}
Int32 CCSHead::Encode(UInt8 *buf, UInt32 size, UInt32 &offset)
{
	Int32 ret = UL_OK;
	ret = CCodeEngine::Encode(buf, size, offset, m_nHttpHead);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nPackageLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nUid);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nFlag);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nOptionLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_szOption, m_nOptionLength, sizeof(m_szOption));
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nHeaderLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nMessageID);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nMessageType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nVersionType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nVersion);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nPlayerID);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, m_nSequence);
	if(ret < 0)
	{
		return ret;
	}
  
	return UL_OK;
}
Int32 CCSHead::Decode(const UInt8 *buf, const UInt32 size, UInt32 &offset)
{
	Int32 ret = UL_OK;
	ret = CCodeEngine::Decode(buf, size, offset, m_nHttpHead);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nPackageLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nUid);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nFlag);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nOptionLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_szOption, m_nOptionLength, sizeof(m_szOption));
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nHeaderLength);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nMessageID);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nMessageType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nVersionType);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nVersion);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nPlayerID);
	if(ret < 0)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, m_nSequence);
	if(ret < 0)
	{
		return ret;
	}

	return UL_OK;
}

Int32 CCSHead::Dump(char *buf, const UInt32 size, UInt32 &offset)
{
	return UL_OK;
}

#ifndef _UDPLOG_LIB_H_
#define _UDPLOG_LIB_H_


#include "udplog_codeengine.h"

class IMessageBody
{
public:
	virtual ~IMessageBody() {}
	virtual Int32 Encode(UInt8* buf, UInt32 size, UInt32& offset) = 0;
	virtual Int32 Decode(const UInt8* buf, const UInt32 size, UInt32& offset) = 0;
	virtual Int32 Dump(char* buf, const UInt32 size, UInt32& offset) = 0;
};

class CCSNotifyWriteLog :public IMessageBody
{
public:
	Int32 m_nUid;
	Int16 m_nTableType;
	char	m_szLog[MAX_LOG_LENGTH];
public:
	CCSNotifyWriteLog();
	virtual ~CCSNotifyWriteLog();

	Int32 Encode(UInt8* buf, UInt32 size, UInt32& offset);
	Int32 Decode(const UInt8* buf, const UInt32 size, UInt32& offset);
	Int32 Dump(char* buf, const UInt32 size, UInt32& offset);
};


class CCSHead :public IMessageBody
{
public:
	UInt8 m_nHttpHead;
	Int32 m_nPackageLength;
	Int32 m_nUid;
	Int16 m_nFlag;
	Int16 m_nOptionLength;
	UInt8	m_szOption[MAX_OPTION_LENGTH];
	Int16 m_nHeaderLength;
	Int16 m_nMessageID;
	Int16 m_nMessageType;
	Int16 m_nVersionType;
	Int16 m_nVersion;
	Int32 m_nPlayerID;
	Int32 m_nSequence;

public:
	CCSHead();
	virtual ~CCSHead();

	Int32 Encode(UInt8* buf, UInt32 size, UInt32& offset);
	Int32 Decode(const UInt8* buf, const UInt32 size, UInt32& offset);
	Int32 Dump(char* buf, const UInt32 size, UInt32& offset);
};

#endif

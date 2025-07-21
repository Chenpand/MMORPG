#ifndef _CL_RED_PACKET_DATAENTRY_H_
#define _CL_RED_PACKET_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum RedPacketSubType
{
	// �Է�
	RPST_BUY = 1,
	// ϵͳ
	RPST_SYSTEM = 2,
};

/**
*@brief �����
*/
struct RedPacketDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)reason; }

	bool Read(Avalon::DataRow& row);

	bool IsValidNum(UInt8 num);

public:
	// ��Դ
	UInt16	reason;
	// ����
	std::string desc;
	// ����(RedPacketType)
	UInt8	type;
	// ������
	RedPacketSubType subType;
	// �ܽ��
	UInt32	totalMoney;
	// ��ѡ����
	std::vector<UInt8>	nums;
	// ���������С���
	UInt32	minMoney;
	// ������������
	UInt32	maxMoney;
	// ���͵Ļ���ID
	UInt32 costMoneyId;
	// �յ��Ļ���ID
	UInt32 getMoneyId;
	// ��������
	UInt8	thrType;
};

typedef Avalon::DataEntryMgr<RedPacketDataEntry> RedPacketDataEntryMgr;

#endif

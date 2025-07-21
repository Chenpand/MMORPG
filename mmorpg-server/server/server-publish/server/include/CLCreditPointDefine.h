#ifndef _CL_CREDIT_POINT_DEFINE_H_
#define _CL_CREDIT_POINT_DEFINE_H_

#include <CLDefine.h>
#include <CLObject.h>
#include <CLGoldConsignmentValueDataEntry.h>

/**
 *@brief ���õ�ȯ����
 */
struct CreditPointOrder
{
	CreditPointOrder() : orderId(0), getNum(0), getTime(0) {}
	CreditPointOrder(UInt64 oi, UInt32 gn, UInt32 gt) : orderId(oi), getNum(gn), getTime(gt) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & getNum & getTime;
	}

	// ����id
	UInt64 orderId;
	// �������
	UInt32 getNum;
	// ���ʱ��
	UInt32 getTime;
};

/**
 *@brief ���õ�ȯ��¼
 */
class CreditPointRecordEntry : public CLObject
{
public:
	CreditPointRecordEntry()
	{
		getTime = 0;
		totalNum = 0;
		todayTransfer = 0;
	}
	~CreditPointRecordEntry(){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & getTime & totalNum & todayTransfer & orderVec;
	}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " getTime:" << getTime;
		ss << " totalNum:" << totalNum;
		ss << " todayTransfer:" << todayTransfer;
		return ss.str();
	}

	void EncodeOrderVec(std::string& str)
	{
		for (auto& it : orderVec)
		{
			str.append(std::to_string(it.orderId)).append(",")
				.append(std::to_string(it.getNum)).append(",")
				.append(std::to_string(it.getTime)).append("|");
		}

		if ( !str.empty())
		{
			str.pop_back();
		}
	}

	// �������
	UInt32 getTime;
	// ����
	UInt32 totalNum;
	// ����ת��
	UInt32 todayTransfer;
	// ��ȡ��¼
	std::vector<CreditPointOrder> orderVec;
};

inline UInt32 GetTransferDate()
{
	UInt32 transferDate = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_TRANSFER_POINT_DATA);
	if (transferDate == 0)
		transferDate = 1;

	return (transferDate - 1) * DAY_TO_SEC;
}

#endif // !_CL_CREDIT_POINT_DEFINE_H_

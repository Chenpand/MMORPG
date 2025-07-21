#ifndef _CL_ITEM_TRANSACTION_H_
#define _CL_ITEM_TRANSACTION_H_

#include <CLDefine.h>

namespace CLProtocol {
	struct TransItemData;
}

/**
 *@brief �������
 */
class CLItemTransaction
{
public:
	CLItemTransaction() : m_Status(0), m_Serial(0), m_ErrorCode(0), m_Point(0), m_BindPoint(0)
		, m_Gold(0), m_BindGold(0) {}
	virtual ~CLItemTransaction() {}

public:

	enum Status
	{
		STATUS_INIT = 0,
		STATUS_CHECK = 1,
		STATUS_CHECKED = 2,
		STATUS_FINAL = 3,
	};

	UInt32 GetSerial() const { return m_Serial; }

	void SetStatus(UInt32 status) { m_Status = status; }
	UInt32 GetStatus() const { return m_Status; }

	void SetErrorCode(UInt32 code) { m_ErrorCode = code; }
	UInt32 GetErrorCode() const { return m_ErrorCode; }

	void SetPoint(UInt32 value) { m_Point = value; }
	UInt32 GetPoint() const { return m_Point; }

	void SetBindPoint(UInt32 value) { m_BindPoint = value; }
	UInt32 GetBindPoint() const { return m_BindPoint; }

	void SetGold(UInt32 value) { m_Gold = value; }
	UInt32 GetGold() const { return m_Gold; }

	void SetBindGold(UInt32 value) { m_BindGold = value; }
	UInt32 GetBindGold() const { return m_BindGold; }

protected:

	// ����״̬
	UInt32		m_Status;
	// �������
	UInt32		m_Serial;
	// ���񷵻ش�����
	UInt32		m_ErrorCode;
	// ��ȯ
	UInt32		m_Point;
	// ���
	UInt32		m_BindPoint;
	// ���
	UInt32		m_Gold;
	// ���
	UInt32		m_BindGold;
};

#endif // !_CL_ITEM_TRANSACTION_H_

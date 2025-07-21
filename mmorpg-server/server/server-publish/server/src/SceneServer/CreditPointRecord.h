#ifndef _CREDIT_POINT_H_
#define _CREDIT_POINT_H_

#include <CLCreditPointDefine.h>

class Player;
/**
 *@brief ���õ�ȯ��¼����
 */
class CreditPointRecord
{
public:
	CreditPointRecord();
	~CreditPointRecord();

	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 id) { m_AccId = id; }

	void TransferToPoint(Player* player, UInt32 transferTime);

	/**
	 *@brief �������õ�ȯ��¼
	 */
	void CreditPointRecordReq(Player* player);

	/**
	 *@brief ������õ�ȯ
	 */
	void ObtainCreditPoint(UInt32 obtainNum, UInt64 orderId);

	/**
	 *@brief ��������
	 */
	void LoadAddRecord(CreditPointRecordEntry& record);

private:

	// �˺�id
	UInt32 m_AccId;

	// ��key->������ڿ�ʼʱ�����
	std::map<UInt32, CreditPointRecordEntry>  m_CreditPointMap;
};

#endif // !_CREDIT_POINT_MGR_H_


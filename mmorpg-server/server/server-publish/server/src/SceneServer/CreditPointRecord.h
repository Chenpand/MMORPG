#ifndef _CREDIT_POINT_H_
#define _CREDIT_POINT_H_

#include <CLCreditPointDefine.h>

class Player;
/**
 *@brief 信用点券记录管理
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
	 *@brief 请求信用点券记录
	 */
	void CreditPointRecordReq(Player* player);

	/**
	 *@brief 获得信用点券
	 */
	void ObtainCreditPoint(UInt32 obtainNum, UInt64 orderId);

	/**
	 *@brief 加载数据
	 */
	void LoadAddRecord(CreditPointRecordEntry& record);

private:

	// 账号id
	UInt32 m_AccId;

	// （key->获得日期开始时间戳）
	std::map<UInt32, CreditPointRecordEntry>  m_CreditPointMap;
};

#endif // !_CREDIT_POINT_MGR_H_


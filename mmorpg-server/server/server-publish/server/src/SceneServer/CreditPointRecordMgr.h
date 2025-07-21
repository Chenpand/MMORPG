#ifndef _CREDIT_POINT_RECORD_MGR_H_
#define _CREDIT_POINT_RECORD_MGR_H_

#include <CLCreditPointDefine.h>
#include "CreditPointRecord.h"

class CLRecordCallback;
/**
 *@brief 信用点券记录管理
 */
class CreditPointRecordMgr : public Avalon::Singleton<CreditPointRecordMgr>
{
public:
	CreditPointRecordMgr();
	~CreditPointRecordMgr();

	void Init();
	void Final();

	void OnTick(const Avalon::Time& now, Player* player);

	/**
	 *@brief 请求信用点券记录
	 */
	void HandlerCreditPointRecordReq(Player* player);

	/**
	 *@brief 获得信用点券
	 */
	void ObtainCreditPoint(UInt32 accId, UInt32 obtainNum, UInt64 orderId);

	/**
	 *@brief 查找
	 */
	CreditPointRecord* FindCreditPointRecord(UInt32 accId);

	/**
	 *@brief 添加记录
	 */
	void AddCreditPointRecord(UInt32 accId, CreditPointRecordEntry& entry);

	/**
	 *@brief GM转化记录
	 */
	void GMTransferCreditPoint(UInt32 accId);

	/**
	 *@brief 加载数据
	 */
	bool LoadData(CLRecordCallback* callback);

	/**
	 *@brief 更新数据库
	 */
	void UpdateToDB(CreditPointRecordEntry& record);

	/**
	 *@brief 插入数据库
	 */
	void InsertToDB(UInt32 accId, CreditPointRecordEntry& record);

	/**
	 *@brief 删除数据库
	 */
	void DelToDB(UInt64 guid);

private:

	// （key->账号id）
	std::map<UInt32, CreditPointRecord*>  m_AccCreditPointMap;
};


#endif // !_CREDIT_POINT_MGR_H_


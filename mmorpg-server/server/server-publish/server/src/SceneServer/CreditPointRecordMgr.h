#ifndef _CREDIT_POINT_RECORD_MGR_H_
#define _CREDIT_POINT_RECORD_MGR_H_

#include <CLCreditPointDefine.h>
#include "CreditPointRecord.h"

class CLRecordCallback;
/**
 *@brief ���õ�ȯ��¼����
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
	 *@brief �������õ�ȯ��¼
	 */
	void HandlerCreditPointRecordReq(Player* player);

	/**
	 *@brief ������õ�ȯ
	 */
	void ObtainCreditPoint(UInt32 accId, UInt32 obtainNum, UInt64 orderId);

	/**
	 *@brief ����
	 */
	CreditPointRecord* FindCreditPointRecord(UInt32 accId);

	/**
	 *@brief ��Ӽ�¼
	 */
	void AddCreditPointRecord(UInt32 accId, CreditPointRecordEntry& entry);

	/**
	 *@brief GMת����¼
	 */
	void GMTransferCreditPoint(UInt32 accId);

	/**
	 *@brief ��������
	 */
	bool LoadData(CLRecordCallback* callback);

	/**
	 *@brief �������ݿ�
	 */
	void UpdateToDB(CreditPointRecordEntry& record);

	/**
	 *@brief �������ݿ�
	 */
	void InsertToDB(UInt32 accId, CreditPointRecordEntry& record);

	/**
	 *@brief ɾ�����ݿ�
	 */
	void DelToDB(UInt64 guid);

private:

	// ��key->�˺�id��
	std::map<UInt32, CreditPointRecord*>  m_AccCreditPointMap;
};


#endif // !_CREDIT_POINT_MGR_H_


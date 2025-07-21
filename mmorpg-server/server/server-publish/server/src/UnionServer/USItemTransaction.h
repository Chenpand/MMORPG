#ifndef _US_ITEM_TRANSACTION_H_
#define _US_ITEM_TRANSACTION_H_

#include <CLItemDefine.h>
#include <CLItemTransaction.h>

class USPlayer;

/**
 *@brief ��������
 */
class USItemTransaction : public CLItemTransaction
{
	friend class USPlayer;
public:
	USItemTransaction(USPlayer* player);
	virtual ~USItemTransaction();

public:
	USPlayer* GetPlayer() const { return m_Player; }
	UInt32 GetTimeOut() { return m_TimeOut; }

	void SetServiceType(UInt32 type) { m_ServiceType = type; }
	UInt32 GetServiceType() { return m_ServiceType; }

	void SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem = 0);
	UInt32 GetItemNum(UInt32 id);

	void SetNoBindItem(UInt32 id, UInt32 num);
	UInt32 GetNoBindItemNum(UInt32 id);

	/**
	 *@brief �������
	 */
	void RemoveItem(const std::string& reason);

	/**
	 *@brief ����
	 */
	void UnLock();

	/**
	 *@brief ��ʱ
	 */
	bool CheckTimeOut(const Avalon::Time& now);

	/**
	 *@brief �����
	 */
	virtual void OnCheckSucced(USPlayer* player) = 0;
	virtual void OnCheckFailed(USPlayer* player) = 0;
	virtual void OnTimeOut(USPlayer* player) = 0;

private:
	void CheckItem();

private:

	USPlayer* m_Player;
	// ��ʱʱ��
	UInt32 m_TimeOut;
	// �����ķ���
	UInt32 m_ServiceType;
	// ����
	std::map<UInt32, CLProtocol::TransItemData>	m_Items;
	// �󶨵���
	std::map<UInt32, UInt32>	m_NoBindItems;
};

#endif

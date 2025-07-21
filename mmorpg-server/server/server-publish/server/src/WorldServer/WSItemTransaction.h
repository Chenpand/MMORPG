#ifndef _WS_ITEM_TRANSACTION_H_
#define _WS_ITEM_TRANSACTION_H_

#include <CLDefine.h>
#include <CLItemDefine.h>

class WSPlayer;
namespace CLProtocol{
	struct TransItemData;
}

/**
 *@brief ��������
 */
class ItemTransaction
{
	friend class WSPlayer;
public:
	enum Status
	{
		STATUS_INIT =	0,
		STATUS_CHECK =	1,
		STATUS_CHECKED =	2,
		STATUS_FINAL =	3,
	};
public:
	ItemTransaction(WSPlayer* player);
	virtual ~ItemTransaction();

public:
	void SetErrorCode(UInt32 code) { m_ErrorCode = code; }
	UInt32 GetErrorCode() const { return m_ErrorCode; }

	WSPlayer* GetPlayer() const{ return m_pPlayer; }

	void SetBindSilver(UInt32 bindSilver) { m_BindSilver = bindSilver; }
	UInt32 GetBindSilver() { return m_BindSilver; }
	void SetSilver(UInt32 silver){ m_Silver = silver; }
	UInt32 GetSilver() const{ return m_Silver; }

	// ��ȯ
	void SetGold(UInt32 gold){ m_Gold = gold; }
	UInt32 GetGold() const{ return m_Gold; }

	// ���
	void SetBindGold(UInt32 gold){ m_BindGold = gold; }
	UInt32 GetBindGold() const{ return m_BindGold; }

	void SetReviveCoin(UInt32 value) { m_ReviveCoin = value; }
	UInt32 GetReviveCoin() const { return m_ReviveCoin; }
	void SetGuildContri(UInt32 value) { m_GuildContri = value; }
	UInt32 GetGuildContri() const { return m_GuildContri; }
	
	void SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem = 0);
	UInt32 GetItemNum(UInt32 id);

	void SetNoBindItem(UInt32 id, UInt32 num){ m_NoBindItems[id] = num; }
	UInt32 GetNoBindItemNum(UInt32 id) const
	{
		std::map<UInt32, UInt32>::const_iterator iter = m_NoBindItems.find(id);
		if(iter != m_NoBindItems.end()) return iter->second;
		return 0;
	}
	void SetCounter(std::string name, UInt32 maxCount, UInt32 needCount = 1)
	{
		m_CounterName = name;
		m_MaxCount = maxCount;
		m_NeedCount = needCount;
	}

	void SetCounterTimes(UInt32 times) { m_CounterTimes = times; }
	UInt32 GetCounterTimes() { return m_CounterTimes; }

	void AddDetailItem(ObjID_t guid, UInt32 itemTypeId, UInt32 num)
	{
		DetailItem detailItem;
		detailItem.guid = guid;
		detailItem.itemTypeId = itemTypeId;
		detailItem.num = num;
		m_detailItems.push_back(detailItem);
	}

	void SetIsWeeklySetTask(UInt8 isSetWeeklyTask) { m_isWeeklySetTask = isSetWeeklyTask; }

public:
	/**
	 *@brief ��ȡ����
	 */
	UInt16 GetSerial() const{ return m_Serial; }

	/**
	 *@brief �������
	 */
	void RemoveItem(const std::string& reason);

	/**
	 *@brief ����
	 */
	void UnLock();

	UInt8 GetStatus() { return m_Status; }
	void SetStatus(UInt8 s) { m_Status = s; }
public:
	/**
	 *@brief �����
	 */
	virtual void OnCheckSucced(WSPlayer* player) = 0;
	virtual void OnCheckFailed(WSPlayer* player) = 0;

	/**
	*@brief ��ʱ
	*/
	virtual void OnTimeOut(WSPlayer* player) {}
private:
	void CheckItem();

private:
	UInt8		m_Status;
	UInt16		m_Serial;
	UInt32		m_ErrorCode;
	WSPlayer*	m_pPlayer;
	UInt32		m_BindSilver;
	UInt32		m_Silver;
	UInt32		m_BindGold;	// ���
	UInt32		m_Gold;		// ��ȯ
	UInt32		m_ReviveCoin;
	// ���ṱ��
	UInt32		m_GuildContri;
	std::map<UInt32, CLProtocol::TransItemData>	m_Items;
	std::map<UInt32, UInt32>	m_NoBindItems;

	/*
		��ѯ�����Ƿ��ã�m_MaxCount������������m_NeedCount������Ҫ�Ĵ���
		��Ҫmaxcount - count > needcount
	*/
	std::string	m_CounterName;
	UInt32		m_MaxCount;
	UInt32		m_NeedCount;
	// ����ǲ�ѯ�����ĵ�ǰ����
	UInt32		m_CounterTimes;
	std::vector<DetailItem> m_detailItems;
	UInt8		m_isWeeklySetTask; //�Ƿ����ܳ������Ͻ����߼��
};

#endif

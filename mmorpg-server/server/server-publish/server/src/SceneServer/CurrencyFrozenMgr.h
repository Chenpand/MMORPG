#ifndef _ITEM_FROZEN_MGR_H_
#define _ITEM_FROZEN_MGR_H_

#include <set>
#include <CLDefine.h>

struct CurrencyFrozen
{
	CurrencyFrozen() : guid(0), roleId(0), itemId(0), num(0), frozenTime(0) {}

	// guid
	UInt64 guid;
	// ��ɫid
	UInt64 roleId;
	// ��Ʒid
	UInt32 itemId;
	// ����
	UInt32 num;
	// ʱ��
	UInt64 frozenTime;
	// reason
	std::string reason;

	std::string SerializeToString()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << " roleId:" << roleId;
		ss << " itemId:" << itemId;
		ss << " num:" << num;
		ss << " frozenTime:" << frozenTime;
		ss << " reason:" << reason;

		return ss.str();
	};
};

class CLRecordCallback;

/**
*@brief ���Ҷ������
*/
class CurrencyFrozenMgr : public Avalon::Singleton<CurrencyFrozenMgr>
{
public:
	CurrencyFrozenMgr();
	~CurrencyFrozenMgr();

	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ���붳��
	*/
	UInt64 ApplyFrozen(UInt64 roleId, UInt32 itemId, UInt32 num, const std::string& reason);

	/**
	*@brief �������
	*/
	UInt32 RemoveFrozen(UInt64 guid);

	/**
	*@brief �۳�����
	*/
	UInt32 DeductFrozen(UInt64 guid);

	/**
	*@brief ��ȡ��Ҷ�����Ϣ
	*/
	void GetRoleFrozenOrder(UInt64 roleId, std::vector<CurrencyFrozen>& orderVec);

	/**
	*@brief ����
	*/
	void LoadCurrencyFrozen(CLRecordCallback* callback);

private:

	void _AddRoleOrder(UInt64 roleId, UInt64 orderId);
	void _DelRoleOrder(UInt64 roleId, UInt64 orderId);

	void _InsertFrozen(const CurrencyFrozen& frozen);
	void _DelFrozen(UInt64 guid);

private:

	// �����б�(key->����id)
	HashMap<UInt64, CurrencyFrozen> m_FrozenMap;
	// ��Ҷ���(key->��ɫid��val->�����б�)
	HashMap<UInt64, std::set<UInt64>> m_RoleOrderMap;
};

#endif

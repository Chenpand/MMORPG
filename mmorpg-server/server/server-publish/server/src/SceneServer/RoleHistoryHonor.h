#ifndef _ROLE_HISTORY_HONOR_H_
#define _ROLE_HISTORY_HONOR_H_

#include <CLDefine.h>

enum HONOR_DATE_TYPE
{
	HONOR_DATE_TYPE_TOTAL = 1,	// �ܼ�
	HONOR_DATE_TYPE_TODAY = 2,	// ����
	HONOR_DATE_TYPE_YESTERDAY= 3,	// ����
	HONOR_DATE_TYPE_THIS_WEEK = 4,	// ����
	HONOR_DATE_TYPE_LAST_WEEK = 5,	// ����

	HONOR_DATE_TYPE_MAX = 6
};

/**
*@brief ��ʷ����
*/
struct HistoryHonor
{
	HistoryHonor() : guid(0), totalHonor(0) {}

	void clear() 
	{
		totalHonor = 0;
		pvpCntMap.clear();
	}

	//guid
	UInt64 guid;
	// �����ܼ�
	UInt32 totalHonor;
	// pvp����map(key->pvpType, val->����)
	std::map<UInt32, UInt32> pvpCntMap;
};

class Player;
class CLRecordCallback;

/**
*@brief ��ɫ��ʷ����
*/
class RoleHistoryHonor
{
	typedef std::map<UInt32, HistoryHonor> HistoryHonorMap;

public:
	RoleHistoryHonor();
	~RoleHistoryHonor();

public:

	Player* GetOwner() { return m_player; }
	void SetOwner(Player* player) { m_player = player; }

	/**
	 *@brief ��������
	 */
	void LoadData(CLRecordCallback* pCallback);

	/**
	 *@brief ����
	 */
	void OnOnline(bool isDayChange);

	/**
	 *@brief ����������Ϣ
	 */
	void HandlerHonor();

	/**
	 *@brief ����
	 */
	void OnLogicDayChanged();

	/**
	 *@brief ����
	 */
	void OnLogicWeekChanged(UInt32 changeWeekNum);

	/**
	 *@brief pvp����
	 */
	void OnPvP(UInt32 pvpType, UInt32 obtainHonor);

	/**
	 *@brief ��������
	 */
	void AddHonor(UInt32 honor);

	/**
	 *@brief ����С���
	 */
	void HonorRedPoint();

private:

	/**
	 *@brief ��ȡ��ʷ����
	 */
	HistoryHonor* _GetHistoryHonor(UInt32 dateType);

	/**
	 *@brief pvp����
	 */
	bool _CheckPvpType(UInt32 pvpType);

	/**
	 *@brief ���������͸�������
	 */
	void _UpdatePkCntAndHonor(UInt32 dateType, UInt32 pvpType, UInt32 obtainHonor);

	/**
	 *@brief ��������
	 */
	void _UpdateDB(UInt32 date, bool isInsert);

	/**
	 *@brief ��������
	 */
	void _DelDB(UInt64 guid);

	/**
	 *@brief ������������
	 */
	void _UpdateDateType(UInt64 guid, UInt32 date);

	/**
	 *@brief ��������
	 */
	void _AddHonor(UInt32 dateType, UInt32 honor);

	/**
	 *@brief ����pvp����
	 */
	void _EncodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, const std::string& pvpStr);
	void _DecodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, std::string& str);

private:

	Player* m_player;

	// ��ʷ����(key->��������)
	std::map<UInt32, HistoryHonor> m_HistoryMap;
};
	
#endif

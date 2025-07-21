#ifndef _ROLE_HISTORY_HONOR_H_
#define _ROLE_HISTORY_HONOR_H_

#include <CLDefine.h>

enum HONOR_DATE_TYPE
{
	HONOR_DATE_TYPE_TOTAL = 1,	// 总计
	HONOR_DATE_TYPE_TODAY = 2,	// 今天
	HONOR_DATE_TYPE_YESTERDAY= 3,	// 昨天
	HONOR_DATE_TYPE_THIS_WEEK = 4,	// 本周
	HONOR_DATE_TYPE_LAST_WEEK = 5,	// 上周

	HONOR_DATE_TYPE_MAX = 6
};

/**
*@brief 历史荣誉
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
	// 荣誉总计
	UInt32 totalHonor;
	// pvp次数map(key->pvpType, val->次数)
	std::map<UInt32, UInt32> pvpCntMap;
};

class Player;
class CLRecordCallback;

/**
*@brief 角色历史荣誉
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
	 *@brief 加载数据
	 */
	void LoadData(CLRecordCallback* pCallback);

	/**
	 *@brief 上线
	 */
	void OnOnline(bool isDayChange);

	/**
	 *@brief 请求荣誉信息
	 */
	void HandlerHonor();

	/**
	 *@brief 跨天
	 */
	void OnLogicDayChanged();

	/**
	 *@brief 跨周
	 */
	void OnLogicWeekChanged(UInt32 changeWeekNum);

	/**
	 *@brief pvp结算
	 */
	void OnPvP(UInt32 pvpType, UInt32 obtainHonor);

	/**
	 *@brief 增加荣誉
	 */
	void AddHonor(UInt32 honor);

	/**
	 *@brief 荣誉小红点
	 */
	void HonorRedPoint();

private:

	/**
	 *@brief 获取历史荣誉
	 */
	HistoryHonor* _GetHistoryHonor(UInt32 dateType);

	/**
	 *@brief pvp结算
	 */
	bool _CheckPvpType(UInt32 pvpType);

	/**
	 *@brief 按日期类型更新数据
	 */
	void _UpdatePkCntAndHonor(UInt32 dateType, UInt32 pvpType, UInt32 obtainHonor);

	/**
	 *@brief 更新数据
	 */
	void _UpdateDB(UInt32 date, bool isInsert);

	/**
	 *@brief 更新数据
	 */
	void _DelDB(UInt64 guid);

	/**
	 *@brief 更新日期类型
	 */
	void _UpdateDateType(UInt64 guid, UInt32 date);

	/**
	 *@brief 增加荣誉
	 */
	void _AddHonor(UInt32 dateType, UInt32 honor);

	/**
	 *@brief 编码pvp数据
	 */
	void _EncodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, const std::string& pvpStr);
	void _DecodePvpInfoStr(std::map<UInt32, UInt32>& pvpMap, std::string& str);

private:

	Player* m_player;

	// 历史荣誉(key->日期类型)
	std::map<UInt32, HistoryHonor> m_HistoryMap;
};
	
#endif

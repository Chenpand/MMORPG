#ifndef _WHOLE_BARGAIN_MGR_H_
#define _WHOLE_BARGAIN_MGR_H_

#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class GASPlayer;
class CLRecordCallback;

/**
*@brief 全民砍价活动
*/
class WholeBargainMgr : public Avalon::Singleton<WholeBargainMgr>
{
public:
	WholeBargainMgr();
	~WholeBargainMgr();

	void OnInit();

	/**
	*@brief 加载数据
	*/
	bool LoadWholeBargain(CLRecordCallback* callback);
	bool LoadWholeBargainPlayerJoin(CLRecordCallback* callback);

	/**
	*@brief 活动开始
	*/
	void OnActivityStart(UInt32 actId);

	/**
	*@brief 活动结束
	*/
	void OnActivityEnd();

	/**
	*@brief 增加购买次数
	*/
	void HandleAddJoinNum(const std::string& uid, UInt32 num);

	/**
	*@brief 请求全民砍价信息
	*/
	void HandleWholeBargainReq(GASPlayer* player);

	/**
	*@brief 获取折扣
	*/
	UInt32 GetDiscount(UInt32 actId);

	/**
	*@brief 区服查询折扣
	*/
	void ZoneQueryDiscount(UInt32 zoneId, UInt32 actId);

private:
	
	/**
	*@brief 获取活动参数次数
	*/
	UInt32 _GetActJoinNum(UInt32 actId);

	/**
	*@brief 折扣下发
	*/
	void _DiscountSync(UInt32 discount);

	/**
	*@brief 获取玩家参与次数
	*/
	UInt32 _GetPlayerJoinNum(const std::string& uid);

	/**
	*@brief 清除玩家参与次数
	*/
	void _ClearPlayerJoinNum();

	/**
	*@brief 更新玩家参与次数DB
	*/
	void _UpdatePlayerJoinNumDB(const std::string& uid, UInt32 num, bool isInsert);

	void _InsertDB(UInt32 actId, UInt32 joinNum);
	void _UpdateDB(UInt32 actId, UInt32 joinNum);

private:

	// 当前活动状态
	UInt32 m_CurActStatus;
	// 当前活动id
	UInt32 m_CurActId;

	// 当前活动玩家参与次数(key->playerid)
	std::map<std::string, UInt32> m_PlayerJoinMap;

	// 历史活动参数人数(key->活动id)
	std::map<UInt32, UInt32> m_HistoryActJoinNumMap;
};

#endif

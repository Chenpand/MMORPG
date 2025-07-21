#ifndef _WORLD_GAME_START_ACTIVITY_SCRIPT_H_
#define _WORLD_GAME_START_ACTIVITY_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>

/**
*@brief 排行榜活动脚本
*/
class SortlistActivityScript : public Avalon::ScriptFile
{
public:
	SortlistActivityScript(Avalon::ScriptState* state);
	~SortlistActivityScript();

	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 发放奖励
	*/
	void SendPrize(UInt32 uType, UInt32 uSubType, UInt32 uRanking, ObjID_t uPlayerId);

	/**
	*@brief 发放奖励给部落每个成员
	*/
	void SendTribePrize(UInt32 uRanking, ObjID_t uPlayerId);

	bool DailySortReward(ObjID_t playerId, UInt32 ranking, UInt32 uSortType);

};

#endif


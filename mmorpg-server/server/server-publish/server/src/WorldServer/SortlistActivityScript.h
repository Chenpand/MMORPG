#ifndef _WORLD_GAME_START_ACTIVITY_SCRIPT_H_
#define _WORLD_GAME_START_ACTIVITY_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>

/**
*@brief ���а��ű�
*/
class SortlistActivityScript : public Avalon::ScriptFile
{
public:
	SortlistActivityScript(Avalon::ScriptState* state);
	~SortlistActivityScript();

	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ���Ž���
	*/
	void SendPrize(UInt32 uType, UInt32 uSubType, UInt32 uRanking, ObjID_t uPlayerId);

	/**
	*@brief ���Ž���������ÿ����Ա
	*/
	void SendTribePrize(UInt32 uRanking, ObjID_t uPlayerId);

	bool DailySortReward(ObjID_t playerId, UInt32 ranking, UInt32 uSortType);

};

#endif


#include "SortlistActivityScript.h"

SortlistActivityScript::SortlistActivityScript(Avalon::ScriptState *state)
	: Avalon::ScriptFile(state)
{
}

SortlistActivityScript::~SortlistActivityScript()
{
}

void SortlistActivityScript::Init()
{
	Call<void>("Init");
}

void SortlistActivityScript::SendPrize(UInt32 uType, UInt32 uSubType, UInt32 uRanking, ObjID_t uPlayerId)
{
	Call<void>("SendActivityPrize", uType, uSubType, uRanking, uPlayerId);
}

void SortlistActivityScript::SendTribePrize(UInt32 uRanking, ObjID_t uPlayerId)
{
	Call<void>("SendTribeMemberPrize", uRanking, uPlayerId);
}

bool SortlistActivityScript::DailySortReward(ObjID_t playerId, UInt32 ranking, UInt32 uSortType)
{
	return Call<bool>("DailySortReward", playerId, ranking, uSortType);
}

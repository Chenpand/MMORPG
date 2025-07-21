#include "WorldPlayerScript.h"
#include "WSPlayer.h"

WorldPlayerScript::WorldPlayerScript(Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{

}

WorldPlayerScript::~WorldPlayerScript()
{

}

void WorldPlayerScript::Init()
{
	Call<void>("Init",this);
}

const char* WorldPlayerScript::GetCopySceneName(UInt32 mapId, UInt16 level)
{
	return Call<const char*>("GetCopySceneName", mapId, level);
}

bool WorldPlayerScript::CheckAnswerRight(ObjID_t playerId, const char* chatWord, const char* rightWord)
{
	return Call<bool>("CheckAnswerRight", playerId, chatWord, rightWord);
}

bool WorldPlayerScript::OnUseFireworks(ObjID_t playerId, const char* playerName, UInt32 value)
{
	return Call<bool>("OnUseFireworks", playerId, playerName, value);
}

void WorldPlayerScript::VisitPlayer(const char* func, WSPlayer *player)
{
	Call<void>(func, player);
}

void WorldPlayerScript::OnAddCharm(ObjID_t playerId, const char* playerName, UInt32 value)
{
	Call<void>("OnAddCharm", playerId, playerName, value);
}

void WorldPlayerScript::OnPublishChatQuestion(const char* playerName)
{
	Call<void>("OnPublishChatQuestion", playerName);
}

void WorldPlayerScript::RegOfflineNotify(const char* str)
{
	m_OfflineNotifyTypes.insert(str);
}

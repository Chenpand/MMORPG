#ifndef _WS_PLAYER_SCRIPT_H_
#define _WS_PLAYER_SCRIPT_H_

#include <CLDefine.h>
#include <set>
#include <AvalonScriptFile.h>

class WSPlayer;

/**
 *@brief 玩家脚本
 */
class WorldPlayerScript : public Avalon::ScriptFile
{
public:
	WorldPlayerScript(Avalon::ScriptState* state);
	~WorldPlayerScript();

public:
	void Init();

	/**
	 *@brief 获取副本名                                                                     
	 */
	const char* GetCopySceneName(UInt32 mapId, UInt16 level);

	bool CheckAnswerRight(ObjID_t playerId, const char* chatWord, const char* rightWord);
	bool OnUseFireworks(ObjID_t playerId, const char* playerName, UInt32 value);
	void VisitPlayer(const char* func, WSPlayer *player);
	void OnAddCharm(ObjID_t playerId,const char* playerName, UInt32 value);
	void OnGivedFriendFlower(UInt32 itemId);
	void OnPublishChatQuestion(const char* playerName);

	void RegOfflineNotify(const char* str);

	bool IsOfflineNotify(const std::string& str){ return m_OfflineNotifyTypes.find(str) != m_OfflineNotifyTypes.end(); }

private:
	std::set<std::string>	m_OfflineNotifyTypes;
};

#endif

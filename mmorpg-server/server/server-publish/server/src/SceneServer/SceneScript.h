#ifndef _SCENE_SCRIPT_H_
#define _SCENE_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Scene;
class Player;
class Npc;
class Creature;

/**
 *@brief scene脚本
 */
class SceneScript : public Avalon::ScriptFile
{
public:
	SceneScript(UInt32 id, Avalon::ScriptState* state);
	~SceneScript();

public:
	void OnSceneInit(Scene* scene);
	void OnSceneQuit(Scene* scene);

	void OnSceneUnload(Scene* scene);

	void OnBeforePlayerEnter(Player* player);
	void OnBeforeNpcEnter(Npc* pNpc);

	void OnPlayerEnter(Player* player);
	void OnPlayerLeave(Player* player);

	/**
	 *@brief 重置关卡
	 */
	void OnResetCustomsPass(Player* player);

	/**
	 *@brief 心跳
	 */
	void OnTick(Scene* scene, UInt64 now);

	/**
	 *@brief 怪物被杀
	 */
	void OnNpcKilled(Npc* npc, Creature* attacker);

	/**
	 *@brief 怪物死亡
	 */
	void OnNpcDied(Npc* npc);

	/**
	 *@brief 玩家被杀死
	 */
	void OnPlayerKilledByPlayer(Player* dead, Player* attacker);

	/**
	*@brief 玩家被npc杀死
	*/
	void OnPlayerKilledByNpc(Player *dead, Npc *attacker);

	/**
	 *@brief npc被杀死
	 */
	void OnNpcKilledByPlayer(Npc* npc, Player* attacker);

	/**
	 *@brief 血量改变
	 */
	void OnFixedDmgHpChanged(Creature* creature);

	/**
	 *@brief 进入事件陷阱
	 */
	bool OnEnterEventTrap(Player* player, UInt32 trapid);
	bool OnNpcEnterEventTrap(Npc* npc, UInt32 trapid);

	/**
	*@brief 进入跳转区域
	*/
	UInt32 OnEnterTransZone(Player *pPlayer, UInt32 uIndex);

	/**
	 *@brief 访问玩家
	 */
	void VisitPlayer(const char* func, Player* player);
	void VisitBattlePlayer(const char* func, Scene* scene, ObjID_t playerId);
	void VisitBattlePlayerInOrder(const char* func, Scene* scene, ObjID_t playerId, UInt32 totalNum, UInt32 order);

	/**
	*@brief 访问部落
	*/
	void VisitBattleTribe(const char *func, Scene *scene, ObjID_t tribeId);
	void VisitBattleTribeInOrder(const char *func, Scene *scene, ObjID_t tribeId, UInt32 order);

	/**
	 *@brief 访问所有npc
	 */
	void VisitNpc(const char* func, Npc* npc);

	/**
	 *@brief 检查能否进入
	 */
	bool CheckPlayerEnter(Scene* scene, ObjID_t id, ObjID_t tribeid, UInt16 level);
	bool CheckPlayerEnterCopy(UInt16 level, Player* player);

	/**
	*@brief 请求召唤、升级战场NPC
	*/
	void ReqSummonBattleCreature(Player *pPlayer, Npc *pBuilder, UInt32 uDataId, UInt32 uParam);
	ObjID_t ReqUpgradeBattleCreature(Player *pPlayer, Npc *pNpc);

	/**
	 *@brief 玩家进行某种操作
	 */
	void DoOperate(Player* player, const char* name, UInt32 param);

	/**
	 *@brief 剧情结束                                                                     
	 */
	void OnStoryEnd(Player* player, UInt32 storyId);

	/**
	 *@brief 定时事件触发                                                                     
	 */
	void OnTimerEventTrigger(Scene* scene, const char* handler);
};

#endif

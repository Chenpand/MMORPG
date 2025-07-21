#ifndef _AI_SCRIPT_H_
#define _AI_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Npc;
class Creature;
class Player;
class Trap;

/**
 *@brief ai脚本
 */
class AIScript : public Avalon::ScriptFile
{
public:
	AIScript(UInt32 id, Avalon::ScriptState* state);
	~AIScript();

public:
	/**
	*@brief 空闲心跳
	*/
	void OnIdleHeartbeat(Npc *pNpc, const Avalon::Time &now);

	/**
	 *@brief 战斗心跳
	 */
	void OnFightHeartbeat(Npc* npc, const Avalon::Time& now);

	/**
	 *@brief 进入和离开战斗状态
	 */
	void OnEnterFightState(Npc* npc, const Avalon::Time& now);
	void OnLeaveFightState(Npc* npc, const Avalon::Time& now);

	/**
	 *@brief 战斗状态中移动
	 */
	void OnMovedInFightState(Npc* npc);

	/**
	 *@brief 攻击和杀死别人事件
	 */
	void OnAttacked(Npc* npc, Creature* attacker);
	void OnAttack(Npc* npc, Creature* target);
	void OnKill(Npc* npc, Creature* target);
	void OnKilled(Npc *npc, Creature *pAttacker);

	/**
	*@brief 被伤害
	*/
	void OnDamaged(Npc *pNpc, UInt32 uDmg, Creature *pAttacker);

	/**
	 *@brief 出生
	 */
	void OnBirth(Npc* npc);

	/**
	 *@brief 死亡
	 */
	void OnDied(Npc* npc);

	/**
	 *@brief 到达                                                                     
	 */
	void OnArrived(Npc* npc);

	/**
	 *@brief 访问玩家
	 */
	void VisitPlayer(const char* func, Npc* npc, Player* player);
	void VisitPlayerParam1(const char* func, Npc* npc, Player* player, UInt64 param1);

	/**
	*@brief 访问战场阵营
	*/
	void VisitBattleTribe(const char *func, Npc *npc, ObjID_t uTribeId, UInt32 ranking, UInt32 playernum);

	/**
	 *@brief 访问陷阱
	 */
	void VisitTrap(const char* func, Trap* trap);
};

#endif

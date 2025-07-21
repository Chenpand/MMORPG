#include "AIScript.h"

AIScript::AIScript(UInt32 id, Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	SetId(id);
}

AIScript::~AIScript()
{
}

void AIScript::OnIdleHeartbeat(Npc* pNpc, const Avalon::Time& now)
{
	Call<void>("OnIdleHeartbeat", pNpc, now.MSec());
}

void AIScript::OnFightHeartbeat(Npc* npc, const Avalon::Time& now)
{
	Call<void>("OnFightHeartbeat", npc, now.MSec());
}

void AIScript::OnEnterFightState(Npc* npc, const Avalon::Time& now)
{
	Call<void>("OnEnterFightState", npc, now.MSec());
}

void AIScript::OnLeaveFightState(Npc* npc, const Avalon::Time& now)
{
	Call<void>("OnLeaveFightState", npc, now.MSec());
}

void AIScript::OnMovedInFightState(Npc* npc)
{
	Call<void>("OnMovedInFightState", npc);
}

void AIScript::OnAttacked(Npc* npc, Creature* attacker)
{
	Call<void>("OnAttacked", npc, attacker);
}

void AIScript::OnAttack(Npc* npc, Creature* creature)
{
	Call<void>("OnAttack", npc, creature);
}

void AIScript::OnKill(Npc* npc, Creature* creature)
{
	Call<void>("OnKill", npc, creature);
}

void AIScript::OnKilled(Npc *pNpc, Creature *pAttacker)
{
	Call<void>("OnKilled", pNpc, pAttacker);
}

void AIScript::OnDamaged(Npc *pNpc, UInt32 uDmg, Creature *pAttacker)
{
	Call<void>("OnDamaged", pNpc, uDmg, pAttacker);
}

void AIScript::OnBirth(Npc* npc)
{
	Call<void>("OnBirth", npc);
}

void AIScript::OnDied(Npc* npc)
{
	Call<void>("OnDied", npc);
}

void AIScript::OnArrived(Npc* npc)
{
	Call<void>("OnArrived", npc);
}

void AIScript::VisitPlayer(const char* func, Npc* npc, Player* player)
{
	Call<void>(func, npc, player);
}

void AIScript::VisitPlayerParam1(const char* func, Npc* npc, Player* player, UInt64 param1)
{
	Call<void>(func, npc, player, param1);
}

void AIScript::VisitBattleTribe(const char *func, Npc *npc, ObjID_t tribeid, UInt32 ranking, UInt32 playernum)
{
	Call<void>(func, npc, tribeid, ranking, playernum);
}

void AIScript::VisitTrap(const char* func, Trap* trap)
{
	Call<void>(func, trap);
}

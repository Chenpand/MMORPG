#ifndef _AI_SCRIPT_H_
#define _AI_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Npc;
class Creature;
class Player;
class Trap;

/**
 *@brief ai�ű�
 */
class AIScript : public Avalon::ScriptFile
{
public:
	AIScript(UInt32 id, Avalon::ScriptState* state);
	~AIScript();

public:
	/**
	*@brief ��������
	*/
	void OnIdleHeartbeat(Npc *pNpc, const Avalon::Time &now);

	/**
	 *@brief ս������
	 */
	void OnFightHeartbeat(Npc* npc, const Avalon::Time& now);

	/**
	 *@brief ������뿪ս��״̬
	 */
	void OnEnterFightState(Npc* npc, const Avalon::Time& now);
	void OnLeaveFightState(Npc* npc, const Avalon::Time& now);

	/**
	 *@brief ս��״̬���ƶ�
	 */
	void OnMovedInFightState(Npc* npc);

	/**
	 *@brief ������ɱ�������¼�
	 */
	void OnAttacked(Npc* npc, Creature* attacker);
	void OnAttack(Npc* npc, Creature* target);
	void OnKill(Npc* npc, Creature* target);
	void OnKilled(Npc *npc, Creature *pAttacker);

	/**
	*@brief ���˺�
	*/
	void OnDamaged(Npc *pNpc, UInt32 uDmg, Creature *pAttacker);

	/**
	 *@brief ����
	 */
	void OnBirth(Npc* npc);

	/**
	 *@brief ����
	 */
	void OnDied(Npc* npc);

	/**
	 *@brief ����                                                                     
	 */
	void OnArrived(Npc* npc);

	/**
	 *@brief �������
	 */
	void VisitPlayer(const char* func, Npc* npc, Player* player);
	void VisitPlayerParam1(const char* func, Npc* npc, Player* player, UInt64 param1);

	/**
	*@brief ����ս����Ӫ
	*/
	void VisitBattleTribe(const char *func, Npc *npc, ObjID_t uTribeId, UInt32 ranking, UInt32 playernum);

	/**
	 *@brief ��������
	 */
	void VisitTrap(const char* func, Trap* trap);
};

#endif

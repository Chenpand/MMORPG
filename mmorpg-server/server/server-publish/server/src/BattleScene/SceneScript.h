#ifndef _SCENE_SCRIPT_H_
#define _SCENE_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Scene;
class Player;
class Npc;
class Creature;

/**
 *@brief scene�ű�
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
	 *@brief ���ùؿ�
	 */
	void OnResetCustomsPass(Player* player);

	/**
	 *@brief ����
	 */
	void OnTick(Scene* scene, UInt64 now);

	/**
	 *@brief ���ﱻɱ
	 */
	void OnNpcKilled(Npc* npc, Creature* attacker);

	/**
	 *@brief ��������
	 */
	void OnNpcDied(Npc* npc);

	/**
	 *@brief ��ұ�ɱ��
	 */
	void OnPlayerKilledByPlayer(Player* dead, Player* attacker);

	/**
	*@brief ��ұ�npcɱ��
	*/
	void OnPlayerKilledByNpc(Player *dead, Npc *attacker);

	/**
	 *@brief npc��ɱ��
	 */
	void OnNpcKilledByPlayer(Npc* npc, Player* attacker);

	/**
	 *@brief Ѫ���ı�
	 */
	void OnFixedDmgHpChanged(Creature* creature);

	/**
	 *@brief �����¼�����
	 */
	bool OnEnterEventTrap(Player* player, UInt32 trapid);
	bool OnNpcEnterEventTrap(Npc* npc, UInt32 trapid);

	/**
	*@brief ������ת����
	*/
	UInt32 OnEnterTransZone(Player *pPlayer, UInt32 uIndex);

	/**
	 *@brief �������
	 */
	void VisitPlayer(const char* func, Player* player);
	void VisitBattlePlayer(const char* func, Scene* scene, ObjID_t playerId);
	void VisitBattlePlayerInOrder(const char* func, Scene* scene, ObjID_t playerId, UInt32 totalNum, UInt32 order);

	/**
	*@brief ���ʲ���
	*/
	void VisitBattleTribe(const char *func, Scene *scene, ObjID_t tribeId);
	void VisitBattleTribeInOrder(const char *func, Scene *scene, ObjID_t tribeId, UInt32 order);

	/**
	 *@brief ��������npc
	 */
	void VisitNpc(const char* func, Npc* npc);

	/**
	 *@brief ����ܷ����
	 */
	bool CheckPlayerEnter(Scene* scene, ObjID_t id, ObjID_t tribeid, UInt16 level);
	bool CheckPlayerEnterCopy(UInt16 level, Player* player);

	/**
	*@brief �����ٻ�������ս��NPC
	*/
	void ReqSummonBattleCreature(Player *pPlayer, Npc *pBuilder, UInt32 uDataId, UInt32 uParam);
	ObjID_t ReqUpgradeBattleCreature(Player *pPlayer, Npc *pNpc);

	/**
	 *@brief ��ҽ���ĳ�ֲ���
	 */
	void DoOperate(Player* player, const char* name, UInt32 param);

	/**
	 *@brief �������                                                                     
	 */
	void OnStoryEnd(Player* player, UInt32 storyId);

	/**
	 *@brief ��ʱ�¼�����                                                                     
	 */
	void OnTimerEventTrigger(Scene* scene, const char* handler);
};

#endif

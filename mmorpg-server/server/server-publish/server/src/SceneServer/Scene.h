#ifndef _SCENE_H_
#define _SCENE_H_

#include <AvalonProtocol.h>
#include <AvalonIDGenerator.h>
#include <AvalonTime.h>
#include <AvalonAStar.h>
#include <AvalonSimpleTimer.h>
#include <set>
#include <vector>
#include <functional>
#include <CLScene.h>
#include <CLObjectIndex.h>
#include <CLSceneDefine.h>
#include <CLSceneDataEntry.h>
#include <CLItemProperty.h>
#include <CLSysNotify.h>
#include <CLChatDefine.h>

#include "SceneObject.h"

class Player;
class Npc;
class Creature;
class Vehicle;
class Trap;
class CityMonster;

/**
 *@brief ����״̬
 */
enum SceneStatus
{
	SCENE_INIT,			//��ʼ��
	SCENE_NORMAL,		//ͨ��
	SCENE_QUIT,			//�˳�״̬
};

/**
 *@brief ������������󳤶�
 */
#define MAX_SCENEPARAM_NAME_LEN 16

/**
 *@brief ������ʱ�¼�                                                                     
 */
struct SceneTimerEvent
{
	SceneTimerEvent():nextTime(0), interval(0), times(1){}

	std::string	handler;
	UInt64		nextTime;
	UInt64		interval;
	UInt32		times;
};

/**
*@brief ��������Ϣ
*/
struct TransportDoor
{
    UInt32      id;
	ScenePos	birthScenePos;
    CLPosition  birthPos;

    UInt32      targetSceneId;
    UInt32      targetDoorId;
};

/**
 *@brief ����
 */
class Scene : public CLScene
{
	AVALON_DEFINE_OBJECTPOOL(Scene, Avalon::NullMutex)

	typedef std::map<std::string, UInt64>	SceneParamMap;
	typedef std::map<UInt64, UInt64>	IdTimeMap;
	typedef std::map<Creature*, UInt64>	CreatureTimeMap;
	typedef std::vector<Scene*>			SceneVec;
	typedef std::vector<SceneTimerEvent>	TimerEventVec;

	//����λ��ѯ��Χ
	const static Int32	NONEGIRD_SEARCH_RANGE = 128;
	//�г�����ʱʱ��
	const static UInt32 CHANGESCENE_TIMEOUT = 10;
	//��������ҵȼ�
	const static UInt32	MAX_GHOSTSCENE_NUM = 5;

public:
	Scene();
	Scene(const Scene& scene);
	~Scene();

public:
	/**
	 *@brief ����ж��
	 */
	virtual bool Load(UInt32 mapid,const std::string& config);
	virtual void UnLoad(){}

	/**
	 *@brief ��ȡ������Դid
	 */
	UInt32 GetMapID() const{ return m_MapID;}

	/**
	 *@brief ��ȡ����
	 */
	virtual UInt8 GetType() const{ return m_pDataEntry->type; }

	/**
	 *@brief ��������
	 */
	void ScriptSetName(const char* name);

	/**
	 *@brief ��ȡ������
	 */
	SceneDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief ��ȡ����
	 */
	UInt32 GetMajorCity() const;

    /**
    *@brief ��û�������
    */
    bool HasThisDoor(UInt32 doorId);

    /**
    *@brief ��ȡ�ŵ�λ��
    */
    CLPosition GetDoorBirthPos(UInt32 doorId);
	ScenePos GetDoorBirthScenePos(UInt32 doorId);

	const std::vector<TransportDoor>& GetDoors() const { return m_doors; }

	/**
	 *@brief ��ȡ�����
	 */
	UInt32 GetPlayerNum(){ return m_PlayerNum; }
	UInt32 GetWaitPlayerNum();
	UInt32 GetMaxPlayerNum() const{ return m_pDataEntry->maxPlayerNum; }

	/**
	 *@brief ��ͼ�㲥
	 */
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);
	//by huchenhui
	//void BroadcastNotifyById(UInt32 id, ...);

	template <typename... Args>
	void BroadcastNotifyById(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		BroadcastNotify(type, color, buffer);
	}

	/**
	 *@brief ͬ��������worldserver
	 */
	virtual void SyncPlayerNumToWorldServer();

public: //��̬��ͼ���
	/**
	 *@brief ���á���ȡ�����ȼ�
	 */
	virtual void SetLevel(UInt16 level){}
	virtual UInt16 GetLevel() const{ return 0; }

	/**
	 *@brief ����ܷ����
	 */
	virtual bool CheckPlayerEnter(ObjID_t id, UInt16 level){ return true; }

    /**
    *@brief ��ȡ������
    */
    virtual CLPosition GetBirthPoint();

    /**
    *@brief ��ȡ���Ӹ߶�
    */
    virtual float GetGridHeight();

    /**
    *@brief ��ȡ���ӿ��
    */
    virtual float GetGridWidth();

public:
	/**
	 *@brief ����Ƴ�����һ��object
	 */
	bool AddObject(SceneObject* obj);
	void RemoveObject(SceneObject* obj);
	SceneObject* FindObject(ObjID_t id);

	/**
	 *@brief ������س�������
	 */
	Creature* FindCreature(ObjID_t id);
	Player* FindPlayer(ObjID_t id);
	
	/**
	 *@brief ����Ƴ�����һ���ȴ��г�����obj
	 */
	bool AddWaitObject(SceneObject* obj);
	UInt32 GetWaitObjectNum() const;
	void RemoveWaitObject(SceneObject* obj);
	void MoveOutWaitList(SceneObject* obj);
	SceneObject* FindWaitObject(ObjID_t id);

	/**
	 *@brief �Ƴ����
	 */
	void ClearItems();

	/**
	 *@brief ���û�ȡ����״̬
	 */
	void SetStatus(SceneStatus status){ m_Status = status; }
	SceneStatus GetStatus() const { return m_Status; }

	/**
	 *@brief �߳����
	 */
	virtual void KickPlayersOut(){}
	virtual void KickPlayerOut(Player* player){}

public://����������

	/**
	*@brief ָ�����������Ϣˢ��
	*/
	CityMonster* SummonCityMonster(ObjID_t guid, ObjID_t owner, const std::string& name,
		UInt8 type, UInt32 dataId, NpcPos pos, UInt32 dungeonId, UInt32 remainTimes, bool notify);

	/**
	*@brief ��ȡ������Ϣ
	*/
	SceneNpcInfo GetNpcInfo(Player* player);

	/**
	*@brief ��ӹ���
	*/
	void AddCityMonster(CityMonster* monster, bool notify);

	/**
	*@brief ɾ������
	*/
	void DelCityMonster(CityMonster* monster, bool notify = true);

	/**
	*@brief ɾ�����˽�еĹ���
	*/
	void DelPersonalCityMonsters(ObjID_t owner, bool notify);

	/**
	*@brief ���ҹ���,owner=0˵������ͨ�������������˽�еĹ���
	*/
	CityMonster* FindCityMonster(ObjID_t owner, ObjID_t id);

	/**
	*@brief ���ҹ���
	*/
	CityMonster* FindCityMonster(ObjID_t id);

	/**
	*@brief �������е�ͨ�ù���
	*/
	void VisitCommonCityMonsters(const std::function<bool(CityMonster*)>& F);

public://·���赲���
	/**
	 *@brief ���ü���赲
	 */
	void SetBlock(const CLPosition& pos, UInt8 flag);
	void UnsetBlock(const CLPosition& pos, UInt8 flag);
	bool CheckBlock(const CLPosition& pos, UInt8 flag);
	/**
	 *@brief �жϴ�һ�㵽��һ���ֱ�߾�����û��ĳЩ���͵��赲
	 */
	bool CheckBlock(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag);
	/**
	 *@brief ��һ����������Χ��ȡһ������
	 */
	bool FindPosNearby(const CLPosition& pos, CLPosition& tarpos, UInt8 flag);
	/**
	 *@brief �����߶�����ֱ�ߵ������Զ�ĵ�
	 */
	bool FindPosReachable(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag, CLPosition& tarpos);
	/**
	 *@brief �ڷ�Χ�������һ��
	 *@return true �ҵ���  false û�ҵ�
	 */
	bool FindRandPos(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief �ڵ�ͼ�������һ����
	 */
	bool FindRandPos(CLPosition& pos, UInt8 blockFlag, UInt32 edgeWidth = 3);
	/**
	 *@brief ��һ��Ȧ�������һ����
	 */
	bool FindRandPosOnCircle(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	*@brief ��һ��Ȧ�������ָ���赲���͵�һ����
	*/
	bool FindRandBlockPosOnCircle(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief ��һ��Ȧ��ָ�������Ͽ�ʼ�Ҹ���
	 */
	bool FindPosInDirection(const CLPosition& center, Int32 radius, Int32 dir, CLPosition& pos, UInt8 blockFlag);
	/**
	 *@brief ����һ��·��
	 */
	bool FindPath(const CLPosition& startPos, const CLPosition& endPos, std::vector<CLPosition>& path, UInt8 blockFlag);

public://�¼�
	/**
	 *@brief ������ʼ��
	 */
	virtual void OnSceneInit();

	/**
	 *@brief �����˳�
	 */
	virtual void OnSceneQuit();

	/**
	 *@brief ������ʼж��
	 */
	virtual void OnSceneUnload();

	/**
	 *@brief ��Ҽ�������
	 */
	virtual void OnBeforePlayerEnter(Player* player);

	/**
	 *@brief npc��������                                                                     
	 */
	virtual void OnBeforeNpcEnter(Npc* npc);

	/**
	 *@brief ��ҽ���
	 */
	virtual void OnPlayerEnter(Player* player);

	/**
	 *@brief ����뿪
	 */
	virtual void OnPlayerLeave(Player* player);

	/**
	 *@brief tick
	 */
	virtual void OnTick(const Avalon::Time& now);

public://���ʽӿ�

	/**
	 *@brief ����objects
	 */
	virtual void VisitSceneObjects(SceneObjectVisitor& visitor);
	
	/**
	 *@brief �ű������������
	 */
	void ScriptVisitPlayers(const char* func);

	/**
	 *@brief ������Χ�����
	 */
    virtual void VisitPlayersAround(const CLPosition& pos, CLVisitor<Player>& visitor);

	/**
	 *@brief ������Χ��npc
	 */
	void VisitNpcsAround(const CLPosition& pos, CLVisitor<Npc>& visitor);

	/** 
	 *@brief ������Χ����Һ�npc����������
	 */
	void VisitOthersAround(const CLPosition& pos, SceneObjectVisitor& visitor);

	/**
	 *@brief ������Χ��object
	 */
    virtual void VisitObjectsAround(const CLPosition& pos, SceneObjectVisitor& visitor);

	/**
	 *@brief ����except
	 */
	void VisitPlayersNewZone(const SceneObject* obj,  CLVisitor<Player>& visitor);
	void VisitPlayersOldZone(const SceneObject* obj,  CLVisitor<Player>& visitor);

	/**
	 *@brief ����except
	 */
	virtual void VisitObjectsNewZone(const SceneObject* obj, SceneObjectVisitor& visitor);
	virtual void VisitObjectsOldZone(const SceneObject* obj, SceneObjectVisitor& visitor);

    /**
    *@brief �ܷ񿴵��������
    */
    virtual bool CanSeeOthers() { return true; }

	/**
	 *@brief �ж��Ƿ�����Χ
	 */
	bool IsInBroadcastZone(const SceneObject* obj1, const SceneObject* obj2) const;

	/**
	 *@brief ��ȡλ��posͬ�����е������
	 */
	UInt32 GetPlayerNumAround(const CLPosition& pos) const;

	/**
	 *@brief �����������������
	 */
	void VisitPlayers(CLVisitor<Player>& visitor);

public://��Ϣ���
	/**
	 *@brief �㲥�������������
	 */
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);

private:
	//id-obj����
	CLIDObjIndex<SceneObject>	m_Objects;
	//�ȴ����볡�����б�
	CLIDObjIndex<SceneObject>	m_WaitObjects;

	//����ʧ�б�
	IdTimeMap		m_WaitDisappearMap;
	//�������б�
	CreatureTimeMap	m_WaitReviveMap;

	//a*Ѱ·
	Avalon::AStar	m_AStar;

	//��ͼid
	UInt32			m_MapID;
	//����״̬
	SceneStatus		m_Status;
	//���˶�ʱ��
	Avalon::SimpleTimer	m_KickTimer;
	//�붨ʱ��
	Avalon::SimpleTimer	m_SecTimer;

	//��ǰ�����
	UInt32			m_PlayerNum;
	//ԭ�����
	UInt32			m_OldPlayerNum;

	//��ͼ����
	SceneDataEntry*	m_pDataEntry;

    //������λ��
    CLPosition      m_BirthPos;

    //���Ӹ߶�
    float           m_GridHeight;
    //���ӿ��
    float           m_GridWidth;

    std::vector<TransportDoor>   m_doors;

	// ���й���
	std::map<ObjID_t, CityMonster*>					m_AllMonsters;

	// ͨ�ù���(�����˶����ü�)
	std::map<ObjID_t, CityMonster*>					m_CommonMonsters;

	// ���˽�еĹ�(ֻ���Լ����ü�)
	std::map<ObjID_t, std::vector<CityMonster*> >	m_PlayerMonsters;
};

typedef CLVisitor<Scene> SceneVisitor;

#endif

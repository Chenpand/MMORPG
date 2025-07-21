#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#include <CLSceneObject.h>
#include <AvalonTime.h>
#include <AvalonPacket.h>
#include <set>

class Scene;

/**
 *@brief ��������
 */
class SceneObject : public CLSceneObject
{
	typedef std::set<SceneObject*> SceneObjectSet;

public:
	SceneObject();
	~SceneObject();
	
	/**
	 *@brief ��ȡid
	 */
	ObjID_t	GetID() const{ return CLSceneObject::GetID(); }

	/**
	 *@brief ���û�ȡ����
	 */
	void SetName(const std::string& name){ m_Name = name; }
	const std::string& GetName() const { return m_Name; }

	void SetDoorID(UInt32 id) { m_doorId = id; }
	UInt32 GetDoorID() const { return m_doorId; }

	/**
	 *@brief �ű����á���ȡ����
	 */
	const char* ScriptGetName() const{ return m_Name.c_str(); }
	void ScriptSetName(const char *szName) { SetName(szName); }

public:
	/**
	 *@brief ���û�ȡ��������
	 */
	void SetScene(Scene* scene){ m_pScene = scene; }
	Scene* GetScene() const{ return m_pScene; }
	UInt32	GetSceneID() const{ return CLSceneObject::GetSceneID(); }

	/**
	 *@brief ����λ��
	 */
	void SetPos(const CLPosition& pos);

	/**
	 *@brief ���û�ȡ����id
	 */
	virtual bool SetGateID(UInt32 nodeid){ return true; }
	virtual UInt32 GetGateID() const { return 0; } 

	/**
	 *@brief �����˺�id
	 */
	virtual void SetAccID(UInt32 accid){}
	virtual UInt32 GetAccID() const { return 0; }

	/**
	 *@brief �Ƿ��ܿ�����
	 */
	virtual bool IsCanSeeMe(SceneObject* obj);

	/**
	 *@brief �Ƿ������
	 */
	virtual bool IsHidden(){ return false; }

	/**
	 *@brief ��ʼ����
	 */
	virtual bool SetupBase() = 0;

	/**
	 *@brief ����object
	 */
	static SceneObject* CreateObject(int type);
	static void FreeObject(SceneObject* obj);
	static void DestroyObjects();
	static ObjID_t GenObjID();
	static void SetIDSeed(ObjID_t id);

	/**
	 *@brief ͬ�����Ա仯
	 */
	virtual void SyncProperty();

    void SetPosChanged(bool changed) { m_bPosChanged = changed; }

public://�������
	/**
	 *@brief �л�����
	 *@param sceneid Ŀ�곡��id
	 *@param pos Ŀ��λ��
	 *@param dir Ŀ�귽��
	 *@param bTransZone �Ƿ���������ת���г���
	 */
	virtual bool ChangeScene(UInt32 sceneid, const CLPosition& pos, UInt8 dir, bool bTransZone = false, UInt32 targetDoorId = 0);

	/**
	*@brief �Ƿ���Խ��볡��
	*/
	virtual bool CanEnterMap(UInt32 sceneid) { return true; }

public://�¼�
	/**
	 *@brief ����
	 */
	virtual void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief �������볡���¼����˽ӿ�ͨ������ȷ��λ��
	 */
	virtual void OnBeforeEnterScene();

	/**
	 *@brief ���볡���¼�
	 */
	virtual void OnEnterScene();

	/**
	 *@brief �뿪�����¼�
	 */
	virtual void OnLeaveScene();

	/**
	 *@brief �л�����
	 */
	virtual void OnSceneChanged(){}

	/**
	 *@brief ͬ������ӷ����仯
	 */
	virtual void OnGridChanged();

	/**
	 *@brief �ƶ��¼�
	 */
	virtual void OnMoved(const CLPosition& oldpos);

	/** 
	 *@brief ����ڶ���
	 */
	virtual void OnDayChanged();

	//����Ƿ������
	virtual bool CheckDayChange(const Avalon::Time& now);

public:
	/**
	 *@brief �㲥����Χ�ܿ����ҵ�
	 */
	virtual void Broadcast(Avalon::Protocol& protocol);
	virtual void Broadcast(Avalon::Packet* packet);
	virtual void BroadcastExceptMe(Avalon::Protocol& protocol);
	virtual void BroadcastExceptMe(Avalon::Packet* packet);

	/**
	 *@brief �㲥��������
	 */
	virtual void BroadcastNewZone(Avalon::Packet* packet);
	virtual void BroadcastOldZone(Avalon::Packet* packet);

public:
	/**
	 *@brief ����ͬ��object����Ϣ��
	 */
	Avalon::Packet* MakeSyncObjectPacket(bool bEnterScene);

	/**
	 *@brief ����ͬ�����Ե���Ϣ��
	 */
	Avalon::Packet* MakeSyncPropertyPacket(UInt32 type, bool bDirty);
	Avalon::Packet* MakeSyncPropertyPacket(UInt32 type);

	/**
	 *@brief ����ɾ��obj����Ϣ��
	 */
	Avalon::Packet* MakeDelObjectPacket();

	/**
	 *@brief �����г�������Ϣ��
	 */
	Avalon::Packet* MakeChangeSceneDataPacket(ChangeSceneInfo& info);

public:
	CL_SYNC_PROPERTY_BEGIN(CLSceneObject)
	CL_SYNC_PROPERTY(m_Name, SOA_NAME, SST_SELF | SST_AROUND | SST_TEAM | SST_WATCH | SST_SCENE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSceneObject)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY_END()

	DEFINE_CHANGESCENE_SERIALIZATION(CLSceneObject)
	{
		stream & m_bDayChanged;
	}

	virtual void ClearInvalidData(){}


public:
	//�Ƿ񴥷����ڱ仯
	UInt8		m_bDayChanged;

private:
	//id��������
	static UInt64	m_IdSeed;
	//�������б�
	static SceneObjectSet	m_FreeObjects;

	//����
	Scene*		m_pScene;
	//ԭλ��
	CLPosition	m_OldPos;
	//λ���Ƿ�仯
	bool		m_bPosChanged;

	//����
	OwnerObjString	m_Name;

	//��ID
	UInt32		m_doorId;
};

/**
 *@brief ��Ҷ��������
 */
typedef CLVisitor<SceneObject> SceneObjectVisitor;

#endif

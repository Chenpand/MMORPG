#ifndef _WS_SCENE_H_
#define _WS_SCENE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <set>
#include <CLSceneDataEntry.h>

class WSPlayer;

/**
 *@brief World������
 */
class WSScene
{
public:
	WSScene();
	~WSScene();

public:
	/**
	 *@brief ��ʼ��
	 */
	bool Init(UInt32 id, UInt32 mapid, const std::string& name, UInt32 nodeid);

	/**
	 *@brief ��ȡ����id
	 */
	UInt32 GetID() const{ return m_Id; }

	/** 
	 *@brief ��ȡ������
	 */
	const char* ScriptGetName() const{ return m_Name.c_str(); }

	/**
	 *@brief ��ȡmapid
	 */
	UInt32 GetMapID() const { return m_MapID; }

	/**
	 *@brief ��ȡ���ڷ�����id
	 */
	UInt32 GetNodeID() const{ return m_Conn->GetID(); }

	/**
	 *@brief ���û�ȡ����
	 */
	void SetType(UInt8 type){ m_Type = type; }
	UInt8 GetType() const{ return m_Type; }

	/**
	 *@brief ��ȡ�������
	 */
	UInt32 GetMaxPlayerNum() const{ return m_pDataEntry->maxPlayerNum; }

	/**
	*@brief ��ȡ�������
	*/
	SceneDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief ��ȡ����ʱ��
	 */
	UInt32 GetCreateTime() const{ return m_CreateTime; }

	/**
	 *@brief ���ó�������
	 */
	void SetParam(const char* name, UInt64 value);

	/** 
	 *@brief ���û�ȡ���������
	 */
	void SetPlayerNum(UInt32 num){ m_PlayerNum = num; }
	UInt32 GetPlayerNum() const{ return m_PlayerNum; } 

public://�������
	/**
	 *@brief ���û�ȡ�����ȼ�
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief ���û�ȡboss��ɱ�б�
	 */
	void SetBossList(const std::map<UInt32, UInt8>& bosses){ m_Bosses = bosses; }
	const std::map<UInt32, UInt8>& GetBossList() const{ return m_Bosses; }

	/**
	 *@brief ����Ƿ��һ�ν��븱��
	 */
	bool IsFirstEnter(ObjID_t playerId);

	/**
	 *@brief ���û�ȡ������
	 */
	void SetOwner(ObjID_t owner){ m_Owner = owner; }
	ObjID_t GetOwner() const{ return m_Owner; }

	/**
	 *@brief ���ø����
	 */
	void SetRevivePos(const CLPosition& pos){ m_RevivePos = pos; }
	const CLPosition& GetRevivePos() const{ return m_RevivePos; }

	/**
	 *@brief ����
	 */
	void PullPlayer(WSPlayer* player);
	void PullPlayerToPos(WSPlayer* player, const CLPosition& pos);

public://�¼�
	/**
	 *@brief ����������
	 */
	void OnCreated(WSPlayer* creater);

	void OnDestroyed();

public://����
	/**
	 *@brief ���͵�����������
	 */
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

private:
	//id
	UInt32	m_Id;
	//����
	std::string m_Name;

	//��������������
	Avalon::NetConnectionPtr m_Conn;
	//map id
	UInt32 m_MapID;
	//����
	UInt8	m_Type;
	//����������
	SceneDataEntry*	m_pDataEntry;

	//����ʱ��
	UInt32	m_CreateTime;

	//�����ȼ�
	UInt16	m_Level;
	//����boss
	std::map<UInt32, UInt8> m_Bosses;
	//���������������б�
	std::set<ObjID_t>	m_EnterPlayers;

	//������
	ObjID_t	m_Owner;
	//�����
	CLPosition	m_RevivePos;

	//�������
	UInt32	m_PlayerNum;
};

typedef CLVisitor<WSScene> WSSceneVisitor;

#endif 

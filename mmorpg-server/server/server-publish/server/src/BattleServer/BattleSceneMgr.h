#ifndef _BATTLE_SCENE_MGR_H_
#define _BATTLE_SCENE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>

#include "GameZoneMgr.h"

class BattleScene;

/**
 *@brief ս������������                                                                     
 */
class BattleSceneMgr : public Avalon::Singleton<BattleSceneMgr>
{
public:
	//һ�ֵ�ʱ��
	const static UInt32	ROUND_LASTTIME = 3 * 60;
	//����ʱ��
	const static UInt32 ROUND_SIGNIN_TIME = 60;

	typedef std::vector<BattleScene*>	SceneVec;
	typedef std::vector<UInt32>			SceneServerVec;	

public:
	BattleSceneMgr();
	~BattleSceneMgr();

public:
	bool Init();
	void Final();

	BattleScene* FindBattleScene(UInt32 nodeId);

public:
	/**
	 *@brief �¼�                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ��������                                                                     
	 */
	void OnSceneCreated(UInt32 nodeId, UInt32 sceneId);

	/**
	 *@brief ��������                                                                     
	 */
	void OnSceneDestroyed(UInt32 sceneId);

	/**
	 *@brief �������������ӽ���                                                                     
	 */
	void OnSceneServerConnected(UInt32 nodeId);

	/**
	 *@brief �������������ӶϿ�                                                                     
	 */
	void OnSceneServerDisconnected(UInt32 nodeId);

	/**
	*@brief ���һ�����õĳ���������
	*/
	UInt32 SelectSceneServer();

	/**
	*@brief ��óԼ�׼������������
	*/
	UInt32 GetChiJiSceneServer();

private:
	//�����б�
	SceneVec					m_Scenes;

	//�����������б�
	SceneServerVec				m_SceneServers;

	//����������ʱ��
	Avalon::SimpleTimer			m_CreateSceneTimer;

	//��һ������������
	UInt32						m_SceneServer;
};

#endif

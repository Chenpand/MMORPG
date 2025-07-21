#pragma once
#include "CLDefine.h"

#include <AvalonSimpleTimer.h>

class BSBaseBattle;

class BSBaseBattleMgr
{
public:
	typedef std::vector<UInt32>			SceneServerVec;

public:
	BSBaseBattleMgr();
	~BSBaseBattleMgr();

public:
	/**
	*@brief ��ȡbattle id
	*/
	static UInt32 GenBattleID();

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

	//ս��
	virtual bool AddBattle(BSBaseBattle* battle);
	virtual void DelBattle(BSBaseBattle* battle);

	BSBaseBattle* FindBattle(UInt32 battleId);

	void DestoryBattle(BSBaseBattle* battle);

	void OnTick(const Avalon::Time& now);

	virtual void OnTickImp(const Avalon::Time& now) {};

	virtual void DestoryBattleImp(BSBaseBattle* battle) {};

	template<class T>
	T* FindBattleWithType(ObjID_t id)
	{
		auto race = FindBattle(id);
		if (!race)
		{
			return NULL;
		}

		return dynamic_cast<T*>(race);
	}
	
	virtual void SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime);
protected:
	//����ս��<battleId, BSBaseBattle*>
	std::map<UInt32, BSBaseBattle*>	m_allBattles;

	//��ǰ���Է����ID
	static UInt32 m_CurrentID;

	//�����������б�
	SceneServerVec				m_SceneServers;
	//��һ������������
	UInt32						m_SceneServer;

	// ׼��ʱ��
	UInt32 m_PreTime;
	// ��ʼʱ��
	UInt32 m_StartTime;
	// ����ʱ��
	UInt32 m_EndTime;
	// �ɽ�ʱ��
	UInt32 m_AwardTime;

	// ��ʱ����0.5s
	Avalon::SimpleTimer	m_Timer;
};
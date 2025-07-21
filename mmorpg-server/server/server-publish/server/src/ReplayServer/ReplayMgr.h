#ifndef __REPLAY_MGR_H__
#define __REPLAY_MGR_H__

#include <memory> 
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include "ReplayList.h"

class ReplayMgr : public Avalon::Singleton<ReplayMgr>
{
public:
	ReplayMgr();
	~ReplayMgr();

	/**
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief ����
	*/
	void Final();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

public:
	/**
	*@brief ����¼���б�
	*/
	void LoadReplayList(ReplayListType type, ObjID_t ownerid, ReplayListCallback* cb);

	/**
	*@brief ��ȡ¼���б�
	*/
	ReplayList* FindReplayList(ReplayListType type, ObjID_t ownerid);

	/**
	*@brief ���¼���б�
	*/
	void AddReplayList(ReplayList* list);

	/**
	*@brief ɾ��¼���б�
	*/
	void DelayDelReplayList(ReplayListType type, ObjID_t id, UInt64 delaySec = 10 * Avalon::Time::SECS_OF_MIN);
	void DelReplayList(ReplayListType type, ObjID_t id);

public:
	/**
	*@brief ��ȡ¼����Ϣ
	*/
	ReplayInfoPtr FindReplayInfo(ObjID_t raceid);

	/**
	*@brief ��ȡ¼����Ϣ
	*/
	bool AddReplayInfo(ReplayInfoPtr info);

	/**
	*@brief ����¼�����ݿ�
	*/
	void InsertReplayInfoToDB(ReplayInfoPtr info);

	/**
	*@brief ����¼�����ݵ����ݿ�
	*/
	void UpdateReplayInfoToDB(ReplayInfoPtr info);

	/**
	*@brief ����¼���ļ�
	*/
	ReplayFile* FindReplayFile(ObjID_t raceid);

	/**
	*@brief ɾ��¼���ļ�
	*/
	void DelReplayFile(ObjID_t raceid);

	/**
	*@brief ����¼���ļ������ݿ�
	*/
	bool InsertReplayFileToDB(ReplayFile* replay);

public:
	/**
	*@brief �ܷ������ֶԾ������б���
	*/
	bool CanJoinMasterWaitList(UInt32 score);

	/**
	*@brief ���뵽���ֶԾ������б���
	*/
	bool AddToMasterWaitList(ReplayInfoPtr info, ReplayFile* file);

	/**
	*@brief ���뵽�ͽ�������̭���б���
	*/
	bool AddToPremiumLeagueBattleList(ReplayInfoPtr info, ReplayFile* file);

	/**
	*@brief �Ѷ������зŵ�һ��������
	*/
	void UpgradeMasterWaitList();

public:
	// ¼�񻺴�
	/**
	*@brief ���뵽¼�񻺴�
	*/
	void AddToReplayBuffer(ObjID_t raceid, UInt8* buffer);

	/**
	*@brief ���뵽¼�񻺴�
	*/
	UInt8* GetReplayBuffer(ObjID_t raceid);

	/**
	*@brief ɾ��¼�񻺴�
	*/
	void DelReplayBuffer(ObjID_t raceid);

private:
	HashMap<ObjID_t, ReplayList*>		m_ReplayLists[REPLAY_LIST_NUM];
	std::map<ObjID_t, ReplayInfoPtr>	m_ReplayInfoes;
	std::map<ObjID_t, ReplayFile*>		m_ReplayFiles;
	std::map<ObjID_t, UInt8*>			m_ReplayBuffer;

	Avalon::SimpleTimer					m_MasterListTimer;
	Avalon::SimpleTimer					m_ReplayInfoTickTimer;
	Avalon::SimpleTimer					m_ReplayListTickTimer;
};

#endif
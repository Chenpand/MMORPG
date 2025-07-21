#ifndef __SEASON_MGR_H__
#define __SEASON_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include "CLSeasonDefine.h"

class WSSeasonMgr : public Avalon::Singleton<WSSeasonMgr>
{
public:
	WSSeasonMgr(){}
	~WSSeasonMgr(){}

	bool Init();

public:
	/**
	*@brief Tick
	*/
	void OnTick(Avalon::Time now);

	/**
	*@brief ��������
	*/
	void SetSeasonId(UInt32 id);

private:

	/**
	*@brief ��ʼ�����Ա仯ʱ��
	*/
	void _InitSeasonChangeAttrTime();

	/**
	*@brief �������Ա仯ʱ��
	*/
	void _SaveSeasonChangeAttrTime();

	/**
	*@brief ��ȡ���а����id
	*/
	void _GetSeasonSortListPlayerIds();

	/**
	*@brief ��ȡ���а����id
	*/
	void _ClearSeasonSortListPlayerIds();

	/**
	*@brief ��ʼ������״̬
	*/
	void _InitSeasonStatus();

	/**
	*@brief ��������״̬
	*/
	void _SaveSeasonStatus();

	/**
	*@brief ��ʼ������ʱ��
	*/
	void _ResetSeasonTime();

	/**
	*@brief ��ʼ����������ʱ��
	*/
	void _ResetSeasonAttrTime();

	/**
	*@brief �ı�����
	*/
	void _ChangeSeason();

	/**
	*@brief ͬ����scene
	*/
	void _SyncToScene();

public:
	void OnSceneConnected(UInt32 id);

private:
	UInt32			m_Id;

	Avalon::Time	m_StartTime;

	Avalon::Time	m_SeasonStartTime;

	Avalon::Time	m_SeasonEndTime;

	//�ı�����ʱ�� 
	Avalon::Time	m_SeasonChangeAttrTime;

	Avalon::Time	m_SeasonAttrStartTime;

	Avalon::Time	m_SeasonAttrEndTime;

	

	//�������а����ID
	std::vector<UInt64>	m_SeasonSortListPlayerIds;

};



#endif  //__SEASON_MGR_H__
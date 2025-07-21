#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include <AvalonSimpleTimer.h>

#include <CLGameDefine.h>
#include <CLGlobalActivityServerDefine.h>


// ��Ҫͬ�������Ϸ����Ϣ
struct NeedSyncActivityGameZoneInfo
{
	NeedSyncActivityGameZoneInfo()
	{
		zoneId = 0;
		needSync = false;
	}

	UInt32 zoneId;
	bool   needSync;
};

class GASGameZone;

class ActivityMgr : public Avalon::Singleton<ActivityMgr>
{
public:
	ActivityMgr();
	~ActivityMgr();

public:
	bool OnInit();
	void Final();

	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ���ر��
	*/
	void ReloadTable();

	/**
	*@brief ���һ����Ӫ�
	*/
	void AddOpActivityInfo(OpActivityInfo* opAct);

	/**
	*@brief ����һ����Ӫ�
	*/
	OpActivityInfo* FindOpActivityInfo(UInt32 actId);

	/**
	*@brief ������Ӫ�
	*/
	void VisitOpActivities(const std::function<bool(OpActivityInfo*)>& F);

public:
	/**
	*@brief ��gm����
	*/
	void RequestGmOpActivity();

	/**
	*@brief gm������Ӫ�
	*/
	void GmSetOpActivity(const OpActivityInfo& opAct);

	/**
	*@brief gm�ر���Ӫ�
	*/
	bool GmCloseOpActivity(UInt32 id);

public:
	void OnConnected(UInt32 id);

	/**
	*@brief ������Ҫͬ�������Ϸ����Ϣ
	*/
	void _ResetNeedSyncActGameZoneInfo();

	/**
	*@brief �㲥�������������
	*/
	void _BroadcastActivity(const OpActivityInfo* opAct);

	/**
	*@brief �㲥���л��������
	*/
	void _BroadcastActivities();

	/**
	*@brief �㲥���л��ָ����
	*/
	void _BroadcastActivities(GASGameZone* zone);

private:
	/**
	*@brief ���ر��
	*/
	bool _LoadTable(bool isReload = false);

	/**
	*@brief ���ر��
	*/
	bool _LoadTaskTable();

	/**
	*@brief �״̬�ı�
	*/
	void _OnActivityStateChange(OpActivityInfo* opAct, ActivityState status);

	/**
	*@brief �׼��
	*/
	void _OnActivityPrepare(const OpActivityInfo* opAct);

	/**
	*@brief �����
	*/
	void _OnActivityStart(const OpActivityInfo* opAct);

	/**
	*@brief ��ر�
	*/
	void _OnActivityEnd(const OpActivityInfo* opAct);

private:	
	// ��Ӫ�
	OpActivityInfoVec m_OpActivityInfoVec;
	// ����ʱ��ʱ�
	OpActivityInfoVec m_TempOpActivityInfoVec;

	// ��Ҫͬ�������Ϸ��
	std::vector<NeedSyncActivityGameZoneInfo*> m_NeedSyncActGameZones;

	// ÿ�붨ʱ��
	Avalon::SimpleTimer m_PerSecTimer;
};

#endif

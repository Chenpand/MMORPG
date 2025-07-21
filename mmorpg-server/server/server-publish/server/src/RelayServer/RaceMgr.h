#ifndef __RACE_MGR_H__
#define __RACE_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include <CLMatchDefine.h>
#include "RaceDefine.h"

struct ReconnectInfo
{
	ReconnectInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	ObjID_t		roldId;
	ObjID_t		raceId;
	UInt32		timeoutTime;
};

// ս��ͳ��
struct RaceStatistic
{
	RaceStatistic()
	{
		memset(this, 0, sizeof(*this));
	}

	// �����ı�������
	UInt32		createNum;
	// ��ʼ�ı�������
	UInt32		startNum;
	// ��ͬ���ı�������
	UInt32		unsyncNum;
};

class Race;
class RaceMgr : public Avalon::Singleton<RaceMgr>
{
public:
    RaceMgr();
    ~RaceMgr();

	bool OnInit(Avalon::PropertySet* prop);

    void OnTick(const Avalon::Time& now);

    Race* CreateRace(RaceType type);

    void AddRace(Race* race);
    Race* FindRace(ObjID_t id);
    
    template<class T>
    T* FindRaceWithType(ObjID_t id)
    {
        auto race = FindRace(id);
        if (!race)
        {
            return NULL;
        }

        return dynamic_cast<T*>(race);
    }

    void DelRace(ObjID_t id);

public:
	// �������

	/**
	*@brief �ܷ�
	*/
	const ReconnectInfo* GetReconnectInfo(ObjID_t roleId);

	/**
	*@brief ɾ��������Ϣ
	*/
	void DelReconnectInfo(ObjID_t roleId);

	/**
	*@brief ���������Ϣ
	*/
	void AddReconnectInfo(ObjID_t roleId, ObjID_t raceId, UInt32 timeoutTime);

	/**
	*@brief ��ù�ս����
	*/
	UInt32 GetObserveLoad();

public:
	/**
	*@brief ���������
	*/
	UInt32 GetPhysicalFPS() const { return m_physicalFPS; }

	/**
	*@brief �߼��������
	*/
	UInt32 GetLogicalFPS() const { return m_logicalFPS; }

public:
	// ����ͳ��
	void IncCreateRaceNum(RaceInstanceType type);
	void IncStartRaceNum(RaceInstanceType type);
	void IncUnsyncRaceNum(RaceInstanceType type);

private:
    HashMap<ObjID_t, Race*>         m_races;
	HashMap<ObjID_t, ReconnectInfo>	m_reconnInfo;

	// ϵͳ����
	UInt32							m_physicalFPS;
	UInt32							m_logicalFPS;

	// �㱨ս����Ϣ��ʱ��
	Avalon::SimpleTimer				m_reportTimer;

	// ����ͳ��
	RaceStatistic					m_raceStatistic[RACE_NUM];
};

#endif
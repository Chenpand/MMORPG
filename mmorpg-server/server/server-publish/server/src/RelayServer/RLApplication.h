#ifndef _RELAY_APPLICATION_H_
#define _RELAY_APPLICATION_H_

#include <set>

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

class RLApplication : public CLApplication, public  Avalon::Singleton<RLApplication>
{
public:
	RLApplication ();
	~RLApplication ();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	void SetIsSendUnsyncNotify(bool isSend) { m_IsSendUnsyncNotify = isSend; }
	bool IsSendUnsyncNotify() { return m_IsSendUnsyncNotify; }
	bool IsOpenReplay() const { return m_IsOpenReplay; }

	UInt32 GetVersion() const { return m_Version; }

	/**
	*@brief �Ƿ����ظ�������³�
	*/
	bool IsOpenVerifyDungeon() const { return m_OpenVerifyDungeon; }

	/**
	*@brief �Ƿ����µ���������
	*/
	bool IsOpenNewChecksumCheck() const { return m_OpenNewChecksumCheck; }

	/**
	*@brief ���³��������ͬ��ʱT��
	*/
	bool IsKickPlayerWhenDungeonUnsync() const { return m_KickPlayerWhenDungeonUnsync; }

	/**
	*@brief �Ƿ���֤PK
	*/
	bool IsVerifyPK() const { return m_OpenVerifyPK; }

	/**
	*@brief ��֤����ʱ������������ھ�ɾ��
	*/
	bool NotifyGiveupVerifyWhenRaceUnexist() { return m_NotifyGiveupVerifyWhenRaceUnexist; }

	/**
	*@brief �Ƿ�������
	*/
	bool IsOpenHeartbeat() const { return m_OpenHeartbeat; }

	/**
	*@brief ������ʱʱ��
	*/
	UInt32 GetHeartbeatTimeoutSec() const { return m_HeartbeatTimeoutSec; }

	/**
	*@brief ���������ʱ��
	*/
	UInt32 GetHeartbeatIntervalSec() const { return m_HeartbeatIntervalSec; }

	/**
	*@brief ��սRelay�ȴ�������Ӷ�ʱʱ��(s)
	*/
	UInt32 GetObserveWaitSec() const { return m_ObserveWaitSec; }

	Avalon::SockAddr GetAddr();

	/**
	*@brief �Ƿ���Ҫ�ȴ���֤����
	*/
	bool IsDungeonNeedWaitVerifyRaceEnd(UInt32 dungeonid);

	/**
	*@brief �ȴ���֤����ʱ��(s)
	*/
	bool GetDungeonWaitVerifyRaceEndSec() const { return m_WaitVerifyRaceEndSec; }

protected:
	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	void LoadDynamicConfig();
	void LoadVersion();
	bool ReadVersionFromStr(std::string str);
	UInt32 MakeVersion(UInt32 major, UInt32 minor, UInt32 develop);

private:
    Avalon::SimpleTimer         memCheckTimer;

	// �Ƿ�Ҫ���¼��������ļ�
	bool						m_ReloadConfig;

	// �Ƿ��Ͳ�ͬ����ʾ
	bool						m_IsSendUnsyncNotify;

	// �汾��
	UInt32						m_Version;

	// �Ƿ��¼��
	bool						m_IsOpenReplay;

	// �Ƿ�����֤
	bool						m_OpenVerifyDungeon;

	// �Ƿ����µ���������
	bool						m_OpenNewChecksumCheck;

	// ���³��������ͬ��ʱT��
	bool						m_KickPlayerWhenDungeonUnsync;

	// �Ƿ�����֤1v1
	bool						m_OpenVerifyPK;
	// ��֤����ʱ������������ھ�ɾ��
	bool						m_NotifyGiveupVerifyWhenRaceUnexist;

	// �Ƿ����������
	bool						m_OpenHeartbeat;
	// ������ʱʱ��(s)
	UInt32						m_HeartbeatTimeoutSec;
	// ���������ʱ��(s)
	UInt32						m_HeartbeatIntervalSec;
	// ��սRelay�ȴ�������Ӷ�ʱʱ��(s)
	UInt32						m_ObserveWaitSec;

	// ��Ҫ�ȴ���֤����ĵ��³�
	std::set<UInt32>			m_WaitVerifyRaceEndDungeons;
	// �ȴ���֤����ʱ��(s)
	UInt32						m_WaitVerifyRaceEndSec;
};



#endif

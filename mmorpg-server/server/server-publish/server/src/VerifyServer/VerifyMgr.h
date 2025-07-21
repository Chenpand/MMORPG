#ifndef __VERIFY_MGR_H__
#define __VERIFY_MGR_H__

/*
	��������������֤������
*/

#include <sstream>
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <AvalonSimpleTimer.h>
#include "AvalonMonoDelegate.h"
#include "LogicClientInterface.h"

struct VerifyStatistic
{
	VerifyStatistic() { memset(this, 0, sizeof(*this)); }

	std::string ToString()
	{
		std::stringstream ss;
		ss << "create(" << create << ") start(" << start << ") unsync(" << unsync << ") success(" << success << ") timeout(" << timeout << ")"
			<< " raceend(" << raceend << ") raceendUnsync(" << raceEndUnsync << ") averageRaceEndDelay(" << averRaceEndDelayMs << ") maxRaceEndDelayMs("
			<< maxRaceEndDelayMs << ") missLogicRaceEnd(" << missLogicRaceEnd << ").";

		return ss.str();
	}

	// �����ľ���
	UInt32 create;
	// �ɹ���ʼ��֤�ľ���
	UInt32 start;
	// ��ͬ������
	UInt32 unsync;
	// �ɹ���֤�ľ���
	UInt32 success;
	// ��ʱ
	UInt32 timeout;
	// ��֤����ľ���
	UInt32 raceend;
	// ���㲻ͬ������
	UInt32 raceEndUnsync;
	// �ܵĽ����ӳ�
	UInt64 totalRaceEndDelayMs;
	// ����ƽ���ӳ�
	UInt64 averRaceEndDelayMs;
	// ��������ӳ�
	UInt64 maxRaceEndDelayMs;
	// �߼�û�ϴ�����
	UInt32 missLogicRaceEnd;
};

class VerifyTask;
class VerifyMgr : public LogicClentInterface, public Avalon::Singleton<VerifyMgr>
{
public:
	VerifyMgr();
	~VerifyMgr();

	/**
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief ����
	*/
	void Final();

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ��ȡͳ����Ϣ
	*/
	VerifyStatistic& GetStatistic() { return m_Statistic; }
public:
	/**
	*@brief �������
	*/
	VerifyTask* FindTask(ObjID_t id);
	void AddTask(VerifyTask* task);
	void DelTask(ObjID_t id);

	/**
	*@brief �ϱ�
	*/
	void ReportAbilityToCenter();

	/**
	*@brief ��ǰ���ڽ��е���������
	*/
	UInt32 GetVerifyTaskNum() const { return (UInt32)m_Tasks.size(); }

private:
	std::map<ObjID_t, VerifyTask*>		m_Tasks;

	// �ϱ���������Ķ�ʱ��
	Avalon::SimpleTimer					m_ReportTimer;

	// ͳ����Ϣ
	VerifyStatistic						m_Statistic;
	Avalon::SimpleTimer					m_StatistaicTimer;
};

#endif
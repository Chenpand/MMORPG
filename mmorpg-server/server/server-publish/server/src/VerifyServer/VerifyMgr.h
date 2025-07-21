#ifndef __VERIFY_MGR_H__
#define __VERIFY_MGR_H__

/*
	管理所有正在验证的任务
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

	// 创建的局数
	UInt32 create;
	// 成功开始验证的局数
	UInt32 start;
	// 不同步局数
	UInt32 unsync;
	// 成功验证的局数
	UInt32 success;
	// 超时
	UInt32 timeout;
	// 验证结算的局数
	UInt32 raceend;
	// 结算不同步局数
	UInt32 raceEndUnsync;
	// 总的结算延迟
	UInt64 totalRaceEndDelayMs;
	// 结算平均延迟
	UInt64 averRaceEndDelayMs;
	// 结算最大延迟
	UInt64 maxRaceEndDelayMs;
	// 逻辑没上传结算
	UInt32 missLogicRaceEnd;
};

class VerifyTask;
class VerifyMgr : public LogicClentInterface, public Avalon::Singleton<VerifyMgr>
{
public:
	VerifyMgr();
	~VerifyMgr();

	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief 结束
	*/
	void Final();

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 获取统计信息
	*/
	VerifyStatistic& GetStatistic() { return m_Statistic; }
public:
	/**
	*@brief 任务相关
	*/
	VerifyTask* FindTask(ObjID_t id);
	void AddTask(VerifyTask* task);
	void DelTask(ObjID_t id);

	/**
	*@brief 上报
	*/
	void ReportAbilityToCenter();

	/**
	*@brief 当前正在进行的任务数量
	*/
	UInt32 GetVerifyTaskNum() const { return (UInt32)m_Tasks.size(); }

private:
	std::map<ObjID_t, VerifyTask*>		m_Tasks;

	// 上报负载情况的定时器
	Avalon::SimpleTimer					m_ReportTimer;

	// 统计信息
	VerifyStatistic						m_Statistic;
	Avalon::SimpleTimer					m_StatistaicTimer;
};

#endif
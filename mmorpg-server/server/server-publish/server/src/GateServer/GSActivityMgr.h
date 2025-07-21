#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <AvalonSingleton.h>
#include <CLGameDefine.h>
#include <map>
#include <vector>
#include <string>
#include <CLActivityDefine.h>
#include <AvalonSimpleTimer.h>

/**
*@brief 活动
*/
class GSActivityMgr : public Avalon::Singleton<GSActivityMgr>
{
public:
	GSActivityMgr();
	~GSActivityMgr();

	void OnTick(const Avalon::Time& now);

	/**
	*@brief 加载运营活动配置
	*/
	void OnLoadOpActData(OpActRegMap& activeData);

	/**
	*@brief 同步当前运营活动状态
	*/
	void OnOpActStateChange(OpActivityRegInfo& info);

	/**
	*@brief 通过模板类型获取对应的所有运营活动
	*/
	std::vector<OpActivityRegInfo*> GetOpActDataByTmpType(OpActivityType type, bool inActivity = true) const;

public:
	/**
	*@brief 获取所有的预约职业
	*/
	void CountAppointmentOccus();

	/**
	*@brief 判断该时间创建的职业是否还是处于预约活动中
	*/
	bool IsOccuInAppointmentOccuActivity(UInt8 occu, UInt32 createTime) const;

	/**
	*@brief 获取所有的预约职业
	*/
	const std::vector<UInt8>& GetAppointmentOccus() const { return m_AppointmentOccus; }

	/**
	*@brief 设置预约职业
	*/
	void SetAppointmentOccus(const std::vector<UInt8>& occus) { m_AppointmentOccus = occus; }

	/**
	*@brief 是否是预约职业
	*/
	bool IsAppointmentOccu(UInt8 occu) const { return std::find(m_AppointmentOccus.begin(), m_AppointmentOccus.end(), occu) != m_AppointmentOccus.end(); }

	/**
	*@brief 获取最大预约角色个数
	*/
	UInt32 GetMaxAppointmentRoleNum();

private:
	// 定时器
	Avalon::SimpleTimer						m_Timer;
	// 运营活动列表
	std::map<UInt32, OpActivityRegInfo*>	m_opRegInfos;
	// 可预约的职业
	std::vector<UInt8>						m_AppointmentOccus;
};

#endif
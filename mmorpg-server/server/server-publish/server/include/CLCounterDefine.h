#ifndef _CL_COUNTER_DEFINE_H_
#define _CL_COUNTER_DEFINE_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include "TimeUtil.h"

//������������
#define	MAX_COUNTER_NAME_LEN	16

/**
 *@brief ������������
 */
enum CounterCycle
{
	COUNTER_CYCLE_NONE	= 0,	//������
	COUNTER_CYCLE_HOUR	= 1,	//СʱΪ����
	COUNTER_CYCLE_DAY	= 2,	//��Ȼ��Ϊ����
	COUNTER_CYCLE_WEEK	= 3,	//��Ȼ��Ϊ����
	COUNTER_CYCLE_MONTH = 4,	//��Ȼ��Ϊ����
	COUNTER_CYCLE_YEAR	= 5,	//��Ȼ��Ϊ����
	COUNTER_CYCLE_WEEK_SPECIFY = 6,	//ָ������һ��ѭ��
	COUNTER_CYCLE_MONDAY_HOUR = 7, //ָ����һ����ˢ��
	COUNTER_CYCLE_INVALID,
};


struct CounterConfig
{
	std::string		name;
	CounterCycle	type;
	union 
	{
		UInt32		param1;
		UInt32		hour;             //����COUNTER_CYCLE_MONTHˢ����˵�����˶��پͻ���ÿ�¿�ʼ����Сʱ��ˢ��
		UInt32		dayOfWeek;
		UInt32		dayOfMonth;
	};
	
};

/**
 *@brief ��������
 */
class CounterCfg : public Avalon::Singleton<CounterCfg>
{
	typedef std::map<std::string, CounterConfig>	CounterCfgMap;
public:
	CounterCfg(){}
	~CounterCfg(){}

public:
	/**
	 *@brief ע��һ������
	 */
	void RegCounter(const char* name, Int32 cycle, UInt32 param1 = 0)
	{
		if(name == NULL || cycle < COUNTER_CYCLE_NONE || cycle >= COUNTER_CYCLE_INVALID) return;
		CounterConfig config;
		config.name = name;
		config.type = (CounterCycle)cycle;
		config.param1 = param1;
		m_CounterCfg.insert(std::make_pair(name, config));
	}

	Int32 GetCycleType(const std::string& name) const
	{
		CounterCfgMap::const_iterator iter = m_CounterCfg.find(name);
		if(iter == m_CounterCfg.end()) return COUNTER_CYCLE_INVALID;
		return iter->second.type;
	}

	const CounterConfig* GetCycleConfig(const std::string& name) const
	{
		CounterCfgMap::const_iterator iter = m_CounterCfg.find(name);
		if (iter == m_CounterCfg.end()) return NULL;
		return &iter->second;
	}

	bool CheckReset(UInt32 time, const CounterConfig* config) const
	{
		if (!config)
		{
			return false;
		}

		UInt64 curTime = Avalon::Time::CurrentTime().MSec();
		Avalon::Time updateTime(UInt64(time) * 1000);
		Avalon::Date updateDate(updateTime);

		bool reset = false;
		switch(config->type)
		{
		case COUNTER_CYCLE_HOUR:
			{
				if(updateDate.Hour() != CURRENT_DATE.Hour()
					||updateDate.Day() != CURRENT_DATE.Day()
					||updateDate.Month() != CURRENT_DATE.Month()
					||updateDate.Year() != CURRENT_DATE.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_DAY:
			{
				Avalon::Date refreshDate = updateDate;
				refreshDate.Hour(config->hour);
				refreshDate.Min(0);
				refreshDate.Sec(0);
				Avalon::Time refreshTime = refreshDate.ToTime();
				if (updateTime.MSec() >= refreshTime.MSec())
				{
					refreshTime += Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
				}

				if (curTime >= refreshTime.MSec())
				{
					reset = true;
				}
			}
			break;
		case COUNTER_CYCLE_WEEK:
			{
				Avalon::Date date1(updateTime.MSec() - updateDate.WDay() * DAY_TO_SEC * 1000);
				Avalon::Date date2(CURRENT_TIME.MSec() - CURRENT_DATE.WDay() * DAY_TO_SEC * 1000);
				if(date1.Day() != date2.Day()
					||date1.Month() != date2.Month()
					||date1.Year() != date2.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_MONTH:
			{
				Avalon::Time resetTimeOffset(config->hour * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC);
				Avalon::Date updateDate(updateTime - resetTimeOffset);
				Avalon::Date nowTime(curTime - resetTimeOffset);
				if (updateDate.Month() != nowTime.Month()
					|| updateDate.Year() != nowTime.Year())
				{
					reset = true;
				}
			}
			break;
		case COUNTER_CYCLE_YEAR:
			{
				if(updateDate.Year() != CURRENT_DATE.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_WEEK_SPECIFY:
			{
				UInt64 resetMsec = 0;

				/*if ((UInt32)updateDate.WDay() >= config->dayOfWeek &&
					updateDate.Hour() >= 6)
				{
					resetMsec = updateTime.MSec() + (updateDate.WDay() + 7 - (updateDate.WDay() - config->dayOfWeek)) * DAY_TO_SEC * 1000;
				}
				else
				{
					resetMsec = updateTime.MSec() + (config->dayOfWeek - updateDate.WDay()) * DAY_TO_SEC * 1000;
				}*/

				if ((UInt32)updateDate.WDay() > config->dayOfWeek || 
					((UInt32)updateDate.WDay() == config->dayOfWeek && updateDate.Hour() >= 6))
				{
					resetMsec = updateTime.MSec() + (config->dayOfWeek + 7 - (UInt32)updateDate.WDay()) * DAY_TO_SEC * 1000;
				}
				else{
					resetMsec = updateTime.MSec() + (config->dayOfWeek - (UInt32)updateDate.WDay()) * DAY_TO_SEC * 1000;
				}

				Avalon::Date resetDate(resetMsec);
				resetDate.Hour(6);
				resetDate.Min(0);
				resetDate.Sec(0);
				
				if (CURRENT_TIME.MSec() >= resetDate.ToTime().MSec())
				{
					reset = true;
				}
			}
			break;

		case COUNTER_CYCLE_MONDAY_HOUR:
		{
			TimeUtil::RefreshDate refreshDate;
			refreshDate.weekDay = 1;
			refreshDate.hour = config->hour;
			UInt64 nextRefreshTimestamp = 0;
			if (TimeUtil::CalculateNextRefreshTimestampByAppointTime(TimeUtil::REFRESH_TYPE_PER_WEEK, refreshDate, updateTime.MSec(), nextRefreshTimestamp))
			{
				if (curTime >= nextRefreshTimestamp)
				{
					reset = true;
				}
			}
			break;
		}

		default:
			break;
		}

		return reset;
	}

	bool CheckReset(UInt32 time, Int32 cycle, UInt32 hour = 0) const
	{
		if(cycle == COUNTER_CYCLE_NONE) return false;

		Avalon::Time updateTime(UInt64(time) * 1000);
		Avalon::Date updateDate(updateTime);
		UInt64 curTime = Avalon::Time::CurrentTime().MSec();

		bool reset = false;
		switch(cycle)
		{
		case COUNTER_CYCLE_HOUR:
			{
				if(updateDate.Hour() != CURRENT_DATE.Hour()
					||updateDate.Day() != CURRENT_DATE.Day()
					||updateDate.Month() != CURRENT_DATE.Month()
					||updateDate.Year() != CURRENT_DATE.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_DAY:
			{
				Avalon::Date refreshDate = updateDate;
				refreshDate.Hour(hour);
				refreshDate.Min(0);
				refreshDate.Sec(0);
				Avalon::Time refreshTime = refreshDate.ToTime();
				if (updateTime.MSec() >= refreshTime.MSec())
				{
					refreshTime += Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
				}

				if (curTime >= refreshTime.MSec())
				{
					reset = true;
				}
			}
			break;
		case COUNTER_CYCLE_WEEK:
			{
				Avalon::Date date1(updateTime.MSec() - updateDate.WDay() * DAY_TO_SEC * 1000);
				Avalon::Date date2(CURRENT_TIME.MSec() - CURRENT_DATE.WDay() * DAY_TO_SEC * 1000);
				if(date1.Day() != date2.Day()
					||date1.Month() != date2.Month()
					||date1.Year() != date2.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_MONTH:
			{
				if(updateDate.Month() != CURRENT_DATE.Month()
					||updateDate.Year() != CURRENT_DATE.Year())
					reset = true;
			}
			break;
		case COUNTER_CYCLE_YEAR:
			{
				if(updateDate.Year() != CURRENT_DATE.Year())
					reset = true;
			}
			break;
		}

		return reset;
	}

	std::string GetTimeStr(Avalon::Date date) const 
	{
		char str[128];
		avalon_snprintf(str, sizeof(str), "%d-%d-%d %d:%d:%d", date.Year(), date.Month(), date.Day(), date.Hour(), date.Min(), date.Sec());
		return str;
	}

private:
	CounterCfgMap	m_CounterCfg;
};

#endif

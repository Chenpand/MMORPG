#include "TimerScript.h"

#include "WSPlayer.h"

void ScriptTimerHandler::OnTimeout(TimerScript* script, const Avalon::Time& now)
{
	InfoLogStream << "daily event(" << m_Func << ") start..." << LogStream::eos;
	script->Call<void>(m_Func.c_str());
}

TimerScript::TimerScript(Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
	,m_uGSNewVerTime(0)
{
}

TimerScript::~TimerScript()
{
}

void TimerScript::Init()
{
	Call<void>("Init", this);
}

void TimerScript::RegisterTimerEvent(UInt32 time, UInt32 interval, const char* func)
{
	UInt64 timeout = UInt64(time) * 1000;
	if(CURRENT_TIME.MSec() > timeout) return;
	m_Handlers.insert(ScriptTimerHandler(timeout, UInt64(interval) * 1000, func));
}

void TimerScript::RegisterHourEvent(UInt32 hourtime, const char* func)
{
	Avalon::Date date(CURRENT_TIME);
	date.Min(0);
	date.Sec(0);
	UInt64 timeout = (date.ToTime().Sec() + hourtime) * 1000;
	if(CURRENT_TIME.MSec() > timeout) timeout = timeout + HOUR_TO_SEC * 1000;
	m_Handlers.insert(ScriptTimerHandler(timeout, HOUR_TO_SEC * 1000, func));
}

void TimerScript::RegisterDailyEvent(UInt32 daytime, const char* func)
{
	Avalon::Date date(CURRENT_TIME);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	UInt64 timeout = (date.ToTime().Sec() + daytime) * 1000;
	if(CURRENT_TIME.MSec() > timeout) timeout = timeout + DAY_TO_SEC * 1000;

	InfoLogStream << "register daily event(" << func << ") timeout time(" << timeout << ")." << LogStream::eos;

	m_Handlers.insert(ScriptTimerHandler(timeout, DAY_TO_SEC * 1000, func));
}

void TimerScript::RegisterWeeklyEvent(UInt32 weekday, UInt32 daytime, const char* func)
{
	Avalon::Date date(CURRENT_TIME);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	UInt64 timeout = date.ToTime().MSec() - date.WDay() * DAY_TO_SEC * 1000 + weekday * DAY_TO_SEC * 1000 + daytime * 1000;
	if(CURRENT_TIME.MSec() > timeout) timeout = timeout + 7 * DAY_TO_SEC * 1000;
	m_Handlers.insert(ScriptTimerHandler(timeout, DAY_TO_SEC * 1000, func));
}

void TimerScript::SetGSNewVerTime(Int32 year, Int32 month, Int32 day)
{
	Avalon::Date	date;
	date.Year(year);
	date.Month(month);
	date.Day(day);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);

	m_uGSNewVerTime = UInt32(date.ToTime().Sec());
}

void TimerScript::OnTick(const Avalon::Time& now)
{
	std::vector<ScriptTimerHandler> handlerList;

	for(HANDLER_SET::iterator iter = m_Handlers.begin();iter != m_Handlers.end();)
	{
		if(now.MSec() <= iter->GetTimeout()) break;

		ScriptTimerHandler handler = *iter;
		m_Handlers.erase(iter++);

		handler.OnTimeout(this, now);

		if(handler.m_Interval > 0)
		{
			handler.m_Timeout = now.MSec() + handler.m_Interval;
			handlerList.push_back(handler);
		}
	}

	m_Handlers.insert(handlerList.begin(), handlerList.end());
}

void TimerScript::OnSelectCounterReturn(const char* key, ObjID_t playerId, const char* counter, UInt32 value)
{
	Call<void>("OnSelectCounterReturn", key, playerId, counter, value);
}

void TimerScript::OnActivityStart(const char *name)
{
	Call<void>("OnActivityStart", name);
}

void TimerScript::OnActivityEnd(const char* name)
{
	Call<void>("OnActivityEnd", name);
}

void TimerScript::OnVisitSortlist(const char *func, ObjID_t uId, ObjID_t uOwner, const char *szOwnerName, UInt32 ranking)
{
	Call<void>(func, uId, uOwner, szOwnerName, ranking);
}

void TimerScript::OnVisitWorldSortlist(const char* func, ObjID_t id, UInt32 ranking, UInt32 value1, UInt32 value2, UInt32 value3)
{
	Call<void>(func, id, ranking, value1, value2, value3);
}

void TimerScript::OnActivityPrepare(const char* name)
{
	Call<void>("OnActivityPrepare", name);
}


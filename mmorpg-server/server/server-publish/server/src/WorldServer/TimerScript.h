#ifndef _TIMER_SCRIPT_H_
#define _TIMER_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include <set>

class TimerScript;
class WSPlayer;

class ScriptTimerHandler
{
	friend class TimerScript;
public:
	ScriptTimerHandler(UInt64 timeout, UInt64 interval, const std::string& func):m_Timeout(timeout), m_Interval(interval), m_Func(func){}
	~ScriptTimerHandler(){}

	void OnTimeout(TimerScript* script, const Avalon::Time& now);

	UInt64 GetTimeout() const { return m_Timeout; }

private:
	//��ʱʱ��
	UInt64		m_Timeout;
	//��ʱ���
	UInt64		m_Interval;
	//�ص�����
	std::string	m_Func;
};

class ScriptTimerLess : public std::less<ScriptTimerHandler*>
{
public:
	bool operator()(const ScriptTimerHandler& left,const ScriptTimerHandler& right) const
	{
		return left.GetTimeout() < right.GetTimeout();
	}
};

/**
 *@brief ��ʱ���ű�
 */
class TimerScript : public Avalon::ScriptFile
{
	typedef std::multiset<ScriptTimerHandler,ScriptTimerLess> HANDLER_SET;
public:
	TimerScript(Avalon::ScriptState* state);
	~TimerScript();

public:
	/**
	 *@brief ��ʼ��
	 */
	void Init();

	/**
	 *@brief ע�ᶨʱ�¼�
	 */
	void RegisterTimerEvent(UInt32 time, UInt32 interval, const char* func);
	void RegisterHourEvent(UInt32 hourtime, const char* func);
	void RegisterDailyEvent(UInt32 daytime, const char* func);
	void RegisterWeeklyEvent(UInt32 weekday, UInt32 daytime, const char* func);

	/**
	*@brief ���á�����°汾�������а���ʱ��
	*/
	void SetGSNewVerTime(Int32 year, Int32 month, Int32 day);
	UInt32 GetGSNewVerTime() const { return m_uGSNewVerTime;}

public:
	/**
	 *@brief ��ʱ����
	 */
	void OnTick(const Avalon::Time& now);

public://�¼�
	/**
	 *@brief ��������ѯ�������ݷ���
	 */
	void OnSelectCounterReturn(const char* key, ObjID_t playerId, const char* counter, UInt32 value);

	/**
	*@brief ���ʼ
	*/
	void OnActivityStart(const char *name);

	/**
	 *@brief �����
	 */
	void OnActivityEnd(const char* name);

	/**
	*@brief �׼��
	*/
	void OnActivityPrepare(const char* name);

	/**
	*@brief �������а�
	*/
	void OnVisitSortlist(const char *func, ObjID_t uId, ObjID_t uOwner, const char *szOwnerName, UInt32 ranking);

	/**
	 *@brief ����ȫ�����а�
	 */
	void OnVisitWorldSortlist(const char* func, ObjID_t id, UInt32 ranking, UInt32 value1, UInt32 value2, UInt32 value3);

	/**
	*@brief ע���ϱ��ع�
	*/
	void RegVerteransReturnActivity(WSPlayer* player);

private:
	//��ʱ�¼��б�
	HANDLER_SET		m_Handlers;
	//��ǰʱ��
	Avalon::Time	m_Now;
	// ʹ���°汾���а���
	UInt32				m_uGSNewVerTime;
};

#endif

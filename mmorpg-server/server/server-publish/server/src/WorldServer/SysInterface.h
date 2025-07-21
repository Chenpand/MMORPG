#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>
#include <CLItemDefine.h>

class WSPlayer;
class WSScene;

class SysInterface
{
public:
	void LogError(const char* msg){ SYSLOG_ERROR(msg); }
	void LogInfo(const char* msg){ SYSLOG_INFO(msg); }
	void LogDebug(const char* msg){ SYSLOG_DEBUG(msg); }
	void LogTrace(const char* msg){ SYSLOG_TRACE(msg); }
	void LogFatal(const char* msg){ SYSLOG_FATAL(msg); }

	UInt64 Now(){ return CURRENT_TIME.MSec(); }

	void RegCounter(const char* name, Int32 cycle, UInt32 param = 0);

	Int32 Year(){ return CURRENT_DATE.Year(); }
	Int32 Month(){ return CURRENT_DATE.Month(); }
	Int32 Day(){ return CURRENT_DATE.Day(); }
	Int32 Hour(){ return CURRENT_DATE.Hour(); }
	Int32 Min(){ return CURRENT_DATE.Min(); }
	Int32 Sec(){ return CURRENT_DATE.Sec(); }
	Int32 WeekDay(){ return CURRENT_DATE.WDay(); }

	Int32 RandBetween(Int32 min, Int32 max){ return Avalon::Random::RandBetween(min, max); }

	UInt32 TimeOfDay();

	UInt32 MakeTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec);

	/**
	 *@brief ����һ������
	 */
	WSScene* FindScene(UInt32 id);
	
	/**
	 *@brief ѡ��һ��������̬�����ķ�����
	 */
	UInt32 SelectDynSceneServer();

	/**
	*@brief ������������
	*/
	void CreatePublicDynScene(UInt32 uMapId, UInt16 uLevel, UInt64 uParam, UInt32 uLastTime);

	/**
	 *@brief �㲥����
	 */
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);

	/**
	 *@brief ���ȼ��㲥����
	 */
	void BroadcastNotifyByLvl(UInt16 level, UInt16 type, UInt8 color, const char* word);

	/**
	@brief ϵͳ�ʼ����
	*/
	std::vector<ItemReward>* GetRewardGroup();
	void AddReward(std::vector<ItemReward> *pGroup, UInt32 uId, UInt32 uNum, UInt8 qualityLv, UInt8 strength);
	void SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards);
	void BroadcastSysMailOnline(const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards);

	/**
	 *@brief ����
	 */
	void ClearActivityRegInfos();
	void RegisterDailyActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOpenServerOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 lastTime, UInt32 openServerTime);
	void RegisterWeekActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterMonthActivity(UInt32 id, const char *name, UInt16 level);
	void RegisterCustomCircleActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, const UInt32 lastTime);
	void RegisterConsistActivity(UInt32 id, const char *name, UInt16 level);

	void RegisterGSActivity(UInt32 id, const char *name, UInt16 level, UInt32 lastTime);
	void RegisterGSActivityEndTime(UInt32 id, const char *name, UInt16 level, const char* endTime);
	void RegisterGSActivityIntervalTime(UInt32 id, const char *name, UInt16 level, const char* fromTime, const char* toTime);
	void RegisterGSActivityIntervalDay(UInt32 id, const char *name, UInt16 level, UInt16 fromDay, UInt16 toDay);
	
	void SetActGameStartLimit(UInt32 id, const char *gamestartTime, UInt8 gamestartFlag);
	void CloseActivity(const char* name);
	bool IsInActivity(const char *name);
	void AddGlobalActivity(const char *name);

	void SendAnnouncementNow(UInt32 id, UInt32 interval, UInt32 times);
	void SendAnnouncement(UInt32 id, const char* begintime, UInt32 interval, UInt32 times);
	void PublicAnnouncementList(UInt32 id, UInt32 durationTime, UInt32 intervalTime, UInt32 startTimeOffset = 0);
	void RemoveAnnouncement(UInt32 dataId);

	void SetActivityMonsterOpen(UInt8 open);

	/**
	*@brief ���ĳ�����а�
	*/
	void ClearSortList(UInt8 type);

	/**
	*@brief ����������а�
	*/
	void OnNewYearRedPacketEnd();

	/**
	 *@brief ��ѯ����ֵ
	 */
	void SelectCounterValue(const char* key, const char* counter, UInt32 minValue);

	/**
	*@brief �������а�
	*/
	void VisitSortList(UInt8 sortType, UInt32 uNum, const char *func);

	void VisitPlayer(const char* fun);

	/**
	*@brief ע��NPC
	*/
	void RegisterNpc(UInt32 npcId, UInt32 mapId, UInt16 x, UInt16 y);

	/**
	*@brief GameParam��������
	*/
	UInt32 GetGameValue(const char* name);
	void SetGameValue(const char* name, UInt32 value);

	/**
	*@brief ͬ���ȫ��������
	*/
	void SyncActivityCounter(UInt8 type, UInt32 value);

	void BroadcastEffectEvent(UInt8 type);

	/**
	*@brief �����ID
	*/
	UInt32 GetZoneId();
	
	UInt32 GetNeutralZoneId() const;

	/**
	*@brief ��ȡƽ̨
	*/
	const char* GetPlatform() const;

	/**
	*@brief ���ܿ���
	*/
	void OpenService(UInt32 type);
	void CloseService(UInt32 type);

	/**
	*@brief �����Զ�ս��ģʽ
	*/
	void SetAutoBattleMode(UInt8 mode);

	/**
	*@brief ����ս
	*/
	void ChangeGuildBattleStatus(UInt32 targetStatus);

	//��Ʒ�ϼ�
	void OnSaleMallWaitGoods(UInt32 goodId);
	
	//��Ʒ�¼�
	void OffSaleMallWaitGoods(UInt32 goodId);

	// �ٻ��������
	void SummonCityMonster();

	// �����Լ�
	void ChijiOpen(const char* name);

	// ��Ҽ��۽���
	void GoldConsignmentClose();

private:

};

extern SysInterface sys;

#endif

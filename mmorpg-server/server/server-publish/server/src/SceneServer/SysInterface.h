#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>

class Player;
class RewardGroup;
class Scene;
class NpcDialog;
class Npc;
class Creature;

class SysInterface
{
public:
	void LogError(const char* msg){ SYSLOG_ERROR(msg); }
	void LogInfo(const char* msg){ SYSLOG_INFO(msg); }
	void LogDebug(const char* msg){ SYSLOG_DEBUG(msg); }
	void LogTrace(const char* msg){ SYSLOG_TRACE(msg); }
	void LogFatal(const char* msg){ SYSLOG_FATAL(msg); }

	UInt64 Now(){ return CURRENT_TIME.MSec(); }
	bool IsSameDay(UInt32 time1, UInt32 time2);

	void RegCounter(const char* name, Int32 cycle, UInt32 param = 0);

	Int32 Year(){ return CURRENT_DATE.Year(); }
	Int32 Month(){ return CURRENT_DATE.Month(); }
	Int32 Day(){ return CURRENT_DATE.Day(); }
	Int32 Hour(){ return CURRENT_DATE.Hour(); }
	Int32 Min(){ return CURRENT_DATE.Min(); }
	Int32 Sec(){ return CURRENT_DATE.Sec(); }
	Int32 WeekDay(){ return CURRENT_DATE.WDay(); }

	bool SelectInHundred(Int32 value){ return Avalon::Random::SelectInHundred(value); }
	bool SelectInThousand(Int32 value){ return Avalon::Random::SelectInThousand(value); }
	bool SelectInTenThousand(Int32 value){ return Avalon::Random::SelectInTenThousand(value); }
	bool SelectInHundredThousand(Int32 value){ return Avalon::Random::SelectInHundredThousand(value); }

	Int32 RandBetween(Int32 min, Int32 max){ return Avalon::Random::RandBetween(min, max); }

	void Notify(ObjID_t id, UInt16 type, UInt8 color, const char* word);
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);

	/**
	 *@brief ���а�ֵ�仯
	 */
	void OnSortValueChanged(UInt8 sortType, ObjID_t id, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3);
	void OnWorldSortValueChanged(Player* player, UInt8 sortType, UInt32 value1, UInt32 value2, UInt32 value3);
	
	RewardGroup* GetRewardGroup();
	void SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards);

	/**
	 *@brief �ж��Ƿ��ڻ��
	 */
	bool IsInActivity(const char* name);
	
	/**
	 *@brief ��ȡ��ͼ��
	 */
	const char* GetMapName(UInt32 mapId);

	/**
	 *@brief ��ȡ�������
	 */
	UInt8 GetRandDirection();

	/**
	 *@brief ��ȡ�������� 
	 */
	UInt32 GetBaseExp(UInt16 level);

	/**
	 *@brief ��ȡָ���ȼ���������
	 */
	UInt32 GetBaseSilver(UInt16 level);

	/**
	*@brief �������ȼ�
	*/
	UInt32 GetWorldLevel();


	void BroadcastEffectEvent(UInt8 type);


	UInt32 GetZoneId();

	void SendOfflineNotify(const char* type, const char* word);

	/**
	*@brief ͬ������ֵ�����
	*/
	void SyncCounter(Player *pPlayer, const char* szCounterName);

	UInt32 GetNeutralZoneId() const;

	/**
	 *@brief ��ȡ�������������                                                                     
	 */
	UInt32 GetNeutralRandMajorCity() const;

public:
	// ����������

	/**
	*@brief �����ˢ��
	*/
	void GenActivityCityMonster();

	/**
	*@brief �����ˢ��
	*/
	void RemoveTaskCityMonster(ObjID_t owner, const char* idstr);

	//��ȡ�����һ�����
	UInt32 GetEnergyExchangeExp(UInt32 playerLev);
};

extern SysInterface sys;

#endif

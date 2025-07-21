#ifndef _FATIGUE_H_
#define _FATIGUE_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>

class Player;

/**
*@brief 疲劳值
*/
class Fatigue : public CLOwnerComplexProperty
{

public:
	Fatigue();
	~Fatigue() {};

public:

	void SetOwner(Player* owner);
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 获取疲劳值上限
	*/
	ObjUInt16& GetMaxFatigue() { return m_MaxFatigue; }

	/**
	*@brief 设置疲劳值上限
	*/
	void SetMaxFatigue(UInt16 num);

	/**
	*@brief 获取已经使用的疲劳值
	*/
	UInt16 GetUseFatigue() { return m_UseFatigue; }

	/**
	*@brief 获取积累的疲劳值
	*/
	UInt32 GetLastFatigue();

	/**
	*@brief 获取现在缺少的疲劳值
	*/
	UInt16 GetLostFatigue() { return m_MaxFatigue - m_Fatigue; }

	/**
	*@brief 获取当前疲劳值
	*/
	ObjUInt16& GetFatigue() { return m_Fatigue; }

	/**
	*@brief 增加疲劳值
	*/
	void AddFatigue(const char* reason, UInt16 fatigue, bool notify = false);

	/**
	*@brief 消耗疲劳值
	*/
	void RemoveFatigue(const char* reason, UInt16 fatigue);

	/**
	*@brief 刷新疲劳值
	*/
	void RefreshFatigue(const Avalon::Time& now);

	/**
	*@brief 获取最后刷新时间
	*/
	Avalon::Time GetLastRefreshTime() { return m_LastRefreshTime; }

public:
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void OnHeartbeat(const Avalon::Time& now, int tickType);

	void OnBirth();

	void OnOnline(bool bDayChanged);

private:

	//拥有者
	Player* m_pOwner;

	//最大值
	ObjUInt16 m_MaxFatigue;

	//已经使用消耗的
	UInt16 m_UseFatigue;

	//现在拥有的
	ObjUInt16 m_Fatigue;

	//上次刷新时间
	Avalon::Time m_LastRefreshTime;

};

#endif
#ifndef _COUNTER_MGR_H_
#define _COUNTER_MGR_H_

#include <CLDefine.h>
#include <CLGameDefine.h>
#include <CLCounterDefine.h>
#include <CLRecordCallback.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <map>
#include <CLItemDefine.h>
#include <CLCounterNameDefine.h>

/**
 *@brief 计数
 */
struct Counter
{
	Counter():id(0), updateTime(0), value(0), dirty(false) { }

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & updateTime & value;
	}

	//主键
	ObjID_t	id;
	//更新时间
	UInt32	updateTime;
	//值
	UInt32	value;
    // 脏标记
    bool dirty;
};

class Player;

/**
 *@brief 计数管理器
 */
class CounterMgr : public CLOwnerComplexProperty
{
	
	typedef std::map<std::string, Counter>	CounterMap;

public:
	CounterMgr();
	~CounterMgr();

public:
	/**
	 *@brief 设置获取计数值
	 */
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

	/**
	 *@bref 增加，获取
	 */
	UInt32	IncCounter(const char* name, UInt32 value);
	UInt32	IncCounterDynamic(const char* preName, UInt32 value, std::vector<UInt32>& suffix);
	UInt32	GetCounter(const char* name);
	UInt32  GetCounterDynamic(const char* preName, std::vector<UInt32>& suffix);
	UInt32	GetCounterWithoutCycle(const char* name);
	void	SetCounter(const char* name, UInt32 value);
	void	SetCounterDynamic(const char* preName, UInt32 value, std::vector<UInt32>& suffix);
	UInt32  GetCounterUpdateTime(const char* name);
	void	RefreshCounterUpdateTime(const char* name);

public://事件
	void OnOnline(bool isDayChanged);
    void OnTick(const Avalon::Time& now);
	void OnDayChanged();
	void OnLogicDayChanged();
	/**
	 *@brief 查询数据返回
	 */
	void OnSelectDataRet(CLRecordCallback* callback);

private://数据库相关
	void InsertToDB(const std::string& name, const Counter& counter);
	void SaveToDB(const Counter& counter);

	void RefreshCounter();

public://编解码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Counters;
	}

    virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
private:
	//拥有者
	Player*	m_pOwner;
	//计数
	CounterMap	m_Counters;
};


#endif

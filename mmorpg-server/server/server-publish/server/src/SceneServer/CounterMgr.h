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
 *@brief ����
 */
struct Counter
{
	Counter():id(0), updateTime(0), value(0), dirty(false) { }

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & updateTime & value;
	}

	//����
	ObjID_t	id;
	//����ʱ��
	UInt32	updateTime;
	//ֵ
	UInt32	value;
    // ����
    bool dirty;
};

class Player;

/**
 *@brief ����������
 */
class CounterMgr : public CLOwnerComplexProperty
{
	
	typedef std::map<std::string, Counter>	CounterMap;

public:
	CounterMgr();
	~CounterMgr();

public:
	/**
	 *@brief ���û�ȡ����ֵ
	 */
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

	/**
	 *@bref ���ӣ���ȡ
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

public://�¼�
	void OnOnline(bool isDayChanged);
    void OnTick(const Avalon::Time& now);
	void OnDayChanged();
	void OnLogicDayChanged();
	/**
	 *@brief ��ѯ���ݷ���
	 */
	void OnSelectDataRet(CLRecordCallback* callback);

private://���ݿ����
	void InsertToDB(const std::string& name, const Counter& counter);
	void SaveToDB(const Counter& counter);

	void RefreshCounter();

public://�����
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Counters;
	}

    virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
private:
	//ӵ����
	Player*	m_pOwner;
	//����
	CounterMap	m_Counters;
};


#endif

#ifndef __MONOPOLY_H__
#define __MONOPOLY_H__

#include <vector>

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>
#include <CLRecordClient.h>
#include <CLMonopolyDataEntry.h>

class WSPlayer;

class AccountInfo;

/**
*@brief �����������
*/
enum MonopolyRandomType
{
	MRT_ALL = 0,
	MRT_RANDOM_ONE = 1,
};

/**
*@brief �����¼�����
*/
enum MonopolyGridEventType
{
	MGET_ADD_ITEM = 0, //���ӵ����¼�
	MGET_ADD_COIN = 1, // ���Ӳ��
	MGET_BUFF = 2, //���buff
	MGET_MISSION = 3, //����ؿ�
	MGET_RANDOM_CARD = 4, //���ף����
	MGET_ADD_ROLL = 5, //�������Ӵ���
	MGET_NULL = 6, //���¼�
};

class MonopolyGridEvent
{
public:
	MonopolyGridEvent() = default;
	virtual ~MonopolyGridEvent() = default;
	virtual void OnTriged(WSPlayer* player) = 0;
	UInt32 GetWeight() const { return m_Weight; }
	static MonopolyGridEvent* GenEvent(MonopolyGridEventDataEntry* data);
	virtual void SendResult(WSPlayer* player, UInt32 result = 0);
protected:
	UInt32 m_Id = 0;
	UInt32 m_Weight = 0;
	UInt32 m_Param = 0;
	UInt32 m_Num = 0;
};

//���ӵ����¼�
class AddItemMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//���Ӳ��
class AddCoinMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//���buff
class AddBuffMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//����ؿ�
class JoinMissionMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//���ף����
class RandomCardMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
	virtual void SendResult(WSPlayer* player, UInt32 result = 0);
};

//�������Ӵ���
class AddRollMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player);
};

//���¼�
class NullMonopolyEvent : public MonopolyGridEvent
{
	virtual void OnTriged(WSPlayer* player) {};
};


class MonopolyMapGrid
{
public:
	MonopolyMapGrid() = default;
	virtual ~MonopolyMapGrid();
	virtual void OnTriged(WSPlayer* player) = 0;
	/**
	*@brief �����������ɸ���
	*/
	static MonopolyMapGrid* GenGrid(UInt32 gridType);
protected:
	std::vector<MonopolyGridEvent*> m_Event;
	UInt32 m_TotalWeight = 0;
};

//�����е������¼���Ҫ����
class AllMonopolyMapGrid : public MonopolyMapGrid
{
	virtual void OnTriged(WSPlayer* player);
};

//���Ӱ�Ȩ���������һ���¼�
class RandomMonopolyMapGrid : public MonopolyMapGrid
{
	virtual void OnTriged(WSPlayer* player);
};





using MonopolyMap = std::vector<MonopolyMapGrid*>;

#define MONOPOLY_INFO(info) "accid(" <<info->GetAccid() << ")"

class MonopolyInfo : public CLSyncObject
{
public:
	MonopolyInfo();
	~MonopolyInfo();


	void Reset();
	/**
	*@brief ���ݵ�ͼ���ó�ʼ����ͼ
	*/
	void InitMap();
	void OnTick(const Avalon::Time &now);
	void OnWeekChange();
	void OnDayChanged();
	void ResetMap();
	void LoadData();
	void SetData(CLRecordCallback* callback);
	void InsertToDB();
	void OnOnline();
	void OnOffline();
	inline void SetLoaded() { m_IsLoaded = true; }

	void SendStatus();
	void SendCardList();
	void SendCoin();

public:
	/**
	*@brief ��ǰ�߼���
	*/
	UInt32 ForwardToMap(UInt32 cnt);
	/**
	*@brief ���rollһ��ɸ��
	*/
	UInt32 OnOwnerRollOnce(UInt32& rolledNum);

	/**
	*@brief ����ǰ������
	*/
	UInt32 GetForwardNum();

	/**
	*@brief ����ĳ�ſ���ָ������
	*/
	UInt32 AddCard(UInt32 id, UInt32 num);

	/**
	*@brief ����ĳ�ſ���ָ������
	*/
	UInt32 RemoveCard(UInt32 id, UInt32 num);

	/**
	*@brief ��ȡĳ�ſ�������
	*/
	UInt32 GetCardNum(UInt32 id);

	/**
	*@brief ���ӿ��ƶһ�����
	*/
	UInt32 AddCardExchange(UInt32 id, UInt32 num);

	/**
	*@brief ���ٿ��ƶһ�����
	*/
	UInt32 RemoveCardExchange(UInt32 id, UInt32 num);

	/**
	*@brief ��ȡ���ƶһ�����
	*/
	UInt32 GetCardExchange(UInt32 id);

	/**
	*@brief ���ӹؿ�Ȩ��
	*/
	void AddMissionLimit(UInt32 id);

	/**
	*@brief �ܷ����ؿ�
	*/
	bool CanJoinMission(UInt32 id);


	std::map<UInt32, UInt32>& GetCards();
	std::map<UInt32, UInt32>& GetCardExchangeList();

	void SaveCards();
	void LoadCards();

	void SaveCardExchange();
	void LoadCardExchange();
	
private:

	/**
	*@brief ��ȡ������id
	*/
	UInt8 GetMaxGridID();

public:
	void SetOwner(AccountInfo* info);
	AccountInfo* GetOwner();
	WSPlayer* GetPlayer();
	inline UInt32 GetAccid() const{ return m_Accid; }
	inline void SetCurrnetStep(UInt32 num) { m_Step = num; }
	inline UInt32 GetCurrnetStep() const { return m_Step; }
	void SetRemainRoll(UInt32 times, std::string reason = "");
	inline UInt32 GetVsersion() const { return m_Vsersion; }
	inline void SetVsersion(UInt32 vsersion) { m_Vsersion = vsersion; }
	inline UInt32 GetRemainRoll() const { return m_RemainRoll; }
	inline void SetRolledTimes(UInt32 times) { m_RolledTimes = times; }
	inline UInt32 GetRolledTimes() const { return m_RolledTimes; }
	void SetCoin(UInt32 num, std::string reason = "");
	inline UInt32 GetCoin() const { return m_Coin; }
	void SetBuff(UInt32 num);
	inline UInt32 GetBuff() const { return m_Buff; }
	inline UInt32 GetTurn() const { return m_Turn; }
	UInt32 GetFakeTurn();
	inline void SetTurn(UInt32 num) { m_Turn = num; }
public:

	void SaveToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("vsersion", m_Vsersion)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("turn", m_Turn)
		CL_DBPROPERTY("step", m_Step)
		CL_DBPROPERTY("remain_roll", m_RemainRoll)
		CL_DBPROPERTY("roll_times", m_RolledTimes)
		CL_DBPROPERTY("coin", m_Coin)
		CL_DBPROPERTY("buff", m_Buff)
		CL_DBPROPERTY("cards", m_Cards)
		CL_DBPROPERTY("exchanged", m_Exchanged)
	CL_DBPROPERTY_END()

private:
	//�˺�ID
	ObjUInt32 m_Vsersion;
	//Ȧ��
	ObjUInt32 m_Turn;
	//�˺�ID
	ObjUInt32 m_Accid;
	//��ǰ����
	ObjUInt32 m_Step;
	//ʣ�����Ӵ���
	ObjUInt32 m_RemainRoll;
	//��ҡ���Ӵ���
	ObjUInt32 m_RolledTimes;
	//Ӳ��
	ObjUInt32 m_Coin;
	//BUFF
	ObjUInt32 m_Buff;
	//�������
	ObjString m_Cards;
	//�Ƿ��ѽ������������
	ObjString m_Exchanged;
	//�Ƿ����ύ�������
	ObjUInt8 m_IsCommitedCard;
private:
	AccountInfo* m_Owner;
	// �������ݿⶨʱ��
	Avalon::SimpleTimer	m_SaveTimer;
	// ���̵�ͼ
	MonopolyMap m_Map;
	// �����ڴ�ṹ ����id -> ��������
	std::map<UInt32, UInt32> m_CardsMap;
	// ���ƶһ��ڴ�ṹ �һ���id -> �һ�����
	std::map<UInt32, UInt32> m_CardExchangeMap;

	//���̹ؿ�id
	UInt32			m_MissionID = 0;
	//���̹ؿ���ʱ��
	Avalon::SimpleTimer	m_MissionTimer;
	//���ر��
	bool			m_IsLoaded = false;
};





#endif
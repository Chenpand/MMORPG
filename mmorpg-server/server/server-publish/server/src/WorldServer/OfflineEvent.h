#ifndef _OFFLINE_EVENT_H_
#define _OFFLINE_EVENT_H_

#include <CLOfflineEventDefine.h>
#include <CLRecordCallback.h>
#include <CLChatDefine.h>

#define DEFINE_OFFLINEEVENT_TYPE(Type) \
	public: \
		const static UInt16	TYPE = Type; \
		UInt16 GetType() const{ return TYPE; } \
	private:

class WSPlayer;

/**
 *@brief �����¼�
 */
class OfflineEvent
{
public:
	OfflineEvent();
	virtual ~OfflineEvent();

public:
	/**
	 *@brief ��ȡ����
	 */
	virtual UInt16 GetType() const = 0;

	/**
	 *@brief ����һ��
	 */
	static OfflineEvent* Create(UInt16 type);

	/**
	 *@brief ����
	 */
	virtual void OnTriggered(WSPlayer* player) = 0;

	void SetTarUid(ObjID_t tarUid)	{ m_tarUid = tarUid; }
	ObjID_t GetTarUid()	{ return m_tarUid; }

public: //�����
	virtual std::string Encode() const = 0;
	virtual void Decode(const std::string& str) = 0;


public: //���ݿ����
	void SetDBData(CLRecordCallback* callback);
	void InsertToDB(ObjID_t id);

protected:
	//����ʱ��
	UInt32	m_Time;
	//������uid
	ObjID_t m_tarUid;
};

//������ �۳�
class SellAuctionEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_SELL_AUCTION)
public:
	SellAuctionEvent(){ m_Money = 0; }
	SellAuctionEvent(UInt32 money) : m_Money(money) {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	UInt32 m_Money;

};

//���빤��
class JoinGuildEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_JOIN_GUILD)

public:
	JoinGuildEvent() {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

};

//�������߰�ʦ
class RequestMasterEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_REQUEST_MASTER)
public:
	RequestMasterEvent() {}
	RequestMasterEvent(ObjID_t uid, const std::string& Name, UInt8 occu, UInt16 lv, UInt8 vipLv)
		:m_uid(uid), m_Name(Name), m_Occu(occu), m_Lv(lv), m_vipLv(vipLv) {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	ObjID_t		m_uid;
	std::string	m_Name;
	UInt8		m_Occu;
	UInt16		m_Lv;
	UInt8		m_vipLv;
};

//����������ͽ
class RequestDiscipleEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_REQUEST_DISCILPE)
public:
	RequestDiscipleEvent() {}
	RequestDiscipleEvent(ObjID_t uid, const std::string& Name, UInt8 occu, UInt16 lv, UInt8 vipLv)
		:m_uid(uid), m_Name(Name), m_Occu(occu), m_Lv(lv), m_vipLv(vipLv) {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	ObjID_t		m_uid;
	std::string	m_Name;
	UInt8		m_Occu;
	UInt16		m_Lv;
	UInt8		m_vipLv;
};

//�����������
class InviteFriendEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_INVITE_FRIEND)
public:
	InviteFriendEvent() {}
	InviteFriendEvent(ObjID_t uid, const std::string& Name, UInt8 occu, UInt16 lv, UInt8 vipLv)
		:m_uid(uid), m_Name(Name), m_Occu(occu), m_Lv(lv), m_vipLv(vipLv) {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	ObjID_t		m_uid;
	std::string	m_Name;
	UInt8		m_Occu;
	UInt16		m_Lv;
	UInt8		m_vipLv;
};

//���߷���˽��
class SendPrivateChatEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_PRIVATE_CHAT)
public:
	SendPrivateChatEvent() {}
	SendPrivateChatEvent(ChatDataInfo* chatData):m_ChatData(chatData) {}
	~SendPrivateChatEvent()	{
		if (m_ChatData){
			CL_SAFE_DELETE(m_ChatData);
		}
	}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	ChatDataInfo* m_ChatData;
};

//�������ߺ���
class ReplyFriendEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_REPLY_FRIEND)
public:
	ReplyFriendEvent() {}
	ReplyFriendEvent(ObjID_t uid, const std::string& Name, UInt8 occu, UInt16 lv)
		:m_uid(uid), m_Name(Name), m_Occu(occu), m_Lv(lv) {}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	ObjID_t		m_uid;
	std::string	m_Name;
	UInt8		m_Occu;
	UInt16		m_Lv;
};

class ImpeachFailedForLeaveEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_IMPEACHFAILED_FORLEAVE)
public:
	ImpeachFailedForLeaveEvent(){}
	ImpeachFailedForLeaveEvent(const std::string& tribeName)
		:m_TribeName(tribeName){}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const{ return m_TribeName; }
	void Decode(const std::string& str){ m_TribeName = str; }

private:
	std::string	m_TribeName;
};

class ImpeachFailedForOnlineEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_IMPEACHFAILED_FORONLINE)
public:
	ImpeachFailedForOnlineEvent(){}
	ImpeachFailedForOnlineEvent(UInt32 time)
		:m_OnlineTime(time){}

	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const;
	void Decode(const std::string& str);

private:
	UInt32	m_OnlineTime;
};

class BeImpeachedEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_BEIMPEACHED)
public:
	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const{ return ""; }
	void Decode(const std::string& str){}
};

class BeTribeLeaderEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_BETRIBELEADER)
public:
	BeTribeLeaderEvent(){}
	BeTribeLeaderEvent(const std::string& tribeName):m_TribeName(tribeName){}
public:
	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const{ return m_TribeName; }
	void Decode(const std::string& str){ m_TribeName = str; }

private:
	std::string m_TribeName;
};


class FireTribeEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_FIRETRIBE)
public:
	FireTribeEvent(){}
	FireTribeEvent(const std::string& tribeName):m_TribeName(tribeName){}
public:
	void OnTriggered(WSPlayer* player);
public:
	std::string Encode() const{ return m_TribeName; }
	void Decode(const std::string& str){ m_TribeName = str; }

private:
	std::string m_TribeName;
};

class IncIntimacyEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_INC_INTIMACY)
public:
	IncIntimacyEvent() {}
	IncIntimacyEvent(ObjID_t uFriendId, UInt16 uIntimacy)
		: m_uFriendId(uFriendId)
		, m_uIntimacy(uIntimacy)
	{
	}
	~IncIntimacyEvent() {}
public:
	void OnTriggered(WSPlayer *pPlayer);
public:
	std::string Encode() const;
	void Decode(const std::string &str);

private:
	ObjID_t	m_uFriendId;
	UInt16	m_uIntimacy;
};

class AddMallItemBuyCount : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_EVENT_ADD_MALLITEM_BUYCOUNT)
public:
	AddMallItemBuyCount() : m_uItemId(0), m_uNum(0) {}
	AddMallItemBuyCount(UInt32 uItemId, UInt16 uNum)
		: m_uItemId(uItemId), m_uNum(uNum){}
	
public:
	void OnTriggered(WSPlayer *pPlayer);

public://�����
	std::string Encode() const;
	void Decode(const std::string &str);

private:
	// ��Ʒid
	UInt32	m_uItemId;
	// ��������
	UInt32	m_uNum;
};

class TitleEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_GET_TITLE)
public:
	TitleEvent() : m_TitleId(0), m_ExpireTime(0) {}
	TitleEvent(UInt32 titleId, UInt32 expireTime)
		: m_TitleId(titleId), m_ExpireTime(expireTime) {}

public:
	void OnTriggered(WSPlayer *pPlayer);

public://�����
	std::string Encode() const;
	void Decode(const std::string &str);

private:
	// ͷ��id
	UInt32 m_TitleId;
	// ����ʱ��
	UInt32 m_ExpireTime;
};

class GoldConsignmentPointEvent : public OfflineEvent
{
	DEFINE_OFFLINEEVENT_TYPE(OFFLINE_GOLD_CONSIGNMENT_POINT)
public:
	GoldConsignmentPointEvent() : m_redPointTime(0) {}
	GoldConsignmentPointEvent(UInt32 redTime) : m_redPointTime(redTime) {}

public:
	void OnTriggered(WSPlayer *pPlayer);

public://�����
	std::string Encode() const;
	void Decode(const std::string &str);

private:
	// ���ʱ��
	UInt32 m_redPointTime;
};

#endif

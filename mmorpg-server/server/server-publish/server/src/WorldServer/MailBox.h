#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include <functional>
#include <AvalonSimpleTimer.h>
#include <CLMailDefine.h>
#include <string>
#include "WSItem.h"

class WSPlayer;
class SelectMailsCallback;
class SelectMailItemsCallback;

/**
 *@brief �ʼ�
 */
struct Mail
{
	Mail();
	~Mail();

	//������Ϣ
	MailTitleInfo	titleInfo;
	//������id
	ObjID_t			senderId;
	//�������˺�id
	UInt32			senderAccid;
	//����
	std::string		content;
	//��Ԫ
	UInt32			silver;
	//�ʼ���Դ
	std::string		reason;
	//�򵥵���
	std::vector<ItemReward>	items;
	//��ϸ����
	std::vector<WSItem*>	detailItems;
};

/**
 *@brief �����ʼ��ص�                                                                     
 */
class SendMailCallback
{
public:
	enum ErrorCode
	{
		MAIL_ERROR_NOPLAYER = 1,		//û�и����
		MAIL_ERROR_TOOMANYITEMS = 2,	//����̫��
		MAIL_ERROR_TEXT = 3,			//���ִ���
	};

public:
	SendMailCallback(int refcount):m_refCount(refcount){ if(m_refCount <= 0) m_refCount = 1; }
	virtual ~SendMailCallback(){}

	void SetResult(ObjID_t receiverId, int errcode);

protected:
	virtual void OnSucceed(ObjID_t receiverId) = 0;
	virtual void OnFailed(ObjID_t receiverId, int errcode) = 0;
	virtual void OnFinished() = 0;

private:
	int m_refCount;
};

/** 
 *@brief ����
 */
class MailBox
{
	friend class SelectRoleCallback;
	friend class SysSelectRoleCallback;
	friend class DailyClearCounterHandler;

	typedef std::list<Mail*> MailList;
	typedef std::map<ObjID_t,Mail*>	MailMap;
	typedef HashMap<ObjID_t, UInt32> CountMap;

public:
	/*
	// Ų�� CLMailDefine.h��,��Ϊ��scene��Ҳ��Ҫ���. modified by huchenhui
	//��󸽼���
	const static UInt32	MAX_MAILITEM_NUM = 5;
	//�ʼ�����
	const static UInt32 MAIL_COST =	50;
	//���⺺����
	const static Int32	MAX_TITLE_SHOW_LEN = 20;
	//���ݺ�����
	const static Int32	MAX_CONTENT_SHOW_LEN = 200;
	//�����˺�����
	const static UInt32 MAX_SENDER_NAME_LEN = 15;
	//�ʼ������Ч��
	const static UInt32 MAIL_VALID_TIME = (30 * DAY_TO_SEC);
	*/

public:
	MailBox();
	~MailBox();

public:
	/**
	 *@brief ȫ�ֳ�ʼ��
	 */
	static void Init();

	/**
	 *@brief ����ӵ����
	 */
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const { return m_pOwner; }

public:
	/** 
	 *@brief �����ʼ����ڴ����û�ʱ WSPlayer::LoadData()������
	 */
	void LoadMails();

	/**
	*@brief �����ʼ�
	*/
	void VisitMails(const std::function<bool(Mail*)>& F);

	/**
	 *@brief �����ʼ��б�
	 */
	void ListMails();
	//void ListMails(UInt8 type, UInt16 start, UInt16 count);

	/**
	 *@brief ��ȡһ���ʼ�
	 */
	void ReadMail(ObjID_t id);

	/** 
	 *@brief ��ȡ�����ʼ�����
	 */
	void GetMailItems(ObjID_t id);

	/**
	*@brief һ����ȡ�ʼ�����
	*/
	void GetMailItems(UInt32 mailType);

	/**
	 *@brief ����һ���ʼ�
	 */
	void SendMail(const std::string& receiver, const std::string& title, const std::string& content, UInt32 silver
		, const std::map<ObjID_t,UInt16>& items);
	
	/** 
	 *@brief �ظ�һ���ʼ�
	 */
	void ReplyMail(ObjID_t id, const std::string& title, const std::string& content, UInt32 silver
		, const std::map<ObjID_t,UInt16>& items);

	/**
	 *@brief ����ϵͳ�ʼ� ����Ʒ
	 */
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief ����ϵͳ�ʼ� ��ϸ��Ʒ
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, std::vector<WSItem*>& detailitems, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief ����ϵͳ�ʼ� ����Ʒ+��ϸ��Ʒ
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	static UInt32 SendSysMailGetMailID(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt64& mailId, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief ����ϵͳ�ʼ� ����Ʒ �лص�
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, SendMailCallback* callback, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	static void SendSysMailByAccId(UInt32 accId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	 *@brief �㲥����ϵͳ�ʼ�
	 */
	static void BroadcastSysMailOnline(const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	 *@brief ɾ���ʼ�
	 */
	void DeleteMails(const std::vector<ObjID_t>& mailids);

	/**
	 *@brief �����������
	 */
	static bool CheckWord(const std::string& title, const std::string& content);

public://�¼�
	/**
	 *@brief ����
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	void OnOnline();

	/**
	 *@brief �����ʼ�����
	 */
	void OnLoadMailsRet(SelectMailsCallback* callback);
	/**
	 *@brief �����ʼ���������
	 */
	void OnLoadMailItemsRet(SelectMailItemsCallback* callback);
	/**
	 *@brief ����������
	 */
	void OnCheckPackageRet(const std::vector<ObjID_t>& mailids);
	/**
	 *@brief �۳����ķ���
	 */
	void OnRemoveCostRet(ObjID_t receiveId, UInt32 accid, const std::string& receiveName, UInt8 replyTimes, const std::string& title
		, const std::string& content, UInt32 silver, std::vector<WSItem*>& items);

private:
	/**
	 *@brief ��ѯ�ʼ�
	 */
	Mail* FindMail(ObjID_t id);

	/**
	 *@brief ͬ���ʼ�״̬
	 */
	void SyncMailStatus(Mail* mail);

	/**
	 *@brief ͬ��δ���ʼ���
	 */
	void SyncUnreadMailNum();

	/**
	 *@brief ����ʼ�����
	 */
	void CheckMailCost(ObjID_t receiver, UInt32 accid, const std::string& receiverName, UInt8 replyTimes, const std::string& title
		, const std::string& content, UInt32 silver
		, const std::map<ObjID_t,UInt16>& items);

	void PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime);
	
	void PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, std::vector<WSItem*>& items, UInt32 validDay, UInt32 sendtime);

	void PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, std::vector<WSItem*>& detailItems, UInt32 validDay, UInt32 sendtime);

	static void SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
		const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, UInt32 sendtime, UInt32 deadline);
	
	static void SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
		const std::string& content, const std::string& reason, const std::vector<WSItem*>& items, UInt32 sendtime, UInt32 deadline);

	static void SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
		const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, const std::vector<WSItem*>& detailItems, UInt32 sendtime, UInt32 deadline);

	bool PutMail(Mail* mail);

private:
	//ӵ����
	WSPlayer*	m_pOwner;
	//�ʼ��б� ����Mail*���б�
	MailList	m_Mails;
	//�ʼ�map�� ��Mail�� guidΪkey
	MailMap		m_MailMap;
	//���Ŷ�ʱ��
	Avalon::SimpleTimer m_Timer;

	//ÿ�շ��ʼ�����
	static CountMap	m_SendCounts;
};

#endif

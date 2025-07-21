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
 *@brief 邮件
 */
struct Mail
{
	Mail();
	~Mail();

	//主题信息
	MailTitleInfo	titleInfo;
	//发件人id
	ObjID_t			senderId;
	//发件人账号id
	UInt32			senderAccid;
	//内容
	std::string		content;
	//银元
	UInt32			silver;
	//邮件来源
	std::string		reason;
	//简单道具
	std::vector<ItemReward>	items;
	//详细道具
	std::vector<WSItem*>	detailItems;
};

/**
 *@brief 发送邮件回调                                                                     
 */
class SendMailCallback
{
public:
	enum ErrorCode
	{
		MAIL_ERROR_NOPLAYER = 1,		//没有该玩家
		MAIL_ERROR_TOOMANYITEMS = 2,	//附件太多
		MAIL_ERROR_TEXT = 3,			//文字错误
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
 *@brief 邮箱
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
	// 挪到 CLMailDefine.h里,因为在scene中也需要检查. modified by huchenhui
	//最大附件数
	const static UInt32	MAX_MAILITEM_NUM = 5;
	//邮件花费
	const static UInt32 MAIL_COST =	50;
	//标题汉字数
	const static Int32	MAX_TITLE_SHOW_LEN = 20;
	//内容汉字数
	const static Int32	MAX_CONTENT_SHOW_LEN = 200;
	//发件人汉字数
	const static UInt32 MAX_SENDER_NAME_LEN = 15;
	//邮件最大有效期
	const static UInt32 MAIL_VALID_TIME = (30 * DAY_TO_SEC);
	*/

public:
	MailBox();
	~MailBox();

public:
	/**
	 *@brief 全局初始化
	 */
	static void Init();

	/**
	 *@brief 设置拥有者
	 */
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const { return m_pOwner; }

public:
	/** 
	 *@brief 加载邮件，在创建用户时 WSPlayer::LoadData()被调用
	 */
	void LoadMails();

	/**
	*@brief 访问邮件
	*/
	void VisitMails(const std::function<bool(Mail*)>& F);

	/**
	 *@brief 请求邮件列表
	 */
	void ListMails();
	//void ListMails(UInt8 type, UInt16 start, UInt16 count);

	/**
	 *@brief 读取一封邮件
	 */
	void ReadMail(ObjID_t id);

	/** 
	 *@brief 获取单个邮件附件
	 */
	void GetMailItems(ObjID_t id);

	/**
	*@brief 一键获取邮件附件
	*/
	void GetMailItems(UInt32 mailType);

	/**
	 *@brief 发送一封邮件
	 */
	void SendMail(const std::string& receiver, const std::string& title, const std::string& content, UInt32 silver
		, const std::map<ObjID_t,UInt16>& items);
	
	/** 
	 *@brief 回复一封邮件
	 */
	void ReplyMail(ObjID_t id, const std::string& title, const std::string& content, UInt32 silver
		, const std::map<ObjID_t,UInt16>& items);

	/**
	 *@brief 发送系统邮件 简单物品
	 */
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief 发送系统邮件 详细物品
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, std::vector<WSItem*>& detailitems, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief 发送系统邮件 简单物品+详细物品
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	static UInt32 SendSysMailGetMailID(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt64& mailId, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	*@brief 发送系统邮件 简单物品 有回调
	*/
	static UInt32 SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, SendMailCallback* callback, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	static void SendSysMailByAccId(UInt32 accId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	 *@brief 广播在线系统邮件
	 */
	static void BroadcastSysMailOnline(const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay = MAIL_VALID_DAY, UInt32 sendtime = UInt32(CURRENT_TIME.Sec()));

	/**
	 *@brief 删除邮件
	 */
	void DeleteMails(const std::vector<ObjID_t>& mailids);

	/**
	 *@brief 检查标题和内容
	 */
	static bool CheckWord(const std::string& title, const std::string& content);

public://事件
	/**
	 *@brief 心跳
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief 上线
	 */
	void OnOnline();

	/**
	 *@brief 加载邮件返回
	 */
	void OnLoadMailsRet(SelectMailsCallback* callback);
	/**
	 *@brief 加载邮件附件返回
	 */
	void OnLoadMailItemsRet(SelectMailItemsCallback* callback);
	/**
	 *@brief 检查包裹返回
	 */
	void OnCheckPackageRet(const std::vector<ObjID_t>& mailids);
	/**
	 *@brief 扣除消耗返回
	 */
	void OnRemoveCostRet(ObjID_t receiveId, UInt32 accid, const std::string& receiveName, UInt8 replyTimes, const std::string& title
		, const std::string& content, UInt32 silver, std::vector<WSItem*>& items);

private:
	/**
	 *@brief 查询邮件
	 */
	Mail* FindMail(ObjID_t id);

	/**
	 *@brief 同步邮件状态
	 */
	void SyncMailStatus(Mail* mail);

	/**
	 *@brief 同步未读邮件数
	 */
	void SyncUnreadMailNum();

	/**
	 *@brief 检查邮件消耗
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
	//拥有者
	WSPlayer*	m_pOwner;
	//邮件列表 对象Mail*的列表
	MailList	m_Mails;
	//邮件map， 以Mail的 guid为key
	MailMap		m_MailMap;
	//发信定时器
	Avalon::SimpleTimer m_Timer;

	//每日发邮件计数
	static CountMap	m_SendCounts;
};

#endif

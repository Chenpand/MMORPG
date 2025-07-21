#ifndef _BROADCASTMAIL_MGR_H_
#define _BROADCASTMAIL_MGR_H_

#include <AvalonSingleton.h>
#include <CLItemDefine.h>
#include <CLRecordCallback.h>
#include <map>

class WSPlayer;

/** 
 *@brief 广播邮件
 */
struct BroadcastMail
{
	BroadcastMail():id(0), sendtime(0), level(0){}

	//id
	ObjID_t		id;
	//发送时间
	UInt32		sendtime;
	//发送人
	std::string senderName;
	//标题
	std::string	title;
	//内容
	std::string	content;
	//来源
	std::string reason;
	//附件
	std::vector<ItemReward>	items;
	//等级
	UInt16		level;
	//渠道
	std::string  pf;
	//职业
	UInt16		occu;
	//充值总金额达到值
	UInt32		playerChargeNum;
};

/**
 *@brief 广播邮件管理器
 */
class BroadcastMailMgr : public Avalon::Singleton<BroadcastMailMgr>
{
	typedef std::vector<BroadcastMail>	BroadcastMailVec;

	//邮件最大有效期
	const static UInt32 MAIL_VALID_TIME = (30 * DAY_TO_SEC);

public:
	BroadcastMailMgr();
	~BroadcastMailMgr();

public:
	void Init();
	void Final();

public:
	/**
	 *@brief 放一个邮件
	 */
	bool PutMail(const std::string& senderName, const std::string& title, 
		const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, UInt16 level = 0, std::string pf = "",
		UInt8 occu = 0, UInt32 playerChargeNum = 0);

public:
	/**
	 *@brief 查询邮件返回
	 */
	bool OnSelectBroadcastMailRet(CLRecordCallback* callback);

	/**
	 *@brief 心跳事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 玩家上线
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief 是否符合玩家所在渠道
	*/
	bool FitPlatform(std::string playerPf, std::string pfs);
	
	/**
	*@brief 检查一个邮件是否要发送
	*/
	bool CheckMailCanSend(WSPlayer* player, BroadcastMail* mail, bool checkOfflineMailTime = true);
private:
	//广播邮件列表
	BroadcastMailVec	m_BroadcastMails;
};

#endif

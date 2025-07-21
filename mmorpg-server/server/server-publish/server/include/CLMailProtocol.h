#ifndef _CL_MAIL_PROTOCOL_H_
#define _CL_MAIL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLMailDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client 同步未读邮件数
	 */
	class WorldSyncUnreadMailNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_UNREADMAIL_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & num;
		}

		UInt32	num;
	};

	/**
	 *@brief client->server 请求邮件列表
	 */
	class WorldMailListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MAILLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & type & start & num;
		}

	public:
		//类型	0全部 1系统 2个人 3已读 4未读
		//UInt8	type;
		//开始位置 0开始
		//UInt16	start;
		//数量
		//UInt16	num;
	};

	/**
	 *@brief server->client 返回邮件列表
	 */
	class WorldMailListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MAILLIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & type & start & totalnum & mails;
			stream & mails;
		}

	public:
		//类型  同上
		//UInt8	type;
		//开始位置
		//UInt16	start;
		//邮件总数
		//UInt16	totalnum;
		//列表
		std::vector<MailTitleInfo>	mails;
	};

	/**
	 *@brief client->server 请求读取信件
	 */
	class WorldReadMailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_READMAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//id
		ObjID_t	id;
	};

	/**
	 *@brief server->client 返回邮件信息
	 * detailItems格式:[itemid(UInt32) + data][itemid + data]...[itemid(0)]
	 * data格式:(见CLItemProtocol.h)
	 */
	class WorldReadMailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_READMAIL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & id & content & silver & items;
			stream & id & content & items;
		}

	public:
		//id
		ObjID_t id;
		//内容
		std::string	content;
		//银币
		//UInt32	silver;
		//简单道具
		std::vector<ItemReward>	items;
		//详细道具信息
		//detailItems ...
	};

	/**
	 *@brief client->server 收取信件附件
	 */
	class WorldGetMailItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MAILITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & mailType;
		}

	public:
		//类型 0为收取单个 1为全部收取
		UInt8	type;
		//id
		ObjID_t	id;
		//邮件类型
		UInt32	mailType;
	};

	/**
	 *@brief server->client 同步邮件状态
	 */
	class WorldSyncMailStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MAILSTATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & status & hasitem;
		}
		
	public:
		//id
		ObjID_t	id;
		//状态	0未读 1已读
		UInt8	status;
		//是否有附件 0没有 1有
		UInt8	hasitem;
	};

	/**
	 *@brief client->server 发送一封邮件
	 */
	class WorldSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & replyId & receiver & title & content & silver & items;
		}

	public:
		//回复的邮件id 0表示首封邮件
		ObjID_t		replyId;
		//接收者
		std::string	receiver;
		//主题
		std::string	title;
		//内容
		std::string	content;
		//银币
		UInt32		silver;
		//附件(唯一id+数量)列表
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief server->client 通知新邮件
	 */
	class WorldNotifyNewMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEWMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		//邮件标题信息
		MailTitleInfo	info;
	};

	/**
	 *@brief client->server 删除邮件
	 */
	class WorldDeleteMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DELETE_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ids;
		}

	public:
		//邮件id列表
		std::vector<ObjID_t>	ids;
	};

	/**
	 *@brief server->client 通知删除邮件
	 */
	class WorldNotifyDeleteMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DELETEMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ids;
		}

	public:
		//邮件id列表
		std::vector<ObjID_t>	ids;
	};

	/**
	 *@brief world->scene 邮件检查包裹
	 */
	class SceneMailCheckPackageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_CHECKPACKAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & items;
		}

	public:
		ObjID_t	playerId;
		std::map<ObjID_t, std::vector<ItemReward> > items;
	};

	/**
	 *@brief scene->world 邮件检查包裹返回
	 */
	class SceneMailCheckPackageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_CHECKPACKAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & mailids;
		}

	public:
		ObjID_t	playerId;
		//邮件id
		std::vector<ObjID_t>	mailids;
	};

	/**
	 *@brief world->scene 给道具
	 * detailItems格式:[Guid_t + itemid(UInt32) + data][Guid_t + itemid + data]...[Guid_t(0)]
	 * data格式:(见CLItemProtocol.h)
	 */
	class SceneMailGiveItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_GIVEITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & silver & items;
		}
	public:
		//玩家
		ObjID_t	playerId;
		//银币
		UInt32	silver;
		//简单道具
		std::vector<ItemReward>	items;
		//详细道具信息
		//detailItems ...
	};

	/**
	 *@brief world->scene 发送邮件时场景处理扣除邮资和附件
	 */
	class SceneMailRemoveCostReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_REMOVECOST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & accid & receiveId & receiveName & replyTimes & title & content & cost & silver & items;
		}

	public:
		//玩家id
		ObjID_t playerId;
		//接收者
		ObjID_t	receiveId;
		//接收者账号
		UInt32	accid;
		//接收者姓名
		std::string	receiveName;
		//回复次数
		UInt8	replyTimes;
		//主题
		std::string	title;
		//内容
		std::string content;
		//邮资
		UInt32	cost;
		//银币
		UInt32	silver;
		//附件(唯一id+数量)列表
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief scene->world 发送邮件时场景处理扣除邮资和附件返回
	 * detailItems格式:[itemid(UInt32) + data][itemid(UInt32) + data]...[UInt32(0)]
	 * data格式:(见CLItemProtocol.h)
	 */
	class SceneMailRemoveCostRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_REMOVECOST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & receiveId & accid & receiveName & replyTimes & title & content & silver;
		}

	public:
		//玩家id
		ObjID_t	playerId;
		//接收者
		ObjID_t	receiveId;
		//接收者账号
		UInt32	accid;
		//接收者姓名
		std::string receiveName;
		//回复次数
		UInt8 replyTimes;
		//主题
		std::string title;
		//内容
		std::string content;
		//银币
		UInt32 silver;
		//详细道具信息
		//detailItems ...
	};

	/**
	 *@brief scene->world 发送系统邮件
	 */
	class WorldSendSysMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_SYSMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & senderName & title & content & reason & validDay & items;
		}

	public:
		//玩家
		ObjID_t		playerId;
		//发件人
		std::string senderName;
		//标题
		std::string title;
		//内容
		std::string content;
		//来源
		std::string reason;
		//有效天数
		UInt32 validDay;
		//银币
		//UInt32	silver;
		//道具
		std::vector<ItemReward>	items;
	};

	/**
	 *@brief scene->world 发送系统邮件
	 */
	class WorldSendSysMailToTribeLeader : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_SYSMAIL_TO_TRIBELEADER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tribeId & title & content & silver & items;
		}

	public:
		//部落id
		ObjID_t		tribeId;
		//标题
		std::string title;
		//内容
		std::string content;
		//银币
		UInt32		silver;
		//道具
		std::vector<ItemReward>	items;
	};

	/**
	*@brief teamcopy->world 发送邮件
	*/
	class WorldTeamCopySendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_COPY_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & name & title & content & reason & rewards;
		}

		// 接收者
		UInt64 roleId;
		// 发送者
		std::string name;
		// 标题
		std::string title;
		// 内容
		std::string content;
		// 原因
		std::string reason;
		// 邮件内容
		ItemRewardVec rewards;
	};


	/**
	*@brief union->world 发送邮件
	*/
	class WorldUnionSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UNION_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mailGuid & roleId & name & title & content & reason & rewards;
		}

		// 邮件guid
		UInt64 mailGuid;
		// 接收者
		UInt64 roleId;
		// 发送者
		std::string name;
		// 标题
		std::string title;
		// 内容
		std::string content;
		// 原因
		std::string reason;
		// 邮件内容
		ItemRewardVec rewards;
	};
}

#endif

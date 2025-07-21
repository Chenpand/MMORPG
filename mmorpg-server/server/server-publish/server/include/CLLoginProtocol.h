#ifndef _CL_LOGIN_PROTOCOL_H_
#define _CL_LOGIN_PROTOCOL_H_

#include <AvalonSocket.h>
#include <CLProtocolDefine.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	/*
		noted by aprilliu, 所有协议都要继承于Avalon::Protocol，并定义自己的协议id，以及实现自己的serialize操作（encode以及decode, 
		NetInputStream以及NetOutputStream重载的&操作符会进行decode以及encode操作）
	*/
	/**
	 *@brief client->admin 请求验证账号
	 */
	class AdminLoginVerifyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_VERIFY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & version & source1 & append1 & source2 & append2;
			stream.SeriaArray(tableMd5, 16);
			stream & svnVersion;
			stream.SeriaArray(append3, 12);
			stream & params;
			stream.SeriaArray(hashValue,20);
		}
	
		// 版本号
		UInt32      version;
		//来源1
		std::string	source1;
		// 无效字段
		UInt32		append1;
		//来源2
		std::string	source2;
		// 无效字段
		UInt8		append2;
		//表格md5
		UInt8		tableMd5[16];
		//svn版本号
		UInt32		svnVersion;
		// 无效字段
		UInt8		append3[12];
		//登陆信息
		std::string	params;
		//hash
		UInt8 hashValue[20];
	};

	/**
	 *@brief admin->client 返回验证账号结果
	 */
	class AdminLoginVerifyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_VERIFY_RET)
	public:
		AdminLoginVerifyRet()
		{
			phoneBindRoleId = 0;
			accid = 0;
			result = 0;
			encryptProtocol = 0;
			openBugly = 0;
			voiceFlag = 0;
			openNewReconnectAlgo = 0;
			openNewReportFrameAlgo = 0;
			zoneId = ZONE_ID;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & phoneBindRoleId & errMsg & accid & dirSig & addr & result & replayAgentAddr & encryptProtocol & 
				openBugly & voiceFlag & activityYearSortListUrl & openNewReconnectAlgo & openNewReportFrameAlgo & webActivityUrl
				& commentServerAddr & zoneId & redPacketRankUrl & convertUrl & reportUrl & battleUseTcp & writeQuestionnaireUrl;
		}

		
		//手机绑的角色ID
		ObjID_t				phoneBindRoleId;
		//错误字符串
		std::string			errMsg;
		//账号id
		UInt32				accid;
		//目录服务器校验签名
		std::string			dirSig;
		//网络地址
		Avalon::SockAddr	addr;
		//结果
		UInt32				result;
		//录像服务器地址
		std::string			replayAgentAddr;
		//是否协议加密
		UInt8				encryptProtocol;
		//是否开启bugly
		UInt8				openBugly;
		//语音标识
		UInt32				voiceFlag;
		// 活动排行榜URL
		std::string			activityYearSortListUrl;
		// 是否开启新的重连机制
		UInt8				openNewReconnectAlgo;
		// 是否开启新的发送帧算法
		UInt8				openNewReportFrameAlgo;
		// 网页活动链接
		std::string			webActivityUrl;
		// 装备图鉴评论服地址
		std::string			commentServerAddr;
		// 区id
		UInt32				zoneId;
		// 红包排名链接
		std::string			redPacketRankUrl;
		// 转移信息链接地址
		std::string			convertUrl;
		// 举报服务器
		std::string			reportUrl;
		// 战斗是否使用tcp
		UInt8				battleUseTcp;
		// 填写问卷
		std::string			writeQuestionnaireUrl;
	};

	/**
	 *@brief admin->admin 网络线程通知逻辑线程新的验证请求
	 */
	class AdminLoginNewPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_NEWPLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
			stream & gmauthority;
			stream.SeriaArray(hash, 20);
			stream & openid & qqVipInfo & source1 & source2 & pf & did & phoneBindRoleId & antiAddictInfo & model & deviceVersion;
		}

	public:
		//账号id
		UInt32	accid;
		//gm权限
		UInt8	gmauthority;
		//hashValue
		UInt8	hash[20];
		//openid
		std::string	openid;
		//qq特权信息
		QQVipInfo	qqVipInfo;
		//来源1
		std::string	source1;
		//来源2
		std::string	source2;
		//平台
		std::string	pf;
		//设备id
		std::string did;
		//手机绑定的角色
		ObjID_t		phoneBindRoleId;
		//防沉迷信息
		AntiAddictInfo antiAddictInfo;
		//设备型号
		std::string model;
		//设备版本
		std::string deviceVersion;
	};

	/**
	*@brief admin->world 查询已经分配给玩家的gateserver
	*/
	class AdminPlayerGateInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_PLAYER_GATE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		// 登录信息
		AdminLoginNewPlayer info;
	};

	/**
	*@brief world->admin 返回已经分配给玩家的gateserver
	*/
	class AdminPlayerGateInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_PLAYER_GATE_INFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & errMsg & gateId & info;
		}

		// 结果
		UInt32 result;
		// 字符串
		std::string errMsg;
		//已分配的gateserver
		UInt32	gateId;
		// 登录信息
		AdminLoginNewPlayer info;
	};
	

	/**
	 *@brief admin->gate 将登陆请求发送到合适的网关
	 */
	class GateSendAccountInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_ACCOUNT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & gmauthority;
			stream.SeriaArray(hash, 20);
			stream & openid & qqVipInfo & source1 & source2 & pf & did & phoneBindRoleId & antiAddictInfo & model & deviceVersion;
		}

		//账号id
		UInt32	accid;
		//gm权限
		UInt8	gmauthority;
		//hashValue
		UInt8	hash[20];
		//openid
		std::string	openid;
		//qq特权信息
		QQVipInfo	qqVipInfo;
		//来源1
		std::string	source1;
		//来源2
		std::string	source2;
		//平台
		std::string	pf;
		//设备id
		std::string did;
		//手机绑定角色ID
		ObjID_t		phoneBindRoleId;
		//防沉迷信息
		AntiAddictInfo antiAddictInfo;
		//设备型号
		std::string model;
		//设备版本
		std::string deviceVersion;
	};

	/**
	 *@brief gate->admin 网关返回登录地址
	 */
	class GateNetAddressRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NETADDRESS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & errMsg & accid & openid;
			if(result == 0)
			{
				stream & addr & phoneBindRoleId & encryptProtocol & pf;
			}
		}

		//结果
		UInt32				result;
		// 错误码
		std::string			errMsg;
		//账号id
		UInt32				accid;
		//openid
		std::string			openid;
		//网络地址
		Avalon::SockAddr	addr;
		//手机绑的角色ID
		ObjID_t				phoneBindRoleId;
		//是否协议加密
		UInt8				encryptProtocol;
		//平台
		std::string			pf;
	};

	/**
	 *@brief client->gate 请求连接到网关
	 */
	class GateClientLoginReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CLIENTLOGIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId;
			stream.SeriaArray(hashValue, 20);
		}

		//登陆信息
		UInt32	accId;
		//hash
		UInt8 hashValue[20];
	};

	/**
	 *@brief gate->client 网关返回连接结果
	 */
	class GateClientLoginRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CLIENTLOGIN_RET)
	public:
		GateClientLoginRet() : notifyVeteranReturn(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & hasrole & waitPlayerNum & serverStartTime & notifyVeteranReturn;
		}

		//结果
		UInt32	result;
		//有木有角色 1有 0木有
		UInt8	hasrole;
		//需要等待的玩家数
		UInt32	waitPlayerNum;
		//服务器开服时间
		UInt32	serverStartTime;
		//通知老兵回归
		UInt8   notifyVeteranReturn;
	};

	/**
	 *@brief gate->admin 网关向admin同步登陆人数
	 */
	class AdminSyncGatePlayerNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_GATEPLAYER_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & maxPlayerNum;
		}

		//人数
		UInt32	playerNum;
		//最大人数
		UInt32	maxPlayerNum;
	};

	/**
	*@brief world->admin world向admin同步登陆人数
	*/
	class AdminSyncWorldPlayerNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_WORLDPLAYER_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & maxPlayerNum;
		}

		//人数
		UInt32	playerNum;
		//最大人数
		UInt32	maxPlayerNum;
	};

	/**
	*@brief gate->admin gate向admin同步排队信息
	*/
	class AdminSyncLoginQueueInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_LOGIN_QUEUE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inQueuePlayerNum;
		}

		//排队中的人数
		UInt32	inQueuePlayerNum;
	};
}

#endif

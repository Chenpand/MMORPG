#ifndef _AS_APPLICATION_H_
#define _AS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <set>

// 重新加载配置
#define  SIG_RELOAD_CONFIG 10

struct ClientMD5Info
{
	std::string tableMd5;
	std::string skillMd5;
};

/**
 *@brief 管理服应用程序类
 */
class ASApplication : public CLApplication, public Avalon::Singleton<ASApplication>
{
public:
	ASApplication();
	~ASApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	void OnQuit();

    bool CheckVersion(UInt32 version);

public:
	bool IsForbidLogin(UInt32 accid);

public:
	const std::string& GetCryptKey1() const{ return m_CryptKey1; }
	const std::string& GetCryptKey2() const{ return m_CryptKey2; }
	const std::string& GetBillingKey() const{ return m_BillingKey; }
	const std::string& GetDirCryptKey() const { return m_DirCryptKey; }
	const std::string& GetReplayAgentAddress() const { return m_ReplayAgentAddress; }

	void SetWorldPlayerNum(UInt32 num) { m_WorldPlayerNum = num; }
	UInt32 GetWorldPlayerNum() const { return m_WorldPlayerNum; }

	void SetWorldMaxPlayerNum(UInt32 num) { m_WorldMaxPlayerNum = num; }
	UInt32 GetWorldMaxPlayerNum() const { return m_WorldMaxPlayerNum; }

	void SetInQueuePlayerNum(UInt32 num) { m_InQueuePlayerNum = num; }
	UInt32 GetInQueuePlayerNum() const { return m_InQueuePlayerNum; }

	const std::string GetTableMD5() const { return m_TableMd5; }
	const std::string GetSkillMD5() const { return m_SkillMd5; }
	const std::vector<ClientMD5Info>& GetClientMD5Infoes() const { return m_Md5Infoes; }
	const std::string GetSoMD5() const { return m_SoMd5; }
	const std::string GetSoKey() const { return m_SoKey; }
	bool CheckTableMD5() const { return m_CheckTableMd5; }
	bool IsOpenBugly(UInt32 accid);
	const UInt32 GetVoiceFlag() const { return m_VoiceFlag; }
	std::string GetActivityYearSortListUrl(const std::string& pf) const;
	std::string GetWebActivityUrl(const std::string& pf) const;

	bool IsOpenNewReconnectAlgo() const { return m_OpenNewReconnectAlgo; }
	bool IsOpenNewReportFrameAlgo() const { return m_OpenNewReportFrameAlgo; }

	const std::string& GetCommentServerUrl() const { return m_CommentServerUrl; }

	const std::string& GetRedPacketRankUrl() const { return m_RedPacketRankUrl; }

	/**
	*@brief 是否需要转移账号
	*/
	bool IsNeedConvertAccount(const std::string& pf) const { return std::find(m_ConvertPfs.begin(), m_ConvertPfs.end(), pf) != m_ConvertPfs.end(); }

	/**
	*@brief 转移信息链接地址
	*/
	const std::string& GetConvertUrl() const { return m_ConvertUrl; }

	/**
	*@brief 举报链接地址
	*/
	const std::string& GetReportUrl() const { return m_ReportWebUrl; }

	/**
	*@brief 填写问卷地址
	*/
	const std::string& GetWriteQuestionnaireUrl() const { return m_WriteQuestionnaireUrl; }

	/**
	*@brief 是否记录团本日志
	*/
	bool IsLogTeamCopyProtocol() { return m_LogTeamCopyProtocol; }

	/**
	*@brief 战斗服是否使用tcp
	*/
	UInt8 BattleUseTcp() const { return m_BattleUseTcp; }

	/**
	*@brief 账号验证参数长度
	*/
	UInt32 GetAccVerifyParamLength() { return m_AccVerifyParamLength; }

protected:
	// 加载各种动态配置
	void LoadDynamicConfig();
	void LoadVersion();
    bool ReadVersionFromStr(std::string str);
    UInt32 MakeVersion(UInt32 major, UInt32 minor, UInt32 develop);
    void FetchVersion(UInt32 version, UInt32& major, UInt32& minor, UInt32& develop);
	void ReportServerStatus();

private:
    //key1             
	std::string	m_CryptKey1;
	//key2
	std::string	m_CryptKey2;
	//目录服务器加密key
	std::string	m_DirCryptKey;

	//billing key
	std::string	m_BillingKey;

    // 版本号
    UInt32      m_Version;

	// 是否要重新加载版本号
	bool		m_ReloadConfig;

	// 是否开始性能测试
	bool		m_StartProfiler;

	// 是否已经开始性能测试
	bool		m_IsStartProfiler;

	// 汇报服务器状态定时器
	Avalon::SimpleTimer		m_ReportTimer;

	// 目录服务器地址
	SockAddrVec				m_DirServerAddrs;

	// 目录服务器socket
	Avalon::Socket			m_DirUdpSocket;

	// 是否禁止登录
	bool					m_ForbidLogin;

	// 登录白名单（即使禁止登录也可以登录）
	std::set<UInt32>		m_LoginWhiteList;

	// 录像代理服务器地址
	std::string				m_ReplayAgentAddress;

	// worldserver当前人数
	UInt32					m_WorldPlayerNum;

	// worldserver最大人数
	UInt32					m_WorldMaxPlayerNum;

	// 表格md5
	std::string				m_TableMd5;

	// 技能md5
	std::string				m_SkillMd5;

	// 所有MD5信息
	std::vector<ClientMD5Info>		m_Md5Infoes;

	// somd5+key
	std::string				m_SoMd5;

	// soKey
	std::string				m_SoKey;

	// 是否检测md5
	bool					m_CheckTableMd5;

	// 正在排队中的玩家
	UInt32					m_InQueuePlayerNum;

	// 是否开启bugly
	bool					m_OpenBugly;

	// 语音标志
	UInt32					m_VoiceFlag;

	//活动排行榜地址
	std::string				m_ActivityYearSortListUrl;

	// bugly白名单
	std::set<UInt32>		m_BuglyWhiteList;

	// 是否开启新的重连机制
	bool					m_OpenNewReconnectAlgo;

	// 是否开启新的发送帧算法
	bool					m_OpenNewReportFrameAlgo;

	//网页活动地址
	std::string				m_WebActivityUrl;

	// 装备图鉴评论服地址
	std::string				m_CommentServerUrl;

	//红包排行地址
	std::string				m_RedPacketRankUrl;

	// 需要账号转移的渠道
	std::vector<std::string>	m_ConvertPfs;
	// 转移信息链接地址
	std::string					m_ConvertUrl;

	// 举报链接地址
	std::string					m_ReportWebUrl;

	// 填写问卷地址
	std::string					m_WriteQuestionnaireUrl;

	// 是否记录团本消息
	bool						m_LogTeamCopyProtocol;

	// 战斗服是否使用tcp
	UInt8						m_BattleUseTcp;

	// 账号验证参数长度
	UInt32						m_AccVerifyParamLength;
};

#endif

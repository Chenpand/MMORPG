#ifndef _CL_RECORD_CLIENT_H_
#define _CL_RECORD_CLIENT_H_

#include <set>

#include <AvalonThread.h>
#include <AvalonDoubleQueue.h>
#include <AvalonNetConnection.h>
#include <AvalonSingleton.h>

#include "CLRecordCommand.h"
#include "CLRecordCallback.h"

class CLApplication;

/**
 *@brief 
 */
class CLRecordClient : public Avalon::Thread, public Avalon::Singleton<CLRecordClient>
{
	typedef std::map<UInt32, CLRecordCallback*> CallbackMap;

	/**
	 *@brief 请求
	 */
	struct RecordRequest
	{
		RecordRequest():cmd(NULL),callback(NULL){}

		CLRecordCommand*	cmd;		//	命令
		CLRecordCallback*	callback;	//	回调
	};
public:
	CLRecordClient();
	~CLRecordClient();

public:
	/**
	 *@brief 初始化
	 */
	bool Init(CLApplication* app, Avalon::NetConnectionPtr conn);

	/**
	*@brief 设置需要转发的表
	*/
	void SetNeedTransTables(const std::vector<std::string>& tables);
	UInt8 IsTableNeedTrans(const std::string& tablename);

	/**
	 *@brief 终止
	 */
	void Final();

	/**
	 *@brief 接收到消息
	 */
	void OnPacketRecved(Avalon::Packet* packet);

	/**
	 *@brief 处理回调
	 */
	void ProcessCallbacks();

public:
	/**
	 *@brief 生成id
	 */
	ObjID_t GenGuid();

	/**
	 *@brief 准备插入命令
	 */
	CLInsertCommand* PrepareInsertCommand(const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLInsertCommand* PrepareInsertCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief 准备更新命令
	 */
	CLUpdateCommand* PrepareUpdateCommand(const std::string& table, DBKey_t key, bool bflush, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(UInt32 zoneId, const std::string& table, DBKey_t key, bool bflush, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief 准备删除命令
	 */
	CLDeleteCommand* PrepareDeleteCommand(const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief 准备查询命令
	 */
	CLSelectCommand* PrepareSelectCommand(const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLSelectCommand* PrepareSelectCommand(UInt32 zoneId, const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief 准备回写命令
	 */
	CLFlushCommand* PrepareFlushCommand(const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLFlushCommand* PrepareFlushCommand(UInt32 zoneId, const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief 准备改名命令
	 */
	CLChangeNameCommand* PrepareChangeNameCommand(const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert);
	CLChangeNameCommand* PrepareChangeNameCommand(UInt32 zoneId, const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert);

	/**
	*@brief 准备主键自增插入命令
	*/
	CLAutoGuidInsertCommand* PrepareAutoGuidInsertCommand(const std::string& table);

	/**
	 *@brief 发送一个命令
	 */
	void SendCommand(CLRecordCommand* cmd,CLRecordCallback* callback = NULL, UInt64 roleId = 0);

public:

private:
	/**
	 *@brief 线程中处理消息包
	 */
	void ProcessPackets();
	
	/**
	 *@brief 线程中发送所有命令
	 */
	void SendCommands();

	void Run();  //noted by aprilliu, 线程函数中被调用

	void HandleInsertRet(Avalon::Packet* packet);
	void HandleUpdateRet(Avalon::Packet* packet);
	void HandleDeleteRet(Avalon::Packet* packet);
	void HandleSelectRet(Avalon::Packet* packet);
	void HandleFlushRet(Avalon::Packet* packet);
	void HandleChangeNameRet(Avalon::Packet* packet);

	/**
	 *@brief 设置结果
	 */
	void SetResult(UInt32 requestid, UInt32 result);

	/**
	 *@brief 查找一个回调
	 */
	CLRecordCallback* FindCallback(UInt32 requestid);

private:
	//应用程序，异常时终止服务
	CLApplication*	m_pApp;

	//请求队列   由于Avalon::DoubleQueue本身是线程安全的，所以m_Requests  m_Callbacks  m_Packets 都没有进行加解锁的操作
	Avalon::DoubleQueue<RecordRequest,256>	m_Requests;  //noted by aprilliu , 发给record server的异步请求队列（在record client线程主循环中被pop出来 进行处理）
	//回调队列
	Avalon::DoubleQueue<CLRecordCallback*>	m_Callbacks; //noted by aprilliu,  已收到回应的请求的回调列表（也是主逻辑线程和record client线程通讯的队列）
	//网络连接列表
	Avalon::NetConnectionPtr	m_Connection;
	//接收到的消息队列   noted by aprilliu record client线程 与 内网IO线程通讯的消息队列（）
	Avalon::PacketQueue	m_Packets;

	//回调列表
	CallbackMap		m_WaitCallbacks;  //noted by aprilliu, 等待record server返回响应后进行回调的队列， 与发出去的db请求对应的回调，以请求序列号为key。
	//当前时间
	UInt32			m_Now;
	//guid种子
	UInt64			m_Seed;
	//请求id种子
	UInt32			m_RequestIdSeed;

	//是否运行
	bool	m_bRun;

	// 需要转发到备用Record的表
	std::set<std::string>	m_Trans2SlaverTableNames;

	// 包序号
	UInt32 m_Sequence;

public:

	/**
	 *@brief 初始化(支持多个RecordServer连接)
	 */
	bool Init(CLApplication* app, const std::vector<Avalon::NetConnectionPtr>& connVec);

private:

	/**
	 *@brief 线程中发送所有命令
	 */
	void SendCommandNews();

private:

	//RecordServer数量
	UInt32 m_ConnectionNum;
	//网络连接列表
	std::vector<Avalon::NetConnectionPtr>	m_ConnectionVec;
	//请求队列
	std::vector<Avalon::DoubleQueue<RecordRequest, 256>>	m_RequestVec;
};


/**
 *@brief 回写玩家相关表数据
 */
 void CLFlushPlayerTables();


#endif

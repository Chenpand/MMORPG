#ifndef _US_APPLICATION_H_
#define _US_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <set>

/**
 *@brief 跨服服务器应用程序类
 */
class USApplication : public CLApplication, public Avalon::Singleton<USApplication>
{
public:
	USApplication();
	~USApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	/**
	*@brief 是否记录转发日志
	*/
	bool IsLogTransProtocol() { return m_LogTransProtocol; }

	/**
	*@brief 是否开启该功能
	*/
	bool IsOpenFunction(UnionServerFunctionType funType) { return m_OpenFunction.find(funType) != m_OpenFunction.end(); }

	/**
	*@brief 是否关闭冠军赛移动同步和观战同步
	*/
	bool IsCloseMove() { return m_CloseMove; }

	/**
	*@brief 获取开启的功能
	*/
	std::set<UnionServerFunctionType>& GeOpenFunction() { return m_OpenFunction; }
	
	/**
	*@brief 下架订单的区服
	*/
	std::set<UInt32>& GetCloseOrderZone() { return m_CloseOrderZone; }

	/**
	*@brief 下架订单的角色
	*/
	std::set<UInt64>& GetCloseOrderRole() { return m_CloseOrderRole; }

	/**
	*@brief 下架订单
	*/
	std::set<UInt64>& GetCloseOrder() { return m_CloseOrder; }
	
	/**
	*@brief 收益检查
	*/
	std::set<UInt32>& GetReturnProfitZone() { return m_ReturnProfitZone; }

	UInt32 GetMoveCnt() { return m_MoveCnt; }

	UInt32 GetPlayerLimit() { return m_PlayerLimit; }

private:
	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	bool ReloadDataTables();

private:
	UInt32 m_MoveCnt = 1;
	//tick时间
	UInt64	m_TickTime;
	// 重载配置
	bool m_ReloadConfig;
	// 重载表格
	bool m_ReloadTable;
	// 是否记录转发日志
	bool m_LogTransProtocol;
	// 冠军赛关闭移动同步和观战同步
	bool m_CloseMove = false;
	// 已开放功能
	std::set<UnionServerFunctionType> m_OpenFunction;

	// 金币寄售需要下架的区服
	std::set<UInt32> m_CloseOrderZone;
	// 金币寄售需要下架的角色
	std::set<UInt64> m_CloseOrderRole;
	// 金币寄售需要下架的订单
	std::set<UInt64> m_CloseOrder;
	// 金币寄售收益返还的区服
	std::set<UInt32> m_ReturnProfitZone;

	//冠军赛人数限制
	UInt32 m_PlayerLimit = 0;
};

#endif

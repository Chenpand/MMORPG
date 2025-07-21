#ifndef _RS_APPLICATION_H_
#define _RS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonXmlDocument.h>

/**
*@brief 录像服务器
*/
class RSApplication : public CLApplication, public Avalon::Singleton<RSApplication>
{
public:
	RSApplication();
	~RSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	void OnSignal(int signal);

	Avalon::RetryDBConnection* GetDBConn() { return m_DBConn; }

public:
	/**
	*@brief 是否可以观看高手录像
	*/
	bool CanViewMasterReplay() { return m_CanViewMasterReplay; }

	/**
	*@brief 是否批量加载录像信息
	*/
	bool IsBatchLoadReplay() { return m_IsBatchLoadReplay; }

private:
	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

	/**
	*@brief 根据配置创建一个数据库连接
	*/
	Avalon::RetryDBConnection* CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node);

	void LoadDynamicConfig();

private:
	//数据库连接
	Avalon::RetryDBConnection* m_DBConn;

	//重新加载配置
	bool	m_ReloadConfig;

	// 是否可以观看高手录像
	bool	m_CanViewMasterReplay;

	// 是否批量加载录像信息
	bool	m_IsBatchLoadReplay;
};

#endif

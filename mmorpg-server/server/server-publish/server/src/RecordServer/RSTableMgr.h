#ifndef _RS_TABLEMGR_H_
#define _RS_TABLEMGR_H_

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonXmlDocument.h>
#include <AvalonSimpleTimer.h>
#include "RSTable.h"

/**
 *@brief 数据缓存
 */
class RSTableMgr : public Avalon::Singleton<RSTableMgr>
{
	typedef std::map<std::string,RSTable*>		TableMap;
	typedef std::map<std::string,std::string>	UniqueNameMap;


	/**
	 *@brief guid种子
	 */
	struct GuidSeed
	{
		GuidSeed():nodetype(0), nodeid(0), seed(0){}

		UInt32 nodetype;	//节点类型
		UInt32 nodeid;		//节点id
		UInt64 seed;		//节点种子
	};
	typedef std::vector<GuidSeed>	GuidSeedVec;
 
public:
	RSTableMgr();
	~RSTableMgr();

	/**
	 *@brief 初始化
	 */
	bool Init(Avalon::PropertySet* properties);

	/**
	 *@brief 终止
	 */
	void Final();

public:
	/**
	 *@brief 获取本地数据库连接
	 */
	Avalon::RetryDBConnection* GetLocalDBConn() const{ return m_pLocalConn; }

	/**
	 *@brief 获取唯一名验证连接
	 */
	Avalon::RetryDBConnection* GetUniqueNameDBConn() const { return m_pUniqueNameConn; }
	/**
	 *@brief 添加一个表
	 */
	bool AddTable(RSTable* table);

	/**
	 *@brief 查找一个表
	 */
	RSTable* FindTable(const std::string& name);

	/**
	 *@brief 设置获取guid种子
	 */
	bool SetGuidSeed(UInt32 nodetype,UInt32 nodeid,UInt64 seed);
	UInt64 GetGuidSeed(UInt32 nodetype,UInt32 nodeid) const;

	/**
	 *@brief 回写所有数据
	 */
	void Flush();

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief 根据配置创建一个数据库连接
	 */
	Avalon::RetryDBConnection* CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node);

	/**
	 *@brief 获取库中的表格
	 */
	bool ShowTables(Avalon::RetryDBConnection* dbconn);

public:
	//本地数据库连接
	Avalon::RetryDBConnection *m_pLocalConn;
	//验证唯一性的连接
	Avalon::RetryDBConnection *m_pUniqueNameConn;

	//回写检查定时器
	Avalon::SimpleTimer	m_CheckFlushTimer;
	//日志定时器
	Avalon::SimpleTimer m_LogTimer;

	//表
	TableMap	m_Tables;
	//缓存时间
	UInt32		m_CacheTime;
	//唯一名map
	UniqueNameMap	m_UniqueNames;

	//guid种子
	GuidSeedVec	m_GuidSeeds;
};

#endif

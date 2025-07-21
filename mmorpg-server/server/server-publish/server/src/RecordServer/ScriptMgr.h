#ifndef _SCRIPT_SYSTEM_H_
#define _SCRIPT_SYSTEM_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonScriptFile.h>
#include <CLScriptMgr.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonXmlDocument.h>

enum RSScriptType
{
	// 文件夹下的脚本
	// 单个脚本
	RECORD_SCRIPT_DBPATCH,
	RECORD_SCRIPT_NUM,
};

class ScriptMgr : public CLScriptMgr, public Avalon::Singleton<ScriptMgr>
{
public:
	ScriptMgr();
	~ScriptMgr();

public:
	virtual bool Init(Avalon::PropertySet* properties);
	//virtual void Final();

private:
	/**
	*@brief 根据配置创建一个数据库连接
	*/
	Avalon::RetryDBConnection* CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node);
	
	/**
	*@brief 注册类和函数
	*/
	void DefClasses();

	/**
	*@brief 加载脚本
	*/
	bool LoadScripts();

	/**
	*@brief 加载脚本
	*/
	bool LoadScript(UInt32 type);

	/**
	*@brief 创建脚本对象
	*/
	Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

private:
	// 脚本配置
	static ScriptConfig		s_scriptConfigs[RECORD_SCRIPT_NUM];
};

#endif
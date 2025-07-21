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
	// �ļ����µĽű�
	// �����ű�
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
	*@brief �������ô���һ�����ݿ�����
	*/
	Avalon::RetryDBConnection* CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node);
	
	/**
	*@brief ע����ͺ���
	*/
	void DefClasses();

	/**
	*@brief ���ؽű�
	*/
	bool LoadScripts();

	/**
	*@brief ���ؽű�
	*/
	bool LoadScript(UInt32 type);

	/**
	*@brief �����ű�����
	*/
	Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

private:
	// �ű�����
	static ScriptConfig		s_scriptConfigs[RECORD_SCRIPT_NUM];
};

#endif
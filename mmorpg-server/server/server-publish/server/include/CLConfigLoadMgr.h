#ifndef _CL_DATA_ENTRY_MGR_H_
#define _CL_DATA_ENTRY_MGR_H_

#include "CLDefine.h"
#include <functional>

/**
*@brief 数据配置加载管理类
*/
class CLConfigLoadMgr
{
public:
	CLConfigLoadMgr();
	virtual ~CLConfigLoadMgr();

public:

	/**
	*@brief 注册需要重载的表
	*/
	virtual void RegisterReloadDataTables() = 0;

	/**
	*@brief 加载需要重载的表
	*/
	void LoadNeedReloadTables(const std::string& fileName);

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables(const std::string& path);

protected:

	typedef std::function<bool(const std::string& file)>	ReloadTableFun;
	typedef std::function<void()>	ReloadExtraFun;

	/**
	*@brief 注册重新载入数据表函数
	*/
	void RegisterReloadDataTableFun(const std::string& file, ReloadTableFun funPtr);

	/**
	*@brief 注册重新载入数据表函数
	*/
	void RegisterReloadDataExtraFun(const std::string& file, ReloadExtraFun funPtr);

#define DATA_TABLE_LOAD(file, class_name)  \
	if(!class_name->LoadData(std::string("../Config/DataTables/") + file)) return false;

#define REGISTER_DATA_TABLE_RELOAD(file, reloadFun, reloadObj) \
 	RegisterReloadDataTableFun(file, std::bind(reloadFun, reloadObj, std::placeholders::_1));
	
#define REGISTER_DATA_TABLE_EXTRA_RELOAD(file, extraFun, extraObj) \
	RegisterReloadDataExtraFun(file, std::bind(extraFun, extraObj));
		
	struct ReloadFunData
	{
		ReloadTableFun tableFn;
		std::vector<ReloadExtraFun> extraFnVec;

		ReloadFunData() : tableFn(nullptr) {}
		ReloadFunData(ReloadTableFun ptr1) : tableFn(ptr1) {}
	};

	typedef std::map<std::string, ReloadFunData>		ReloadDataFunMap;

	// 数据表重载入
	ReloadDataFunMap m_ReloadDataFunMap;

	// 需要重载的表
	std::vector<std::string> m_NeedReloadTableVec;
};
















#endif
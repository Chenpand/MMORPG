#ifndef _CL_DATA_ENTRY_MGR_H_
#define _CL_DATA_ENTRY_MGR_H_

#include "CLDefine.h"
#include <functional>

/**
*@brief �������ü��ع�����
*/
class CLConfigLoadMgr
{
public:
	CLConfigLoadMgr();
	virtual ~CLConfigLoadMgr();

public:

	/**
	*@brief ע����Ҫ���صı�
	*/
	virtual void RegisterReloadDataTables() = 0;

	/**
	*@brief ������Ҫ���صı�
	*/
	void LoadNeedReloadTables(const std::string& fileName);

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables(const std::string& path);

protected:

	typedef std::function<bool(const std::string& file)>	ReloadTableFun;
	typedef std::function<void()>	ReloadExtraFun;

	/**
	*@brief ע�������������ݱ���
	*/
	void RegisterReloadDataTableFun(const std::string& file, ReloadTableFun funPtr);

	/**
	*@brief ע�������������ݱ���
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

	// ���ݱ�������
	ReloadDataFunMap m_ReloadDataFunMap;

	// ��Ҫ���صı�
	std::vector<std::string> m_NeedReloadTableVec;
};
















#endif
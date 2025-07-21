#ifndef _LOGIN_PUSH_MGR_
#define _LOGIN_PUSH_MGR_

#include <CLObject.h>
#include <AvalonSingleton.h>
#include <CLLoginDataEntry.h>
#include <CLEnterGameProtocol.h>


class WSPlayer;

class LoginPushMgr : public Avalon::Singleton<LoginPushMgr>
{
public:
	LoginPushMgr();
	~LoginPushMgr();

public:
	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 加载表格
	*/
	void LoadTable(bool isReload = false);

	/**
	*@brief 重新加载txt
	*/
	void ReloadDataTable();

	/**
	*@brief 添加登录推送
	*/
	void AddLoginPushInfo(LoginPushInfo* info);

	/**
	*@brief 获取登录推送信息
	*/
	void GetLoginPushInfos(std::vector<LoginPushInfo>& infos);

	/**
	*@brief 查找一项推送信息
	*/
	LoginPushInfo* FindLoginPushInfo(UInt8 id);

private:
	// 登录推送
	std::vector<LoginPushInfo*> m_LoginPushInfos;
	std::vector<LoginPushInfo*> m_TempLoginPushInfos;
};


#endif
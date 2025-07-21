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
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ���ر��
	*/
	void LoadTable(bool isReload = false);

	/**
	*@brief ���¼���txt
	*/
	void ReloadDataTable();

	/**
	*@brief ��ӵ�¼����
	*/
	void AddLoginPushInfo(LoginPushInfo* info);

	/**
	*@brief ��ȡ��¼������Ϣ
	*/
	void GetLoginPushInfos(std::vector<LoginPushInfo>& infos);

	/**
	*@brief ����һ��������Ϣ
	*/
	LoginPushInfo* FindLoginPushInfo(UInt8 id);

private:
	// ��¼����
	std::vector<LoginPushInfo*> m_LoginPushInfos;
	std::vector<LoginPushInfo*> m_TempLoginPushInfos;
};


#endif
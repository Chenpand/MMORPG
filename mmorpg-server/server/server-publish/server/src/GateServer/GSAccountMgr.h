#ifndef _GS_ACCOUNT_MGR_H_
#define _GS_ACCOUNT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonMutex.h>
#include <AvalonTime.h>
#include <map>
#include <set>
#include <CLDefine.h>
#include <CLAntiAddictDefine.h>

/**
 *@brief �˺���Ϣ
 */
struct AccountInfo
{
	AccountInfo()
		:accid(0), createTime(0), gmauthority(0), srcZoneId(0), playerId(0), level(0), mapId(0), phoneBindRoleId(0)
	{
		memset(key, 0, sizeof(key));
	}

	//�˺�id
	UInt32	accid;
	//����ʱ��
	UInt32	createTime;
	//����
	UInt8	key[20];
	//gmȨ��
	UInt8	gmauthority;
	//openid
	std::string	openid;
	//qq��Ȩ��Ϣ
	QQVipInfo	qqVipInfo;
	//��Դ1
	std::string	source1;
	//��Դ2
	std::string	source2;
	//ƽ̨
	std::string	pf;
	//�豸id
	std::string did;
	//�豸�ͺ�
	std::string model;
	//�豸�汾
	std::string deviceVersion;

	//��������Ϣ
	AntiAddictInfo antiAddictInfo;

	UInt32		srcZoneId;
	ObjID_t		playerId;
	std::string	name;
	UInt32		level;
	UInt32		mapId;
	//�ֻ��󶨵Ľ�ɫ
	ObjID_t		phoneBindRoleId;
};

struct ReconnInfo
{
	ReconnInfo() : accid(0), sequence(0) {}

	UInt32 accid;
	UInt32 sequence;
};

/**
 *@brief �˺Ź�����
 */
class GSAccountMgr : public Avalon::Singleton<GSAccountMgr>
{
	//��ʱ���ʱʱ��
	const static Avalon::UInt32 KEY_TIMEOUT = 20;

	typedef std::map<UInt32, AccountInfo> AccountMap;

public:
	GSAccountMgr();
	~GSAccountMgr();

	void Init();
	void Final();

public:
	/**
	 *@brief ���һ���˺�
	 */
	void AddAccount(AccountInfo& info);

	/**
	 *@brief ��֤�˺�
	 */
	bool Verify(UInt32 accid, const UInt8* key);

	/**
	 *@brief ��ȡ�˺�
	 */
	bool GetAccount(UInt32 accid, AccountInfo& info);

	/**
	*@brief ɾ���˺�
	*/
	void DelAccount(UInt32 accid);

public:
	/**
	*@brief �����Ҫ���������
	*/
	void AddReconnAccount(UInt32 accid, UInt32 sequence);

	/**
	*@brief ����Ƿ���Ҫ����
	*/
	ReconnInfo GetReconnInfo(UInt32 accid);

	/**
	*@brief ���·�������Ϣ
	*/
	void UpdateAntiAddictTime(UInt32 accid, UInt32 time);

public://�¼�

	void OnTick(const Avalon::Time& now);

private:
	//������  noted by aprilliu ���ڻ������m_Accounts
	Avalon::Mutex m_Mutex;
	//�˺�����
	AccountMap m_Accounts;
	//׼���������˺�
	std::map<UInt32, ReconnInfo> m_ReconnAccounts;
};

#endif

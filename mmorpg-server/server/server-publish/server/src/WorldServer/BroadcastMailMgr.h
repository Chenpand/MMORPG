#ifndef _BROADCASTMAIL_MGR_H_
#define _BROADCASTMAIL_MGR_H_

#include <AvalonSingleton.h>
#include <CLItemDefine.h>
#include <CLRecordCallback.h>
#include <map>

class WSPlayer;

/** 
 *@brief �㲥�ʼ�
 */
struct BroadcastMail
{
	BroadcastMail():id(0), sendtime(0), level(0){}

	//id
	ObjID_t		id;
	//����ʱ��
	UInt32		sendtime;
	//������
	std::string senderName;
	//����
	std::string	title;
	//����
	std::string	content;
	//��Դ
	std::string reason;
	//����
	std::vector<ItemReward>	items;
	//�ȼ�
	UInt16		level;
	//����
	std::string  pf;
	//ְҵ
	UInt16		occu;
	//��ֵ�ܽ��ﵽֵ
	UInt32		playerChargeNum;
};

/**
 *@brief �㲥�ʼ�������
 */
class BroadcastMailMgr : public Avalon::Singleton<BroadcastMailMgr>
{
	typedef std::vector<BroadcastMail>	BroadcastMailVec;

	//�ʼ������Ч��
	const static UInt32 MAIL_VALID_TIME = (30 * DAY_TO_SEC);

public:
	BroadcastMailMgr();
	~BroadcastMailMgr();

public:
	void Init();
	void Final();

public:
	/**
	 *@brief ��һ���ʼ�
	 */
	bool PutMail(const std::string& senderName, const std::string& title, 
		const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, UInt16 level = 0, std::string pf = "",
		UInt8 occu = 0, UInt32 playerChargeNum = 0);

public:
	/**
	 *@brief ��ѯ�ʼ�����
	 */
	bool OnSelectBroadcastMailRet(CLRecordCallback* callback);

	/**
	 *@brief �����¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief �������
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief �Ƿ���������������
	*/
	bool FitPlatform(std::string playerPf, std::string pfs);
	
	/**
	*@brief ���һ���ʼ��Ƿ�Ҫ����
	*/
	bool CheckMailCanSend(WSPlayer* player, BroadcastMail* mail, bool checkOfflineMailTime = true);
private:
	//�㲥�ʼ��б�
	BroadcastMailVec	m_BroadcastMails;
};

#endif

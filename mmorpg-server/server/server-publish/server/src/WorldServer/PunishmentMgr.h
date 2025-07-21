#ifndef _PUNISHMENT_MGR_H_
#define _PUNISHMENT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <vector>
#include <set>
#include <CLDefine.h>
#include <CLRecordCallback.h>
#include <CLCheatDefine.h>

class WSPlayer;

/**
 *@brief �ͷ���Ϣ
 */
struct Punishment
{
	Punishment() :id(0), accid(0), type(0), duetime(0), param(0) {}

	ObjID_t	id;
	UInt32	accid;
	int		type;
	UInt32	duetime;
	UInt32	param;
};

enum ForbidLoginReason
{
	// ͨ�÷�ţ�û������ԭ��
	FORBID_LOGIN_COMMON = 0,

	// ��һ������
	FORBID_LOGIN_CHEAT_FIRST_TIME = 1,
	// ���ף����÷�ͣ
	FORBID_LOGIN_CHEAT_FOREVER = 2,
	// �ű�����
	FORBID_LOGIN_CHEAT_TEAM_COPY = 3,

	// GM
	FORBID_LOGIN_GM = 20,
};

/**
 *@brief �ͷ�������
 */
class PunishmentMgr : public Avalon::Singleton<PunishmentMgr>
{
	typedef std::vector<Punishment>			PunishmentVec;
	typedef std::map<UInt32, PunishmentVec>	PunishmentMap;

	//���ͷ�ʱ��
	const static UInt32	MAX_PUNISH_TIME = 5 * 365 * DAY_TO_SEC;

public:
	PunishmentMgr();
	~PunishmentMgr();

public:
	void Init();
	void Final();

public:
	/**
	 *@brief �ͷ�һ�����
	 */
	void Punish(UInt32 accid, int type, UInt32 time, UInt32 param = 0);
	void UnPunish(UInt32 accid, int type);

	/**
	 *@brief ����ȫ�������б�
	 */
	void SetAllForbidTalkList(const std::vector<UInt32>& players);
	void AddAllForbidTalkPlayer(UInt32 accid);
	void DelAllForbidTalkPlayer(UInt32 accid);

	/**
	 *@brief ��ѯ�ͷ�ʣ��ʱ��
	 */
	UInt32 GetPunishRemainTime(UInt32 accid, UInt8 type);

	/**
	*@brief ��ȡ���ԭ��
	*/
	ForbidLoginReason GetForbidLoginReason(UInt32 accid);

public://�¼�
	/**
	 *@brief ��ѯ���ݷ���
	 */
	bool OnSelectDataRet(CLRecordCallback* callback);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief �������
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief �������л�
	 */
	void OnServerChanged(WSPlayer* player);

	/**
	*@brief �����������
	*/
	void OnReportPlayerCheat(WSPlayer* player, PlayerCheatReason reason, UInt32 param1 = 0, UInt32 param2 = 0);

private:
	/**
	 *@brief ͬ���ͷ�������
	 */
	void SyncPunishment(UInt32 accid) const;

	/**
	 *@brief ɾ���ͷ�
	 */
	void DeletePunishment(ObjID_t id);

private:
	//�ͷ��б�
	PunishmentMap		m_Punishments;
	//ȫ�������б�
	std::set<UInt32>	m_ALLForbidTalkList;
	//��������б�ʱ��
	Avalon::SimpleTimer	m_RequestTimer;
	//�Ƿ�����
	bool				m_bResponse;
};

#endif

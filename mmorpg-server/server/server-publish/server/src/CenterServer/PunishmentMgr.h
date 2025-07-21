#ifndef _PUNISHMENT_MGR_H_
#define _PUNISHMENT_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>

class GameZone;

/**
 *@brief �ͷ�������
 */
class PunishmentMgr : public Avalon::Singleton<PunishmentMgr>
{
	typedef std::map<UInt32, UInt32> PlayerMap;

	//һ�ν���ʱ��
	const static UInt32	FORBITTALK_TIME = DAY_TO_SEC * 30;

public:
	PunishmentMgr();
	~PunishmentMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ����һ�����
	 */
	void ForbitTalk(UInt32 accid);

	/**
	 *@brief ���һ�����
	 */
	void UnForbitTalk(UInt32 accid);

	/**
	 *@brief ���ͽ����б�
	 */
	void SendForbidList(GameZone* zone);

public: //�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	void OnPunished(UInt32 accid);

	/**
	 *@brief ���
	 */
	void OnUnPunished(UInt32 accid);

	/** 
	 *@brief ��Ϸ����������
	 */
	void OnGameZoneConnected(GameZone* zone);

private:
	/**
	 *@brief ɾ������
	 */
	void DeleteForbidTalk(UInt32 accid);

private:
	//�����б�
	PlayerMap	m_PunishmentMap;
};

#endif

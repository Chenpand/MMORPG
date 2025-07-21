#ifndef _WORLD_WORLDBOSS_H_
#define _WORLD_WORLDBOSS_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLGameDefine.h>
#include <CLSysNotify.h>
#include <CLChatProtocol.h>

class WSPlayer;

/**
*@brief ����boss��Ϣ
*/
struct WorldBossInfo
{
	// boss id
	UInt64	uId;
	// ��ͼid
	UInt32	uMapId;
	// �������
	UInt32	uReviveInterval;
	// ����ʱ��
	UInt32	uReviveTime;
	// boss����
	std::string	strName;
	// boss��ǣ�0 ����boss,1 ����boss 2 ��ӪBoss
	UInt8	uFlag;
	// �ϴλ�ɱ�������
	std::string	strLastKiller;
	// ��ע����б�
	std::map<ObjID_t, WSPlayer*>	mapPlayers;
};

/**
*@brief ����boss��Ϣ������
*/
class WorldBossMgr : public Avalon::Singleton<WorldBossMgr>
{
	typedef std::map<UInt32, WorldBossInfo*>	WorldBossInfoMap;
	typedef std::list<UInt32> RefreshTimeList;
	typedef std::map<UInt32, RefreshTimeList> RefreshTimeMap;

public:
	WorldBossMgr();
	~WorldBossMgr();

	/**
	*@brief ע��boss��Ϣ
	*/
	void RegisterBossInfo(UInt32 uNpcId, ObjID_t uId, std::string strName, UInt32 uMapId, UInt32 uReviveInterval, UInt32 uBirthTime, UInt8 uFlag);

	/**
	*@brief ע����ӪBoss��Ϣ
	*/
	void RegisterCountryInfo(UInt32 npcId, std::string& name, UInt32 mapId, std::string& strRefreshTime);

	/**
	*@brief ˢ��boss��Ϣ
	*/
	void RefreshBossInfo(UInt32 uNpcId, ObjID_t uId, UInt8 uStatus, std::string strKiller);

	/**
	*@brief ͬ��boss��Ϣ
	*/
	void SyncBossInfo(WSPlayer *pPlayer);

	/**
	*@brief ����bossˢ������
	*/
	void TraceBoss(WSPlayer *pPlayer, UInt32 uNpcId, UInt8 uFlag);

	/**
	*@brief ȡ������׷��
	*/
	void CancelAllTrace(WSPlayer *pPlayer);

public:	//�¼�
	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time &now);

private:
	
	// by huchenhui
	//void Notify(WorldBossInfo *pBossInfo, UInt32 id, ...);
	void SendNotify(WorldBossInfo* pBossInfo, UInt16 type, UInt8 color, char *word);

	/**
	*@brief ֪ͨbossˢ��
	*/
	template <typename... Args>
	void Notify(WorldBossInfo *pBossInfo, UInt32 id, const Args & ... args)
	{
		if (pBossInfo->mapPlayers.empty())
			return;

		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		SendNotify(pBossInfo, type, color, buffer);
	}


	/**
	*@brief ������ӪBoss����ʱ���
	*/
	UInt32 CalcRebirthTime(UInt32 npcId);

private:
	// ����boss��Ϣ��
	WorldBossInfoMap	m_mapWorldBossInfo;

	//��ӪBoss����ʱ���(ÿ��������ʱ����ҷֲ�������)
	RefreshTimeMap m_RefreshTimeMap;

	// ���Ӷ�ʱ��
	Avalon::SimpleTimer		m_minTimer;
};

#endif


#ifndef _US_PLAYER_H_
#define _US_PLAYER_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLProtocolDefine.h>
#include <CLItemDefine.h>
#include <kingnet_analyzer.h>
#include <udplog_typedef.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#define PLAYERINFO(T) \
	"accid(" << T->GetAccID() << ") playerid(" << T->GetID() << ") name(" << T->GetName() << ")"

class USItemTransaction;
class USItemThing;

/**
*@brief ������
*/
class USPlayer : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(USPlayer, Avalon::NullMutex)
public:

	USPlayer();
	USPlayer(UInt64 roleId, UInt32 accId, UInt32 connId)
	{
		SetID(roleId);
		SetAccID(accId);
		SetConnId(connId);
	}
	virtual ~USPlayer();

	virtual void OnTick(const Avalon::Time& now);

	virtual bool CanSendProtocol() { return true; }

	/**
	*@brief zoneId
	*/
	inline void SetZoneID(UInt32 id) { m_ZoneId = id; }
	inline UInt32 GetZoneID() const { return m_ZoneId; }
	
	/**
	*@brief �˺�
	*/
	inline void SetAccID(UInt32 id) { m_AccId = id; }
	inline UInt32 GetAccID() { return m_AccId; }

	void SetName(const std::string& name) { m_Name = name; }
	const std::string& GetName() const { return m_Name; }

	/**
	*@brief ְҵ
	*/
	inline void SetOccu(UInt32 occu) { m_Occu = occu; }
	inline UInt32 GetOccu() { return m_Occu; }

	/**
	*@brief �ȼ�
	*/
	inline void SetLevel(UInt32 lvl) { m_Level = lvl; }
	inline UInt32 GetLevel() { return m_Level; }

	/**
	*@brief vip�ȼ�
	*/
	inline void SetVipLevel(UInt32 lvl) { m_VipLvl = lvl; }
	inline UInt32 GetVipLevel() { return m_VipLvl; }

	/**
	*@brief װ������
	*/
	inline void SetEquipScore(UInt32 score) { m_EquipScore = score; }
	inline UInt32 GetEquipScore() { return m_EquipScore; }

	/**
	*@brief ����Id
	*/
	inline void SetConnId(UInt32 id) { m_ConnId = id; }
	inline UInt32 GetConnId() { return m_ConnId; }

	/**
	*@brief ��ǰ��������
	*/
	inline void SetCurSceneType(UInt32 type) { m_CurSceneType = type; }
	inline UInt32 GetCurSceneType() { return m_CurSceneType; }

	/**
	*@brief ԭscene
	*/
	inline void SetSceneNodeId(UInt32 id) { m_SceneNodeId = id; }
	inline UInt32 GetSceneNodeId() { return m_SceneNodeId; }

	/**
	*@brief ԭworld
	*/
	inline void SetWorldNodeId(UInt32 id) { m_WorldNodeId = id; }
	inline UInt32 GetWorldNodeId() { return m_WorldNodeId; }

	/**
	*@brief ����id
	*/
	inline void SetGuildId(UInt64 id) { m_GuildId = id; }
	inline UInt64 GetGuildId() { return m_GuildId; }

	/**
	*@brief ����
	*/
	inline void SetAwaken(UInt32 awaken) { m_Awaken = awaken; }
	inline UInt32 GetAwaken() { return m_Awaken; }

	inline void SetServer(std::string server) { m_Server = server; }
	inline const std::string GetServer() const { return m_Server; }
	
	/**
	*@brief �Ƿ�loading
	*/
	inline void SetIsLoading(bool flag) { IsLoading = flag; }
	inline bool GetIsLoading() { return IsLoading; }

	/**
	*@brief ���͵��ͻ���
	*/
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�Scene
	*/
	void SendToScene(Avalon::Packet* packet);
	void SendToScene(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�World
	*/
	void SendToWorld(Avalon::Packet* packet);
	void SendToWorld(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�ScenePlayer
	*/
	void SendToScenePlayer(Avalon::Packet* packet);
	void SendToScenePlayer(Avalon::Protocol& protocol);

	/**
	 *@brief ��������
	 */
	bool BeginItemTransaction(USItemTransaction* transaction);
	void OnCheckItemRet(UInt32 serial, UInt32 result, UInt32 counterTimes);

	bool BeginItemThing(USItemThing* itemThing);
	void BeginItemThingRes(UInt32 result, UInt64 serialId, UInt64 thingId);
	void CommitItemThing(UInt64 thingId, bool isCommit);
	void CommitItemThingRes(UInt32 result, UInt64 thingId);

	/**
	 *@brief ��ý���
	 */
	void AddRewards(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify = false);

	/**
	*@brief ����֪ͨ
	*/
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		SendProtocol(notify);
	}

	/**
	 *@brief ����udplog
	 */
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_TEAM_COPY_UID);
		if (NULL == pLogger) return;

		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#endif

		pLogger->SetUserIP("");
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);
	}


protected:
	// zoneId
	ObjUInt32 m_ZoneId;
	// �˺�Id
	ObjUInt32 m_AccId;
	// ��ɫ��
	ObjString m_Name;
	// ְҵ
	ObjUInt8 m_Occu;
	// �ȼ�
	ObjUInt32 m_Level;
	// vip
	ObjUInt32 m_VipLvl;
	// װ������
	ObjUInt32 m_EquipScore;
	// ����Id
	ObjUInt32 m_ConnId;
	// ��ǰ��������
	ObjUInt32 m_CurSceneType;
	// ԭscene
	ObjUInt32 m_SceneNodeId;
	// ԭworld
	ObjUInt32 m_WorldNodeId;
	// ����id
	ObjUInt64 m_GuildId;
	// ����
	ObjUInt32 m_Awaken;
	// ��������
	ObjString m_Server;
	// �Ƿ�loading
	bool IsLoading;

	// ��������
	USItemTransaction* m_pItemTrans;

	// ��������
	USItemThing* m_pItemThing;
};

#endif

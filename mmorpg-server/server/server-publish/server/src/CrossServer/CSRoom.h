#ifndef _CS_ROOM_H__
#define _CS_ROOM_H__

#include <CLDefine.h>
#include <CLRoomDefine.h>
#include <CLObject.h>
#include "GameZoneMgr.h"

struct PkRoomMatchPlayerInfo;

class CSRoomKickOutPlayerInfo
{
public:
	CSRoomKickOutPlayerInfo(UInt64 playerId, UInt32 kickoutTime);

	~CSRoomKickOutPlayerInfo() {}

	UInt64 GetPlayerId() { return m_PlayerId; }

	UInt32 GetKickOutTime() { return m_KickOutTime; }

public:
	UInt64 m_PlayerId;
	UInt32 m_KickOutTime;
};

class CSRoomInvite
{
public:

	CSRoomInvite(UInt64 invitePlayerId, UInt64 beInvitePlayerId, UInt32 inviteTime) 
		: m_InvitePlayerId(invitePlayerId), m_BeInvitePlayerId(beInvitePlayerId), m_InviteTime(inviteTime) { }

	~CSRoomInvite() { };

public:
	UInt64 GetInvitePlayerId() { return m_InvitePlayerId; }

	UInt64 GetBeInvitePlayerId() { return m_BeInvitePlayerId; }

	UInt32 GetInviteTime() { return m_InviteTime; }

public:
	//������ID
	UInt64 m_InvitePlayerId;
	//��������ID
	UInt64 m_BeInvitePlayerId;
	//����ʱ��
	UInt32 m_InviteTime;
};

class CSRoomSlot
{
public:
	CSRoomSlot();
	~CSRoomSlot();

	void Init(RoomPlayerInfo player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

public:
	UInt32 GetPlayerAccId() { return m_AccId; }
	void SetPlayerAccId(UInt32 accId) { m_AccId = accId; }

	UInt64 GetPlayerId() { return m_PlayerId; }
	void SetPlayerId(UInt64 id) { m_PlayerId = id; }

	std::string GetPlayerName() { return m_PlayerName; }
	void SetPlayerName(std::string name) { m_PlayerName = name; }

	UInt16 GetPlayerLevel() { return m_PlayerLevel; }
	void SetPlayerLevel(UInt16 level) { m_PlayerLevel = level; }

	UInt32 GetPlayerSeasonLevel() { return m_PlayerSeasonLevel; }
	void SetPlayerSeasonLevel(UInt32 seasonLevel) { m_PlayerSeasonLevel = seasonLevel; }

	UInt8 GetPlayerVipLevel() { return m_PlayerVipLevel; }
	void SetPlayerVipLevel(UInt8 vipLevel) { m_PlayerVipLevel = vipLevel; }

	UInt8 GetPlayerOccu() { return m_PlayerOccu; }
	void SetPlayerOccu(UInt8 occu) { m_PlayerOccu = occu; }

	UInt8 GetPlayerAwaken() { return m_PlayerAwaken; }
	void SetPlayerAwaken(UInt8 awaken) { m_PlayerAwaken = awaken; }

	PlayerAvatar GetPlayerAvatar() { return m_PlayerAvatar; }
	void SetPlayerAvatar(PlayerAvatar avatar) { m_PlayerAvatar = avatar; }

	RoomSlotStatus GetStatus() { return m_Status; }
	void SetStatus(RoomSlotStatus status) { m_Status = status; }

	RoomSlotReadyStatus GetReadyStatus() { return m_ReadyStatus; }
	void SetReadyStatus(RoomSlotReadyStatus status) { m_ReadyStatus = status; }

	UInt32 GetSceneId() { return m_SceneId; }
	void SetSceneId(UInt32 sceneId) { m_SceneId = sceneId; }

	UInt32 GetConnId() { return m_ConnId; }
	void SetConnId(UInt32 connId) { m_ConnId = connId; }

	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	UInt32 GetLoseStreak() { return m_LoseStreak; }
	void SetLoseStreak(UInt32 loseStreak) { m_LoseStreak = loseStreak; }

	UInt32 GetWinStreak() { return m_WinStreak; }
	void SetWinStreak(UInt32 winStreak) { m_WinStreak = winStreak; }

	UInt32 GetMatchScore() { return m_MatchScore; }
	void SetMatchScore(UInt32 matchScore) { m_MatchScore = matchScore; }

	UInt32 GetSeasonStar() { return m_SeasonStar; }
	void SetSeasonStar(UInt32 star) { m_SeasonStar = star; }

	UInt32 GetSeasonExp() { return m_SeasonExp; }
	void SetSeasonExp(UInt32 exp) { m_SeasonExp = exp; }

	void GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup status);

	void GetRoomPlayerInfo(RoomPlayerInfo& info);

	RoomSlotGroup GetTargetSlotGroup() { return m_TargetSlotGroup; }
	void SetTargetSlotGroup(RoomSlotGroup slotGroup) { m_TargetSlotGroup = slotGroup; }

	UInt32 GetTargetSlotIndex() { return m_TargetSlotIndex; }
	void SetTargetSlotIndex(UInt32 slotIndex) { m_TargetSlotIndex = slotIndex; }

	UInt32 GetSwapStartTime() { return m_SwapStartTime; }
	void SetSwapStartTime(UInt32 startTime) { m_SwapStartTime = startTime; }

	UInt32 GetBattleCount() { return m_BattleCount; }
	void SetBattleCount(UInt32 count) { m_BattleCount = count; }

	UInt32 GetHeadFrame() { return m_HeadFrame; }
	void SetHeadFrame(UInt32 id) { m_HeadFrame = id; }

	void SetSwapSlotInfo(RoomSlotGroup slotGroup, UInt32 slotIndex, UInt32 startTime);

	bool IsSwapStatus();

	bool CheckSwapInfo(RoomSlotGroup targetSlotGroup, UInt32 targetSlotIndex, UInt32 timeInterval, UInt32 nowTime);

	bool CheckSwapInfoInvaild(UInt32 timeInterval, UInt32 nowTime);

	void ClearSwapInfo();

	void UpdateMatchInfo(UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount);

private:
	UInt32 m_AccId;
	//���ID
	UInt64 m_PlayerId;
	//�������
	std::string m_PlayerName;
	//��ҵȼ�
	UInt16 m_PlayerLevel;
	//��Ҷ�λ
	UInt32 m_PlayerSeasonLevel;
	//vip�ȼ�
	UInt8 m_PlayerVipLevel;
	//���ְҵ
	UInt8 m_PlayerOccu;
	//��Ҿ���
	UInt8 m_PlayerAwaken;
	//���avatar
	PlayerAvatar m_PlayerAvatar;
	//��λ״̬
	RoomSlotStatus m_Status;
	//ս��׼��״̬
	RoomSlotReadyStatus m_ReadyStatus;
	//���ڳ���
	UInt32 m_SceneId;


	//ƥ����Ϣ״̬
	//RoomMatchInfoStatus m_MatchInfoStatus;
	//������
	UInt32 m_LoseStreak;
	//��ʤ��
	UInt32 m_WinStreak;
	//ƥ���
	UInt32 m_MatchScore;
	//��������
	UInt32 m_SeasonStar;
	//��������
	UInt32 m_SeasonExp;
	//ս������
	UInt32 m_BattleCount;
	// ͷ���
	UInt32 m_HeadFrame;

	//ConnId
	UInt32 m_ConnId;

	//����λ��
	RoomSlotGroup m_TargetSlotGroup;
	//�Է���λ������
	UInt32 m_TargetSlotIndex;
	//����λ��ʱ��
	UInt32 m_SwapStartTime;

};

typedef CLVisitor<CSRoomSlot> CSRoomSlotVisitor;

class CSRoom
{
public:
	CSRoom();
	CSRoom(UInt32 id, RoomType roomType, std::string name, UInt8 isLimitPlayerLevel, UInt16 limitPlayerLevel, UInt8 isLimitPlayerSeasonLevel, UInt32 limitPlayerSeasonLevel);
	~CSRoom();

	//��ʼ������
	void InitSlot();

public:
	UInt32 GetID() { return m_ID; }
	void SetID(UInt32 id) { m_ID = id; }

	std::string GetName() { return m_Name; }
	void SetName(std::string name) { m_Name = name; }

	std::string GetPassword() { return m_Password; }
	void SetPassword(std::string password) { m_Password = password; }

	RoomType GetRoomType() { return m_RoomType; }
	void SetRoomType(RoomType type) { m_RoomType = type; }

	RoomStatus GetRoomStatus() { return m_RoomStatus; }
	void SetRoomStatus(RoomStatus status) { m_RoomStatus = status; }

	UInt8 GetIsLimitPlayerLevel() { return m_IsLimitPlayerLevel; }
	void SetIsLimitPlayerLevel(UInt8 isLimitPlayerLevel) { m_IsLimitPlayerLevel = isLimitPlayerLevel; }

	UInt16 GetLimitPlayerLevel() { return m_LimitPlayerLevel; }
	void SetLimitPlayerLevel(UInt16 level) { m_LimitPlayerLevel = level; }

	UInt8 GetIsLimitPlayerSeasonLevel() { return m_IsLimitPlayerSeasonLevel; }
	void SetIsLimitPlayerSeasonLevel(UInt8 isLimitPlayerSeasonLevel) { m_IsLimitPlayerSeasonLevel = isLimitPlayerSeasonLevel; }

	UInt32 GetLimitPlayerSeasonLevel() { return m_LimitPlayerSeasonLevel; }
	void SetLimitPlayerSeasonLevel(UInt32 level) { m_LimitPlayerSeasonLevel = level; }

	//WSPlayer* GetOwner() { return m_Owner; }
	//void SetOwner(WSPlayer* owner) { m_Owner = owner; }

	UInt64 GetOwnerId() { return m_OwnerId; }
	void SetOwnerId(UInt64 ownerId) { m_OwnerId = ownerId; }

	UInt8 GetOwnerOccu() { return m_OwnerOccu; }
	void SetOwnerOccu(UInt8 occu) { m_OwnerOccu = occu; }

	UInt32 GetOwnerSeasonLevel() { return m_OwnerSeasonLevel; }
	void SetOwnerSeasonLevel(UInt32 seasonLevel) { m_OwnerSeasonLevel = seasonLevel; }

	UInt32 GetCreateTime() { return m_CreateTime; }
	void SetCreateTime(UInt32 createTime) { m_CreateTime = createTime; }

	UInt32 GetOpenTime() { return m_OpenTime; }
	void SetOpenTime(UInt32 openTime) { m_OpenTime = openTime; }

	UInt32 GetOwnerOfflineTime() { return m_OwnerOfflineTime; }
	void SetOwnerOfflineTime(UInt32 offlineTime){ m_OwnerOfflineTime = offlineTime; }

public:

	UInt32 CheckPassword(std::string& password);

	//���뷿��
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);

	//�˳�����
	void QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	//��ɢ����
	UInt32 DismissRoom();

	//���ķ���
	void ChangeOwner(UInt64 newOwnerId = 0);

	//���ķ���
	UInt32 CloseSlot(GameZone* zone, RoomPlayerInfo& player,RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 SwapSlot(RoomPlayerInfo& player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 ResponseSwapSlot(RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//�Ƿ���ƥ��
	UInt32 CheckStartReady();

	//ս��׼����ʼ
	UInt32 BattleReadyStart();

	//ս��״̬����
	UInt32 BattleReadyResult(RoomPlayerInfo& player, RoomSlotReadyStatus slotStatus);

	//ƥ����Ҫ����Ϣ
	UInt32 UpdateMatchInfo(ObjID_t playerId, UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount);

	//���ս��״̬
	UInt32 CheckBattleReadySlotStatus();

	//�ܾ�����ս��
	UInt32 RefuseBattle();

	//��ʼƥ��
	UInt32 BattleMatchStart();

public:

	//�򵥷�����Ϣ
	void GetRoomSimpleInfo(RoomSimpleInfo& info);

	//������Ϣ
	void GetRoomInfo(RoomInfo& info);

	//�������Աͬ��������Ϣ
	void SyncRoomSimpleInfo();

	//ͬ�������Ϣ
	void SyncRoomPlayerInfoToWorld(GameZone* zone, ObjID_t playerId, UInt32 roomId, UInt32 quitRoomId, UInt32 quitRoomTime);

	//����ȡ��ƥ����Ϣ
	void SendMatchCencelNotify(RoomPlayerInfo& player, UInt32 notifyId);

	void Broadcast(Avalon::Protocol& protocol);

	//�������Աͬ��������Ϣ
	void SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf = true);

	//��ȡ��������ͷ����������
	void GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize);

	bool IsFull();

	//�Ƿ��Ѿ��ڷ���
	bool IsInRoom(UInt64 playerId);

	//��ȡ����
	RoomSlotGroup GetRoomSlotGroup(UInt64 playerId);

	//��ȡ����λ��
	UInt32 GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex);

	//�������
	UInt32 InvitePlayer(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//���ݱ�������һ�ȡ�������
	UInt64 GetInvitePlayerId(UInt64 beInvitePlayerId);

	//ɾ��������Ϣ
	void DeleteInviteInfo(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//���������Ϣ�Ƿ�ʱ
	UInt32 CheckInviteTimeout(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//��ȡƥ����Ҫ�������Ϣ
	void GetMatchPlayerIds(std::vector<UInt64>& matchPlayerIds);

	//��ȡƥ����Ҫ�������Ϣ
	void GetMatchPlayerInfos(std::vector<PkRoomMatchPlayerInfo>& matchPlayerInfos);

	//��ս�����Ϣ
	void ClearQuitSwapInfo(CSRoomSlot* roomSlot, RoomSlotGroup slotGroup, UInt32 slotIndex);

	//ͬ��������Ϣ
	void SyncKickOutInfo(RoomPlayerInfo& kickOutPlayer, RoomPlayerInfo& player, RoomQuitReason reason);

	//����Ƿ�ոձ��߳�
	UInt32 CheckKickOutInfo(RoomPlayerInfo& player);

	//����avator��Ϣ
	void UpdateAvatar(RoomPlayerInfo& player);

	//����vip�ȼ�
	void UpdateVipLevel(RoomPlayerInfo& player);

	//�������ڳ���
	void UpdateSceneId(RoomPlayerInfo& player);

	bool GetPlayerById(RoomPlayerInfo& info, ObjID_t playerId);

	void QuitPlayerByConnId(UInt32 connId, std::set<UInt32>& dissRoomSet);

public:
	void SendRoomCreateUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomDeleteUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomJoinUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomQuitUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomMatchStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize);
	void SendRoomBattleStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize);

	void SendUdpLog(GameZone* zone, ObjID_t playerId, LogRoomOperationType opType, UInt32 roomPlayerSize);

public: //�¼�
	
	//��������¼�
	void OnPlayerOnline(RoomPlayerInfo& player);

	//��������¼�
	void OnPlayerOffline(RoomPlayerInfo& player);

	//�Ͽ������¼�
	void OnDisconnect(RoomPlayerInfo& player);

	//�����¼�
	void OnReconnectSuccess(RoomPlayerInfo& player);

	//tick
	void OnTick(const Avalon::Time& now);

	//��ʼ����ƥ���¼�
	void OnStartRoomMatch();

	//ȡ������ƥ���¼�
	void OnCancleRoomMatch();

	//��ʼ����ս���¼�
	void OnStartRoomRace();

	//ս�������¼�
	void OnEndRoomRace();

	//�޸ķ����Ա��׼��״̬
	void UpdateSlotReadyStatus(RoomSlotReadyStatus slotStatus);

private:
	UInt32 GetReadyStartTime();

	void ResetReadyStartTime();
	
	void BattleReadySlotTimeOut();

	void SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent);

	void SendBattleCountNotify(std::string name);

	void CheckSwapSlotInfos();

	bool GetPlayerBySlot(RoomPlayerInfo& info, RoomSlotGroup slotGroup, UInt32 slotIndex, bool isNormal = true);

private:
	//����ID
	UInt32 m_ID;

	//������
	std::string m_Name;

	//��������
	std::string m_Password;

	//��������
	RoomType m_RoomType;

	//����״̬
	RoomStatus m_RoomStatus;

	//�Ƿ�������ҵȼ�����
	UInt8 m_IsLimitPlayerLevel;

	//��ҵȼ�����
	UInt16 m_LimitPlayerLevel;

	//�Ƿ�������Ҷ�λ����
	UInt8 m_IsLimitPlayerSeasonLevel;

	//��Ҷ�λ����
	UInt32 m_LimitPlayerSeasonLevel;

	//�������
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>> m_RoomSlotGroup;
	//std::vector<RoomSlot> m_RoomSlots;

	//һ�߶����λ��
	UInt32 m_RoomSlotMaxSize;

	//����ID
	UInt64 m_OwnerId;
	//WSPlayer* m_Owner;

	//����ְҵ
	UInt8 m_OwnerOccu;

	//������λ
	UInt32 m_OwnerSeasonLevel;

	//��������б�
	std::vector<CSRoomInvite> m_RoomInviteList;

	//ս��׼��״̬��ʼʱ��
	UInt32 m_ReadyStartTime;

	//ս��׼��״̬����ʱ��
	UInt32 m_ReadyTimeInterval;

	//�������ʱ��
	UInt32 m_InviteTimeInterval;

	//����λ�ó���ʱ��
	UInt32 m_SwapSlotTimeInterval;

	//���߳������ٴν���ʱ����
	UInt32 m_KickOutTimeInterval;

	//���䴴��ʱ��
	UInt32 m_CreateTime;

	//�����ʱ��
	UInt32 m_OpenTime;

	//��������ʱ��
	UInt32 m_OwnerOfflineTime;

	//���߳��������
	std::vector<CSRoomKickOutPlayerInfo> m_RoomKickOutPlayers;

};

#endif //__ROOM_H__
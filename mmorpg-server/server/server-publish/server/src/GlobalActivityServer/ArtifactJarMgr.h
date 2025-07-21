#ifndef _ARTIFACT_JAR_MGR_H_
#define _ARTIFACT_JAR_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class PlayerArtifactJarLottery : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(PlayerArtifactJarLottery, Avalon::NullMutex);

	PlayerArtifactJarLottery()
	{
		jarId = 0;
		adminServerId = 0;
		centerServerId = 0;
	}

	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("jar_id", jarId)
		CL_DBPROPERTY("player_id", playerId)
		CL_DBPROPERTY("player_name", playerName)
		CL_DBPROPERTY("server_name", serverName)
		CL_DBPROPERTY("admin_id", adminServerId)
		CL_DBPROPERTY("center_id", centerServerId)

		CL_DBPROPERTY_END()

		// ����
	ObjUInt32 jarId;
	// ��ɫID
	ObjString playerId;
	// ��ɫ����
	ObjString playerName;
	// ����������
	ObjString serverName;
	// adminId
	ObjUInt32 adminServerId;
	// centerId
	ObjUInt32 centerServerId;
};

namespace CLProtocol{
	struct ArtifactJarLotteryRecord;
}
class GASPlayer;

class ArtifactJarMgr : public Avalon::Singleton<ArtifactJarMgr>
{
public:
	ArtifactJarMgr();
	~ArtifactJarMgr();

public:

	void OnInit();
	void OnTick(const Avalon::Time& now);

	// ����齱�ʸ��ϱ�
	void HandleGASArtifactJarLotterySignReq(GASPlayer* player, const std::vector<UInt32>& jarIdVec);
	// ����齱��¼����
	void HandleGASArtifactJarLotteryRecordReq(GASPlayer* player, UInt32 jarId);
	// �����������ӽ�����������
	void HandleGASArtifactJarLotteryCfgReq(GASPlayer* player);
	// �����ϴγ齱ʱ��
	bool LoadLastLotteryTime(CLRecordCallback* callback);
	// ���س齱�ʸ�
	bool LoadArtifactJarLottery(CLRecordCallback* callback);
	// ���س齱��¼
	bool LoadArtifactJarLotteryRecord(CLRecordCallback* callback);
	// ���ӳ齱�ʸ�
	void AddPlayerArtifactJarLottery(PlayerArtifactJarLottery& jarLottery, bool isSave = false);
	// ���ӳ齱��¼
	void AddArtifactJarLotteryRecord(UInt32 jarId, const std::string& playerName, const std::string& serverName, UInt64 recordTime, UInt32 itemId, bool isSave = false);
	// ���û״̬
	void SetActivityStatus(UInt32 state);
	// ���ɳ齱��¼
	void LotteryToRecord();

private:

	void ClearDBTable(const std::string& _tableName);

private:

	typedef std::map<std::string, PlayerArtifactJarLottery> PlayerArtifactJarLotteryMap;
	typedef std::map<UInt32, PlayerArtifactJarLotteryMap> ArtifactJarLotteryMap;

	typedef std::vector<CLProtocol::ArtifactJarLotteryRecord> ArtifactJarLotteryRecordVec;
	typedef std::map<UInt32, ArtifactJarLotteryRecordVec> ArtifactJarLotteryRecordMap;

	// ���ݼ��ز���
	UInt32 m_LoadStep;
	// �Ƿ�������
	bool m_IsCheckRecv;
	// �״̬
	UInt32 m_ActivityState;
	// ���һ�γ齱ʱ��
	UInt64 m_LastLotteryTime;
	// �Ƿ����
	bool m_IsDayChange;
	// �齱�ʸ�map,key->����string
	ArtifactJarLotteryMap m_LotteryMap;
	// �齱��¼
	ArtifactJarLotteryRecordMap m_LotteryRecordMap;
};

#endif // !_ARTIFACT_JAR_MGR_H_

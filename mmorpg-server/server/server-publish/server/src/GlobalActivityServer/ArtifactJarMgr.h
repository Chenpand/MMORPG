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

		// 罐子
	ObjUInt32 jarId;
	// 角色ID
	ObjString playerId;
	// 角色名字
	ObjString playerName;
	// 服务器名字
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

	// 处理抽奖资格上报
	void HandleGASArtifactJarLotterySignReq(GASPlayer* player, const std::vector<UInt32>& jarIdVec);
	// 处理抽奖记录请求
	void HandleGASArtifactJarLotteryRecordReq(GASPlayer* player, UInt32 jarId);
	// 处理神器罐子奖励配置请求
	void HandleGASArtifactJarLotteryCfgReq(GASPlayer* player);
	// 加载上次抽奖时间
	bool LoadLastLotteryTime(CLRecordCallback* callback);
	// 加载抽奖资格
	bool LoadArtifactJarLottery(CLRecordCallback* callback);
	// 加载抽奖记录
	bool LoadArtifactJarLotteryRecord(CLRecordCallback* callback);
	// 增加抽奖资格
	void AddPlayerArtifactJarLottery(PlayerArtifactJarLottery& jarLottery, bool isSave = false);
	// 增加抽奖记录
	void AddArtifactJarLotteryRecord(UInt32 jarId, const std::string& playerName, const std::string& serverName, UInt64 recordTime, UInt32 itemId, bool isSave = false);
	// 设置活动状态
	void SetActivityStatus(UInt32 state);
	// 生成抽奖记录
	void LotteryToRecord();

private:

	void ClearDBTable(const std::string& _tableName);

private:

	typedef std::map<std::string, PlayerArtifactJarLottery> PlayerArtifactJarLotteryMap;
	typedef std::map<UInt32, PlayerArtifactJarLotteryMap> ArtifactJarLotteryMap;

	typedef std::vector<CLProtocol::ArtifactJarLotteryRecord> ArtifactJarLotteryRecordVec;
	typedef std::map<UInt32, ArtifactJarLotteryRecordVec> ArtifactJarLotteryRecordMap;

	// 数据加载步骤
	UInt32 m_LoadStep;
	// 是否检查重启
	bool m_IsCheckRecv;
	// 活动状态
	UInt32 m_ActivityState;
	// 最后一次抽奖时间
	UInt64 m_LastLotteryTime;
	// 是否跨天
	bool m_IsDayChange;
	// 抽奖资格map,key->罐子string
	ArtifactJarLotteryMap m_LotteryMap;
	// 抽奖记录
	ArtifactJarLotteryRecordMap m_LotteryRecordMap;
};

#endif // !_ARTIFACT_JAR_MGR_H_

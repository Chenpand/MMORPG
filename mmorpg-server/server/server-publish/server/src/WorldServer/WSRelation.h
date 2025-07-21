#ifndef _WS_RELATION_H_
#define _WS_RELATION_H_

#include <CLRelationDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class CLRecordCallback;
class WSPlayer;

/**
 *@brief 社会关系
 */
class WSRelation : public CLSyncObject
{
public:
	WSRelation(UInt8 type);
	WSRelation(UInt8 type, WSPlayer* player);
	~WSRelation();

public:
	/**
	 *@brief 获取类型
	 */
	inline UInt8 GetType() const{ return m_Type; }
	inline void SetType(UInt8 type)	{ m_Type = type; }

	/**
	 *@brief 设置获取唯一id
	 */
	void SetGuid(ObjID_t guid){ m_Guid = guid; }
	ObjID_t GetGuid() const{ return m_Guid; }

	/**
	 *@brief 获取账号id
	 */
	UInt32 GetAccId() const{ return m_AccId; }

	/**
	 *@brief 设置获取名字
	 */
	void SetName(const std::string& name){ m_Name = name; }
	const std::string& GetName() const{ return m_Name; }

	/**
	 *@brief 设置获取性别
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	 *@brief 设置获取等级
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief 设置获取职业
	 */
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; } 

	/**
	*@brief 设置获取上一次赠送时间
	*/
	void SetGiveResetTime(UInt32 time) { m_giveRestTime = time; }
	UInt32 GetGiveResetTime() const { return m_giveRestTime; }
		
	/**
	 *@brief 设置获取每日赠送次数
	 */
	void SetDayGiftNum(UInt8 num){ m_dayGiveNum = num; }
	UInt8 GetDayGiftNum() const{ return m_dayGiveNum; }
	
	/**
	*@brief 设置获取师徒赠送次数重置时间
	*/
	void SetMasterGiveResetTime(UInt32 time) { m_masterGiveRstTime = time; }
	UInt32 GetMasterGiveResetTime() const { return m_masterGiveRstTime; }

	/**
	*@brief 设置获取师徒每日赠送次数
	*/
	void SetMasterDayGiftNum(UInt8 num){ m_dayMasterGiveNum = num; }
	UInt8 GetMasterDayGiftNum() const{ return m_dayMasterGiveNum; }

	/**
	 *@brief 设置获取是否超级蓝钻
	 */
	void SetVipLv(UInt8 bSuperBlue){ m_vipLv = bSuperBlue; }
	UInt8 GetVipLv() const{ return m_vipLv; }

	/**
	 *@brief 设置获取亲密度
	 */
	void SetIntimacy(UInt16 intimacy);
	UInt16 GetIntimacy() const{ return m_Intimacy; }

	void SetSeasonLv(UInt32 seasonLv) { m_SeasonLv = seasonLv; }
	UInt32 GetSeasonLv() const { return m_SeasonLv; }

	/**
	*@brief 玩家状态，对应枚举FriendMatchStatus
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	 *@brief 设置是否密友                                                                     
	 */
	void SetCloseFriend(UInt8 flag);

	/**
	 *@brief 获取更新时间
	 */
	UInt32 GetAddTime() const{ return m_AddTime; }
	
	/**
	 *@brief 设置上线提醒
	 */
	void SetOnlineNotify(UInt8	bNotify);

	/**
	 *@brief 是否在线                                                                     
	 */
	bool IsOnline() const;
	UInt32 GetOfflineTime();
	void SetOfflineTime(UInt32 offlineTime);

	/**
	*@brief 更新赠送时间
	*/
	void UpdateGivePresentTime();

	/**
	*@brief 更新师徒赠送时间
	*/
	void UpdateMasterGiveTime();

	/**
	*@brief 设置获取师门日常任务状态
	*/
	void SetDailyMasterTaskState(UInt8 state);
	UInt8 GetDailyMasterTaskState() { return m_DailyMasterTaskState; }
	
	/**
	*@brief 更新师门任务状态
	*/
	void UpdateDailyMasterTaskStateOnLogicDayChange();

	/**
	*@brief 同步到client
	*/
	void SyncToClient(WSPlayer* player);

	/**
	*@brief 备注
	*/
	std::string GetRemark() { return m_Remark; }
	void SetRemark(const std::string& remark) { m_Remark = remark; }

	/**
	*@brief 回归玩家标示
	*/
	UInt8 GetRegressionMark() { return m_IsRegress; }
	void SetRegressionMark(bool bRegress) { m_IsRegress = (UInt8)bRegress; }

	/**
	*@brief 头像框
	*/
	UInt8 GetHeadFrame() { return m_HeadFrame; }
	void SetHeadFrame(UInt32 id) { m_HeadFrame = id; }

	/**
	*@brief 公会ID
	*/
	UInt64 GetGuildId() { return m_GuildId; }
	void SetGuildId(UInt64 id) { m_GuildId = id; }

	/**
	*@brief 回归周年称号
	*/
	UInt32 GetReturnYearTitle() { return m_ReturnYearTitle; }
	void SetReturnYearTitle(UInt32 id) { m_ReturnYearTitle = id; }

	/**
	*@brief 设置互为好友标志
	*/
	void SetMutualFriendMark();
	bool IsMutualFriend();
	void ClearMutualFriendMark();
public://编解码
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(m_Name, RA_NAME, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_SeasonLv, RA_SEASONLV, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Level, RA_LEVEL, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Occu, RA_OCCU, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_dayGiveNum, RA_DAYGIFTNUM, RST_FRIEND | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Type, RA_TYPE, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_AddTime, RA_CREATETIME, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_vipLv, RA_VIPLV, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Status, RA_STATUS, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_dayMasterGiveNum, RA_MASTER_DAYGIFTNUM, RST_FRIEND | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_offlineTime, RA_OFFLINE_TIME, RST_FRIEND | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Intimacy, RA_INTIMACY, RST_FRIEND | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_DailyMasterTaskState, RA_DAILY_MASTERTASK_STATE, RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Remark, RA_REMARKS, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_IsRegress, RA_IS_REGRESS, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_Mark, RA_MARK, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_HeadFrame, RA_HEAD_FRAME, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_GuildId, RA_GUILD_ID, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)
		CL_SYNC_PROPERTY(m_ReturnYearTitle, RA_RETURN_YEAR_TITLE, RST_FRIEND | RST_BLACKLIST | RST_STRANGER | RST_MASTER | RST_DISCIPLE)

	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("name",	m_Name)
	CL_DBPROPERTY("seasonlv", m_SeasonLv)
	CL_DBPROPERTY("level",	m_Level)
	CL_DBPROPERTY("occu",	m_Occu)
	CL_DBPROPERTY("giveresttime",	m_giveRestTime)
	CL_DBPROPERTY("daygiftnum",	m_dayGiveNum)
	CL_DBPROPERTY("viplv",	m_vipLv)
	CL_DBPROPERTY("addtime",	m_AddTime)
	CL_DBPROPERTY("intimacy",	m_Intimacy)
	CL_DBPROPERTY("onlinenotify",	m_OnlineNotify)
	CL_DBPROPERTY("type", m_Type)
	CL_DBPROPERTY("master_giveresttime", m_masterGiveRstTime)
	CL_DBPROPERTY("day_master_giftnum", m_dayMasterGiveNum)
	CL_DBPROPERTY("offline_time", m_offlineTime)
	CL_DBPROPERTY("daily_mastertask_state", m_DailyMasterTaskState)
	CL_DBPROPERTY("remark", m_Remark)
	CL_DBPROPERTY("mark", m_Mark)
	CL_DBPROPERTY("head_frame", m_HeadFrame)
	CL_DBPROPERTY("guild_id", m_GuildId)
	CL_DBPROPERTY("return_year_title", m_ReturnYearTitle)
	CL_DBPROPERTY_END()

	/**
	 *@brief 数据库操作
	 */
	void LoadDBData(CLRecordCallback* callback);
	void InsertToDB(ObjID_t owner);
	void SyncToDB();
	void DelMe();

private:
	//类型
	ObjUInt8	m_Type;
	//唯一id
	ObjID_t	m_Guid;
	//对方账号id
	UInt32	m_AccId;
	//对方名字
	ObjString	m_Name;
	//对方段位
	ObjUInt32	m_SeasonLv;
	//性别
	UInt8		m_Sex;
	//对方等级
	ObjUInt16	m_Level;
	//对方职业
	ObjUInt8	m_Occu;
	//赠送重置时间
	ObjUInt32	m_giveRestTime;
	//每日赠送次数
	ObjUInt8	m_dayGiveNum;
	//师傅赠送徒弟重置时间
	ObjUInt32	m_masterGiveRstTime;
	//师傅赠送徒弟每日限制次数
	ObjUInt8	m_dayMasterGiveNum;
	//是否超级蓝钻
	ObjUInt8	m_vipLv;
	//关系建立时间
	ObjUInt32	m_AddTime;
	//亲密度
	ObjUInt16	m_Intimacy;
	//是否上线提醒
	ObjUInt8	m_OnlineNotify;
	//是否密友
	ObjUInt8	m_bCloseFriend;
	//玩家状态(对应枚举FriendMatchStatus)
	ObjUInt8	m_Status;
	//玩家离线时间,时间戳(秒)
	ObjUInt32	m_offlineTime;
	//师门任务状态
	ObjUInt8    m_DailyMasterTaskState;
	//备注
	ObjString	m_Remark;
	//是否是回归玩家
	ObjUInt8	m_IsRegress;
	//标示
	ObjUInt16   m_Mark;
	// 头像框
	ObjUInt32	m_HeadFrame;
	// 公会ID
	ObjUInt64	m_GuildId;
	// 回归周年称号
	ObjUInt32	m_ReturnYearTitle;
};

typedef CLVisitor<WSRelation> RelationVisitor;

#endif

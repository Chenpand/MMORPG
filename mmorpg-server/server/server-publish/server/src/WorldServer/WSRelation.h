#ifndef _WS_RELATION_H_
#define _WS_RELATION_H_

#include <CLRelationDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class CLRecordCallback;
class WSPlayer;

/**
 *@brief ����ϵ
 */
class WSRelation : public CLSyncObject
{
public:
	WSRelation(UInt8 type);
	WSRelation(UInt8 type, WSPlayer* player);
	~WSRelation();

public:
	/**
	 *@brief ��ȡ����
	 */
	inline UInt8 GetType() const{ return m_Type; }
	inline void SetType(UInt8 type)	{ m_Type = type; }

	/**
	 *@brief ���û�ȡΨһid
	 */
	void SetGuid(ObjID_t guid){ m_Guid = guid; }
	ObjID_t GetGuid() const{ return m_Guid; }

	/**
	 *@brief ��ȡ�˺�id
	 */
	UInt32 GetAccId() const{ return m_AccId; }

	/**
	 *@brief ���û�ȡ����
	 */
	void SetName(const std::string& name){ m_Name = name; }
	const std::string& GetName() const{ return m_Name; }

	/**
	 *@brief ���û�ȡ�Ա�
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	 *@brief ���û�ȡ�ȼ�
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief ���û�ȡְҵ
	 */
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; } 

	/**
	*@brief ���û�ȡ��һ������ʱ��
	*/
	void SetGiveResetTime(UInt32 time) { m_giveRestTime = time; }
	UInt32 GetGiveResetTime() const { return m_giveRestTime; }
		
	/**
	 *@brief ���û�ȡÿ�����ʹ���
	 */
	void SetDayGiftNum(UInt8 num){ m_dayGiveNum = num; }
	UInt8 GetDayGiftNum() const{ return m_dayGiveNum; }
	
	/**
	*@brief ���û�ȡʦͽ���ʹ�������ʱ��
	*/
	void SetMasterGiveResetTime(UInt32 time) { m_masterGiveRstTime = time; }
	UInt32 GetMasterGiveResetTime() const { return m_masterGiveRstTime; }

	/**
	*@brief ���û�ȡʦͽÿ�����ʹ���
	*/
	void SetMasterDayGiftNum(UInt8 num){ m_dayMasterGiveNum = num; }
	UInt8 GetMasterDayGiftNum() const{ return m_dayMasterGiveNum; }

	/**
	 *@brief ���û�ȡ�Ƿ񳬼�����
	 */
	void SetVipLv(UInt8 bSuperBlue){ m_vipLv = bSuperBlue; }
	UInt8 GetVipLv() const{ return m_vipLv; }

	/**
	 *@brief ���û�ȡ���ܶ�
	 */
	void SetIntimacy(UInt16 intimacy);
	UInt16 GetIntimacy() const{ return m_Intimacy; }

	void SetSeasonLv(UInt32 seasonLv) { m_SeasonLv = seasonLv; }
	UInt32 GetSeasonLv() const { return m_SeasonLv; }

	/**
	*@brief ���״̬����Ӧö��FriendMatchStatus
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() const { return m_Status; }

	/**
	 *@brief �����Ƿ�����                                                                     
	 */
	void SetCloseFriend(UInt8 flag);

	/**
	 *@brief ��ȡ����ʱ��
	 */
	UInt32 GetAddTime() const{ return m_AddTime; }
	
	/**
	 *@brief ������������
	 */
	void SetOnlineNotify(UInt8	bNotify);

	/**
	 *@brief �Ƿ�����                                                                     
	 */
	bool IsOnline() const;
	UInt32 GetOfflineTime();
	void SetOfflineTime(UInt32 offlineTime);

	/**
	*@brief ��������ʱ��
	*/
	void UpdateGivePresentTime();

	/**
	*@brief ����ʦͽ����ʱ��
	*/
	void UpdateMasterGiveTime();

	/**
	*@brief ���û�ȡʦ���ճ�����״̬
	*/
	void SetDailyMasterTaskState(UInt8 state);
	UInt8 GetDailyMasterTaskState() { return m_DailyMasterTaskState; }
	
	/**
	*@brief ����ʦ������״̬
	*/
	void UpdateDailyMasterTaskStateOnLogicDayChange();

	/**
	*@brief ͬ����client
	*/
	void SyncToClient(WSPlayer* player);

	/**
	*@brief ��ע
	*/
	std::string GetRemark() { return m_Remark; }
	void SetRemark(const std::string& remark) { m_Remark = remark; }

	/**
	*@brief �ع���ұ�ʾ
	*/
	UInt8 GetRegressionMark() { return m_IsRegress; }
	void SetRegressionMark(bool bRegress) { m_IsRegress = (UInt8)bRegress; }

	/**
	*@brief ͷ���
	*/
	UInt8 GetHeadFrame() { return m_HeadFrame; }
	void SetHeadFrame(UInt32 id) { m_HeadFrame = id; }

	/**
	*@brief ����ID
	*/
	UInt64 GetGuildId() { return m_GuildId; }
	void SetGuildId(UInt64 id) { m_GuildId = id; }

	/**
	*@brief �ع�����ƺ�
	*/
	UInt32 GetReturnYearTitle() { return m_ReturnYearTitle; }
	void SetReturnYearTitle(UInt32 id) { m_ReturnYearTitle = id; }

	/**
	*@brief ���û�Ϊ���ѱ�־
	*/
	void SetMutualFriendMark();
	bool IsMutualFriend();
	void ClearMutualFriendMark();
public://�����
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
	 *@brief ���ݿ����
	 */
	void LoadDBData(CLRecordCallback* callback);
	void InsertToDB(ObjID_t owner);
	void SyncToDB();
	void DelMe();

private:
	//����
	ObjUInt8	m_Type;
	//Ψһid
	ObjID_t	m_Guid;
	//�Է��˺�id
	UInt32	m_AccId;
	//�Է�����
	ObjString	m_Name;
	//�Է���λ
	ObjUInt32	m_SeasonLv;
	//�Ա�
	UInt8		m_Sex;
	//�Է��ȼ�
	ObjUInt16	m_Level;
	//�Է�ְҵ
	ObjUInt8	m_Occu;
	//��������ʱ��
	ObjUInt32	m_giveRestTime;
	//ÿ�����ʹ���
	ObjUInt8	m_dayGiveNum;
	//ʦ������ͽ������ʱ��
	ObjUInt32	m_masterGiveRstTime;
	//ʦ������ͽ��ÿ�����ƴ���
	ObjUInt8	m_dayMasterGiveNum;
	//�Ƿ񳬼�����
	ObjUInt8	m_vipLv;
	//��ϵ����ʱ��
	ObjUInt32	m_AddTime;
	//���ܶ�
	ObjUInt16	m_Intimacy;
	//�Ƿ���������
	ObjUInt8	m_OnlineNotify;
	//�Ƿ�����
	ObjUInt8	m_bCloseFriend;
	//���״̬(��Ӧö��FriendMatchStatus)
	ObjUInt8	m_Status;
	//�������ʱ��,ʱ���(��)
	ObjUInt32	m_offlineTime;
	//ʦ������״̬
	ObjUInt8    m_DailyMasterTaskState;
	//��ע
	ObjString	m_Remark;
	//�Ƿ��ǻع����
	ObjUInt8	m_IsRegress;
	//��ʾ
	ObjUInt16   m_Mark;
	// ͷ���
	ObjUInt32	m_HeadFrame;
	// ����ID
	ObjUInt64	m_GuildId;
	// �ع�����ƺ�
	ObjUInt32	m_ReturnYearTitle;
};

typedef CLVisitor<WSRelation> RelationVisitor;

#endif

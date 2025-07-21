#ifndef _WS_MASTER_SYSTEM_H
#define _WS_MASTER_SYSTEM_H

#include <AvalonSimpleTimer.h>
#include "CLRelationDefine.h"
#include "MasterSectRelation.h"
#include "CLMasterSystemDefine.h"
#include "CLRelationProtocol.h"

class WSPlayer;
class MasterSectRelation;
class WSPlayerShareData;

/*
*@brief ʦͽ�ʾ����
*/
class  MsaterDiscipleQuestionnaire
{
public:
	MsaterDiscipleQuestionnaire()
		: guid(0), owner(0), activeTimeType(0), masterType(0), regionId(0), declaration("")
	{}
	~MsaterDiscipleQuestionnaire() {}
public:
	bool SetDataFromClient(CLProtocol::WorldSetDiscipleQuestionnaireReq& req);
	bool SetDataFromClient(CLProtocol::WorldSetMasterQuestionnaireReq& req);
	void LoadFromDB(CLRecordCallback* callback);
	void SynToClient(WSPlayer* player);
	void SynToDB(WSPlayer* player);

public:
	ObjID_t		guid;			//����
	ObjID_t		owner;			//���id
	UInt8		activeTimeType; //����ʱ������
	UInt8		masterType;		//ʦ������
	UInt8		regionId;		//����id
	std::string	declaration;    //����
};

class WSMasterSystem
{
	typedef std::map<ObjID_t, MasterSectRelation*>	MasterSectRelationMap;
	typedef std::vector<ObjID_t> IdVec;
	typedef std::map<ObjID_t, UInt64> TimeStampMap;

public:
	WSMasterSystem();
	~WSMasterSystem();

public:
	/**
	*@breif ���û�ȡӵ����
	*/
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }
	
	/*
	*@brief ���ˢͬ�Ź�ϵ
	*/
	void CheckAndMakeRelations();

	/**
	*@brief ��������ͬ��
	*/
	void LoadRelations();

	/**
	*@brief �������ݷ���
	*/
	void OnLoadDataRet(CLRecordCallback* callback);

	/*
	*#brief �����������
	*/
	void OnLoadPlayerDataRet(CLRecordCallback* callback);

	/**
	*@brief ����
	*/
	void OnHeartbeat(const Avalon::Time& now);

	/*
	*@brief ���¹�ϵ
	*/
	void UpdateRelation();

	/**
	*@brief ���ҡ��Ƴ������ӹ�ϵ
	*/
	bool AddRelation(MasterSectRelation* relation);

	MasterSectRelation* GetRelation(ObjID_t id);

	MasterSectRelation* GetRelation(UInt8 type, ObjID_t id);

	bool HasRelation(UInt8 type);

	bool HasRelation(ObjID_t id);

	bool RemoveRelation(UInt8 type, ObjID_t id);

	void ClearRelations();

	void ClearRelations(UInt8 type);

	
	/**
	*@brief ������ϵ
	*/
	void VisitRelations(MasterSectRelationVisitor& visitor);

	void VisitRelations(UInt8 type, MasterSectRelationVisitor& visitor);

public: 
	/*
	*@brief ��ȡ�ʾ����
	*/
	MsaterDiscipleQuestionnaire& GetQuestionnaire() { return m_Questionnaire; }

	/*
	*@brief ��ȡ���������Ƽ�ʦ��ʱ���
	*/
	UInt64 GetRecommanMasterTimeStamp(ObjID_t masterId);

	/*
	*@brief ��ȡ���������Ƽ�ͽ��ʱ���
	*/
	UInt64 GetRecommanDiscipleTimeStamp(ObjID_t discipleId);

	/*
	*@brief �������������Ƽ�ʦ��ʱ���
	*/
	void SetRecommanMasterTimeStamp(ObjID_t masterId, UInt64 timestamp);

	/*
	*@brief �������������Ƽ�ͽ��ʱ���
	*/
	void SetRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 timestamp);

	/*
	*@brief ������������Ƽ�ʦ��ʱ���
	*/
	bool CheckRecommanMasterTimeStamp(ObjID_t masterId, UInt64 curr_timestamp);

	/*
	*@brief ������������Ƽ�ͽ��ʱ���
	*/
	bool CheckRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 curr_timestamp);

	/*
	*@brief �������ʾ�ƥ����Ŀ
	*/
	static UInt8 CalcQuestionnaireMatchNum(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief ����ʦ�����񷢲��ɹ�
	*/
	static void HandlePublishMasterTaskSuccess(ObjID_t masterId, ObjID_t discipleId);

	/**
	*@brief ��ʦ�ͷ�ʱ���
	*/
	void SetApprenticMasterPunishTime(UInt64 timestamp, bool saveDb = true);
	UInt64 GetApprenticMasterPunishTime() { return m_ApprenticMasterPunishTime; }
	bool CheckApprenticMasterPunishTime();
	
	/**
	*@brief ��ͽ�ͷ�ʱ���
	*/
	void SetRecruitDisciplePunishTime(UInt64 timestamp, bool saveDb = true);
	UInt64 GetRecruitDisciplePunishTime() { return m_RecruitDisciplePunishTime; }
	bool CheckRecruitDisciplePunishTime();

	/**
	*@brief ��ʦֵ
	*/
	void SetGoodTeacherValue(UInt32 value, bool saveDb = false);
	UInt32 GetGoodTeacherValue() { return m_GoodTeacherValue; }

	/**
	*@brief ǿ�ƽ��ʦ�����óͷ�ʱ���
	*/
	void SetApprenticMasterPunishTimeOnRemoveMaster(UInt64 currTime, UInt64 masterOfflineTime);

	/**
	*@brief ǿ�ƽ��ͽ�����óͷ�ʱ���
	*/
	void SetRecruitDisciplePunishTimeOnRemoveDisciple(UInt64 currTime, UInt64 discipleOfflineTime);

	/**
	*@brief ͬ���ͷ�ʱ���
	*/
	void SyncPunishTimeToClient();

	/**
	*@brief ͽ��ѧҵ�ܳɳ�ֵ
	*/
	void SetAcademicTotalGrowth(UInt32 value) { m_AcademicTotalGrowth = value; }
	UInt32 GetAcademicTotalGrowth() { return m_AcademicTotalGrowth; }

	/**
	*@brief ʦ���ճ�����ɳ���ֵ
	*/	
	void SetMasterDailyTaskGrowth(UInt32 value) { m_MasterDailyTaskGrowth = value; }
	UInt32 GetMasterDailyTaskGrowth() { return m_MasterDailyTaskGrowth; }

	/**
	*@brief ʦ��ѧҵ����ɳ���ֵ
	*/
	void SetMasterAcademicTaskGrowth(UInt32 value) { m_MasterAcademicTaskGrowth = value; }
	UInt32 GetMasterAcademicTaskGrowth() { return m_MasterAcademicTaskGrowth; }

	/**
	*@brief ʦ������װ���ɳ���ֵ
	*/
	void SetMasterGiveEquipGrowth(UInt32 value) { m_MasterGiveEquipGrowth = value; }
	UInt32 GetMasterGiveEquipGrowth() { return m_MasterGiveEquipGrowth; }

	/**
	*@brief ʦ�������ɳ���ֵ
	*/
	void SetMasterUplevelGrowth(UInt32 value) { m_MasterUplevelGrowth = value; }
	UInt32 GetMasterUplevelGrowth() { return m_MasterUplevelGrowth; }
	
	/**
	*@brief ʦ����������ɳ���ֵ
	*/
	void SetMasterGiveGiftGrowth(UInt32 value) { m_MasterGiveGiftGrowth = value; }
	UInt32 GetMasterGiveGiftGrowth() { return m_MasterGiveGiftGrowth; }

	/**
	*@brief ��ʦͽ������
	*/
	void SetFinSchDiscipleSum(UInt32 value) { m_FinSchDiscipleSum = value; }
	void AddFinSchDiscipleSum(UInt32 addValue);
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }

	/**
	*brief ����Ƶ�������ֹʱ���
	*/
	UInt32 GetAnnounceProhibitTimeStamp() { return m_AnnounceProhibitTimeStamp; }
	void SetAnnounceProhibitTimeStamp(UInt32 timeStamp) { m_AnnounceProhibitTimeStamp = timeStamp; }
public:
	/*
	*@brief �����µ�ʦͽ��ϵ����
	*/
	static void OnMakeMasterDisciple(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief �����Ƴ�ͽ��
	*/
	static void OnRemoveDisciple(WSPlayer* master, ObjID_t discipleId);

	/*
	*@brief �����Ƴ�ʦ��
	*/
	static void OnRemoveMaster(WSPlayer* disciple, ObjID_t masterId);

	/*
	*@brief ͽ���Ƴ���Ľ����ϵ
	*/
	static void ClearRelationsOnRemoveDisciple(WSPlayer* disciple);

	/*
	*@brief �����ʦ�ɹ�������µ�ʦ��ͬ�Ź�ϵ
	*/
	static void MakeBrotherRelationByMasterMaked(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief ʦ���������߼��غ����ʦ��,ʦ�ֹ�ϵ
	*/
	static void MakeMasterAndBrotherRelationByMasterDataLoaded(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief ��������ͽ�ܵ�ͬ�Ź�ϵ
	*/
	static void MakeAllDiscipleRelations(WSPlayer* master);

	/*
	*@brief ����ʦ��ͬ�Ź�ϵ
	*/
	static void MakeBrotherRelation(WSPlayer* older, WSPlayer* younger);

	/*
	*@brief ����ʦ��ͬ�Ź�ϵ
	*/
	static void MakeMasterRelation(WSPlayer* master, WSPlayer* disciple);

	/*
	*@brief ����ͽ��ͬ�Ź�ϵ
	*/
	static void MakeDiscipleRelation(WSPlayer* master, WSPlayer* disciple);

	/**
	*@brief ������ȡʦ���ճ�������
	*/
	UInt32 OnReceiveMasterDailyTaskReward(UInt8 type, ObjID_t discipeleId);

	/*
	*@brief ������ȡʦ�ųɳ�����
	*/
	UInt32 OnReceiveMasterAcademicReward(UInt32 masterSysGiftId);

	/*
	*@brief �����ʦ
	*/
	static void  HandleDiscipleFinishSchool(WSPlayer* master, WSPlayer* disciple);

	/**
	*@brief �����Զ���ʦ
	*/
	void OnAutomaticFinishSchool(ObjID_t targetId);

	/**
	*@brief ʦ���Զ���ʦ
	*/
	void OnMasterAutomaticFinishSchool(ObjID_t discipleId, WSPlayerShareData* discipleShareData);

	/**
	*@brief ͽ���Զ���ʦ
	*/
	void OnDiscipleAutomaticFinishSchool(ObjID_t masterId, WSPlayerShareData* masterShareData);

	/**
	*@brief �Լ���ͽ�ܳ�ʦ�޸�ͬ������
	*/
	void UpdateMasterSectRelationOnSelfFinSch(ObjID_t masterId);

	/**
	*@brief �Լ���ʦ��ǿ���Զ���ʦĳ��ͽ���޸�ͬ������
	*/
	void UpdateMatserSectRelationOnAutoFinSchOneDiscple(ObjID_t discipleId);

	/**
	*@brief ������ȡʦ���ճ��������
	*/
	void OnRceiveMasterDailyTaskRewardSucc(UInt8 type, ObjID_t discipeleId);

	/**
	*@brief ������ȡʦ�ųɳ��������
	*/
	void OnRceiveMasterAcademicRewardSucc(UInt32 masterSysGiftId);

	/**
	*@brief ����ʦ����ȡ��ʦ������
	*/
	void OnReceiveMasterFinishSchoolRewardSucc(UInt32 giftId, ObjID_t discipleId);

	/**
	*@brief ����ͽ����ȡ��ʦ������
	*/
	void OnReceiveDiscipleFinishSchoolRewardSucc(UInt32 giftId, ObjID_t masterId);

	/**
	*@brief ����ͽ��ʦ���ճ��������
	*/
	void OnMasterDailyTaskComplete();

	/**
	*@brief ���ͳ�ʦ����
	*/
	static void SendFinishSchoolAnnounce(const std::string& masterName, const std::string& discipleName);

	/**
	*@brief  �����ɫ����ɾ��
	*/
	static void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief �㲥��ʦ�¼�
	*/
	static void BroadcastFinschEvent(const std::string& masterName, const std::string& discipleName);
public: //����¼�
	/**
	*@brief �������
	*/
	void OnOnline();

	/**
	*@brief �������
	*/
	void OnOffline();

	/**
	*@brief �߼��ո�
	*/
	void OnLogicDayChange();

	/**
	*@brief �ȼ��ı�
	*/
	void OnLevelChanged();

	/**
	*@brief vip�ȼ��ı�
	*/
	void OnVipLvlChanged();

	/**
	*@brief ְҵ�ı�
	*/
	void OnOccuChanged();

	/**
	*@brief ״̬�ı�
	*/
	void OnStatusChanged(UInt8 status);

	/**
	*@brief ���ָı�
	*/
	static void OnNameChanged(ObjID_t playerId, const std::string& name);
public: //����
	/*
	*@brief ֪ͨ���ɾ����ϵ
	*/
	void SendPlayerDelRelation(MasterSectRelationType type, ObjID_t tarId);

public:
	/**
	*@brief ͬ�������ݿ�
	*/
	void SyncToDB();

	/**
	*@brief ͬ���ȼ������ݿ�
	*/
	static void SyncLevelToDB(ObjID_t playerId, UInt16 level);

	/**
	*@brief ͬ��vip�ȼ������ݿ�
	*/
	static void SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl);

	/**
	*@brief ͬ��ְҵ�����ݿ�
	*/
	static void SyncOccuToDB(ObjID_t playerId, UInt8 occu);

	/**
	*@brief ͬ�����ֵ����ݿ�
	*/
	static void SyncNameToDB(ObjID_t playerId, const std::string& name);

	/**
	*@brief ͬ��ʦ������״̬
	*/
	static void SyncMasterDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state);
	static void SyncDiscipleDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state);

	/**
	*@brief ͬ����ʦ��ʾ�����ݿ�
	*/
	static void SyncFinSchMarkToDB(ObjID_t playerId);
public:
	/**
	*@brief ʦ���ճ�����ʦ��������ȡ״̬
	*/
	void SetRecvingMasterDailyTaskRewardState(UInt64 timesec);
	void ResetRecvingMasterDailyTaskRewardState();

	/**
	*@brief ʦ���ճ�����ͽ�ܽ�����ȡ״̬
	*/
	void SetRecvingDiscipleDailyTaskRewardState(UInt64 timesec);
	void ResetRecvingDiscipleDailyTaskRewardState();

	/**
	*@brief ����콱״̬��ʶ
	*/
	void CheckRecvRewardState(const Avalon::Time& now);
private:
	//ӵ����
	WSPlayer*					m_pOwner;
	
	//ͬ�Ź�ϵ�б�
	MasterSectRelationMap		m_Relations[MSRELATION_MAX];
	
	//�򵥶�ʱ��
	Avalon::SimpleTimer			m_SyncDBTimer;
	
	//ʦͽ�ʾ����
	MsaterDiscipleQuestionnaire m_Questionnaire;
	
	//����ʦ�����Ƽ�ʱ���
	TimeStampMap				m_MasterRecommanTimestamp;
	
	//����ͽ�ܱ��Ƽ�ʱ���
	TimeStampMap				m_DisipleRecommanTimestamp;
	
	//����ʦ��ͽ����ȴʱ��
	UInt32						m_RecommanCoolTime;
	
	//��ʦ�ͷ�ʱ���
	UInt64						m_ApprenticMasterPunishTime;
	
	//����ͽ�ܳͷ�ʱ���
	UInt64						m_RecruitDisciplePunishTime;
	
	//��ʦֵ
	UInt32						m_GoodTeacherValue;

	//ͽ��ѧҵ�ܳɳ�ֵ
	UInt32						m_AcademicTotalGrowth;

	//ʦ���ճ�����ɳ���ֵ
	UInt32					m_MasterDailyTaskGrowth;

	//ʦ��ѧҵ����ɳ���ֵ
	UInt32					m_MasterAcademicTaskGrowth;

	//ʦ������װ���ɳ���ֵ
	UInt32					m_MasterGiveEquipGrowth;

	//ʦ�������ɳ���ֵ
	UInt32					m_MasterUplevelGrowth;

	//ʦ����������ɳ���ֵ
	UInt32					m_MasterGiveGiftGrowth;

	//��ʦͽ������
	UInt32					m_FinSchDiscipleSum;

	//ʦͽ����Ƶ�������ֹʱ���
	UInt32					m_AnnounceProhibitTimeStamp;

	//�Ƿ�������ȡʦ��ʦ���ճ�������
	bool					m_IsRecvingMasterDailyTaskReward;

	//ʦ��ʦ���ճ�������״̬����ʱ���
	UInt64					m_RecvingMasterDailyTaskRewardTime;

	//�Ƿ�������ȡʦ��ͽ���ճ�������
	bool					m_IsRecvingDiscipleDailyTaskReward;

	//ʦ��ͽ���ճ�������״̬����ʱ���
	UInt64					m_RecvingDiscipleDailyTaskRewardTime;
};

#endif
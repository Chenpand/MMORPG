#ifndef _WS_PLAYER_SHARE_DATA_H_
#define _WS_PLAYER_SHARE_DATA_H_

/************************************************************************/
/* �������������                                                                     */
/************************************************************************/

#include "CLTaskDefine.h"
#include "CLObject.h"
#include <CLPlayerDefine.h>
#include <CLRecordCallback.h>

class PlayerShareData;
class WSPlayer;
class WSShareDataLoadCallback;
/**
*@brief ����������״̬
*/
enum PlayerShareDataStatus
{

	PSDS_INIT	 = 1,	//�������
	PSDS_LOADING = 2,	//����������
	PSDS_LOADED	 = 3,   //���ݼ������
};

/**
*@brief ���ݼ������״̬
*/
enum PlayerShareDataLoadedStatus
{
	PSDLS_WORLDDATA_LOADED = 1 << 0,		//World�����Ѿ�����
	PSDLS_SCENEDATA_LOADED = 2 << 1,     //Scene�����Ѿ�����
};

//��ȡ�������������������
enum LoadShareDataReqType
{
	LDSDT_NONE = 0,
	LDSDT_GET_MASTERTASK = 1, //ʦ���鿴ͽ��ʦ������
};


class WSPlayerShareData
{
public:
	WSPlayerShareData();
	~WSPlayerShareData();

public:
	/**
	*@brief ��ȡ��ҽ�ɫid
	*/
	UInt64 GetID() { return m_Guid; }
	void SetID(ObjID_t id) { m_Guid = id; }
	/**
	*@brief ��ȡ�˺�id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() const{ return m_AccId; }

	/**
	*@brief ���û�ȡ�������ʱ��
	*/
	UInt64 GetOfflineTime() { return m_OfflineTime; }
	UInt32 GetOfflineTimeSec() { return (UInt32)(m_OfflineTime / 1000); }
	void SetOfflineTime(UInt64 time) { m_OfflineTime = time; }

	/**
	*@brief �ȼ�
	*/
	UInt16 GetLevel() { return m_level; }
	void SetLevel(UInt16 level) { m_level = level; }

	/**
	*@brief ��ʦͽ������
	*/
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }
	void SetFinSchDiscipleSum(UInt32 sum) { m_FinSchDiscipleSum = sum; };
	void AddFinSchDiscipleSum(UInt32 addValue);
public:
	/**
	*@brief ͬ������ Scene��������
	*/
	void SyncDataFromScene(const PlayerSenceShareData& shareData);
	
	/**
	*@brief ͬ��world����
	*/
	void SyncDataFromWorld(WSPlayer* player);

	/**
	*@brief ͬ��ʦ����������
	*/
	void SyncMasterTaskDataFromScene(const MasterTaskShareData& masterTaskShareData);

	/**
	*@brief ��ȡʦ����������
	*/
	void  GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData);

public:
	//���ݿ����
	void StartLoadData(WSShareDataLoadCallback* cb);

	void LoadGlobalInfo(UInt32 loadReqId);
	void LoadTaskInfo(UInt32 loadReqId);

	/**
	*@brief �������ȫ����Ϣ�ص�
	*/
	void OnLoadGlobalInfo(CLRecordCallback *callback);

	/**
	*@brief �������������Ϣ�ص�
	*/
	void OnLoadTaskInfo(CLRecordCallback *callback);

	/**
	*@brief ���Ӽ�������������ر��μ������
	*/
	UInt8 IncLoadDataCount();

	/**
	*@brief ���ü���״̬
	*/
	void ResetLoadDataStatus();

	/**
	*@brief ��������
	*/
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

	/**
	*@brief �����������
	*/
	void LoadDataFinished();

	/**
	*@brief �����������
	*/
	void LoadingFailed();

	/**
	*@brief ���û�ȡ״̬
	*/
	void SetStatus(UInt8 status) { m_Status = status; }
	UInt8 GetStatus() { return m_Status; }

	bool IsInLoading() { return m_Status == PSDS_LOADING; }

	void SetDataLoadedStatus(UInt8 status);
	bool CheckDataLoadedStatus(UInt8 status);

	bool IsWorldDataLoaded();
	bool IsSceneDataLoaded();

	/**
	*@brief �������ݼ�������id
	*/
	UInt32 IncLoadReqId();
	
	/**
	*@brief ��ȡ���ݼ�������id
	*/
	UInt32 GetLoadReqId() { return m_LoadDataReqId; }

	/**
	*@brief ��Ӽ������ݻص�
	*/
	void AddLoadingCallback(WSShareDataLoadCallback* cb);

	/**
	*@brief ���߼�����ɻص�
	*/
	void LoadFinishedCallback();

	/**
	*#brief ��ʦֵ
	*/
	void SetMasterSysGoodTeacherValue(UInt32 value, bool syncDb = false);
	UInt32 GetMasterSysGoodTeacherValue() { return m_SceneshareData.m_MasterTaskShareData.m_GoodTeachValue; }

	/*
	*@brief ����ͽ��ѧҵ�ܳɳ�ֵ
	*/
	void SetDiscipleGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_AcademicTotalGrowth = num; }
	UInt32 GetDiscipleGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_AcademicTotalGrowth; }

	/*
	*@brief ����ͽ���ճ�����ɳ���ֵ
	*/
	void SetMasterDailyTaskGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterDailyTaskGrowth = num; }
	UInt32 GetMasterDailyTaskGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterDailyTaskGrowth; }

	/*
	*@brief ����ͽ��ʦ��ѧҵ����ɳ���ֵ
	*/
	void SetMasterAcademicTaskGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterAcademicTaskGrowth = num; }
	UInt32 GetMasterAcademicTaskGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterAcademicTaskGrowth; }

	/*
	*@brief ����ͽ��ʦ������װ���ɳ���ֵ
	*/
	void SetMasterGiveEquipGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterGiveEquipGrowth = num; }
	UInt32 GetMasterGiveEquipGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterGiveEquipGrowth; }

	/*
	*@brief ����ͽ��ʦ�������ɳ���ֵ
	*/
	void SetMasterUplevelGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterUplevelGrowth = num; }
	UInt32 GetMasterUplevelGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterUplevelGrowth; }

	/*
	*@brief ����ͽ��ʦ����������ɳ���ֵ
	*/
	void SetMasterGiveGiftGrowthValue(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterGiveGiftGrowth = num; }
	UInt32 GetMasterGiveGiftGrowthValue() const { return m_SceneshareData.m_MasterTaskShareData.m_MasterGiveGiftGrowth; }

	/*
	*@brief ʦͽ���ͨ�����³ǳɳ�ֵ
	*/
	void SetMasterTeamClearDungeonGrowth(UInt32 num) { m_SceneshareData.m_MasterTaskShareData.m_MasterTeamClearDungeonGrowth = num; }
	UInt32 GetMasterTeamClearDungeonGrowth() { return m_SceneshareData.m_MasterTaskShareData.m_MasterTeamClearDungeonGrowth; }

	/**
	*@brief ���ʦ����������
	*/
	void AnalysisTaskBriefInfo(TaskBriefInfo& taskinfo, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finishedTime, UInt8 submitCount);
	bool AddMasterDailyTask(const TaskBriefInfo& taskinfo);
	bool AddMasterAcademicTask(const TaskBriefInfo& taskinfo);

	/**
	*@brief ���ʦ����������
	*/
	void ClearMasterTaskData();
private:
	//���guid
	ObjID_t			m_Guid;
	
	//����˺�id
	UInt32			m_AccId;
	
	//����ʱ��,��λ����
	UInt64			m_OfflineTime;

	//��ҵȼ�
	UInt16			m_level;

	//��ҳ�ʦͽ������
	UInt32			m_FinSchDiscipleSum;
private:
	//���ݼ�����������id
	UInt32          m_LoadDataReqId;

	//���������
	UInt8			m_LoadDataCount;

	//���ؽ���
	UInt64			m_LoadDataSerial;

	//����״̬
	UInt8			m_Status;

	//���ݼ������״̬
	UInt8			m_DataLoadedStatus;

	// ��ȡ������������������� LoadType
	UInt32			m_LoadPlayerType;

	// ����������ҵ���
	ObjID_t			m_RequestTarget;

	// ����shardata��ɻص�
	std::vector<WSShareDataLoadCallback*>    m_LoadingCallback;;

private:
	//���Sence��������
	PlayerSenceShareData m_SceneshareData;
};

/**
*@brief ��ҷ�����
*/
typedef CLVisitor<WSPlayerShareData> PlayerShareDataVisitor;

/**
*@brief ��ѯ����������ݷ���
*/
class SelectRoleShareDataCallback : public CLRecordCallback
{
public:
	SelectRoleShareDataCallback(WSPlayerShareData* shareData, UInt32 loadReqId)
		:m_PlayerId(shareData->GetID()), m_LoadSerial(shareData->IncLoadDataCount()), m_LoadReqId(loadReqId){}
	~SelectRoleShareDataCallback(){}

public:
	virtual void OnFinished(WSPlayerShareData* shareData) = 0;
	virtual void OnFailed(WSPlayerShareData* shareData) = 0;

	UInt32 GetLoadDataReqId() { return m_LoadReqId; }

private:
	void OnFinished();

	void OnLoadFailed();

private:
	//���id
	ObjID_t m_PlayerId;
	//�������
	UInt8	m_LoadSerial;
	//������������id
	UInt32  m_LoadReqId;
};

#endif
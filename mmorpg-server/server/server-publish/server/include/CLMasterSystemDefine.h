#ifndef _CL_MASTER_SYSTEM_DEFINE_H_
#define _CL_MASTER_SYSTEM_DEFINE_H_

/**
*@brief ͬ�Ź�ϵ����
*/
enum MasterSectRelationType
{
	MSRELATION_MASTER = 1,		//ʦ��
	MSRELATION_BROTHER = 2,		//ʦ�ֵ�
	MSRELATION_DISCIPLE = 3,	//ͽ��

	MSRELATION_MAX,
};

/**
*@brief ͬ�Ź�ϵͬ������
*/
enum MasterSectRelationSyncType
{
	MSRST_MASTER = 1 << MSRELATION_MASTER,
	MSRST_BROTHER = 1 << MSRELATION_BROTHER,
	MSRST_DISCIPLE = 1 << MSRELATION_DISCIPLE,
};

/**
*@brief ͬ�Ź�ϵ����
*/
enum MasterSectRelationAttr
{
	MSRA_INVALID,
	MSRA_NAME,		//�Է�����(std::string)
	MSRA_LEVEL,		//�Է��ȼ�(UInt16)
	MSRA_OCCU,		//�Է�ְҵ(UInt8)
	MSRA_TYPE,		//��ϵ(UInt8)
	MSRA_VIPLV,		//vip�ȼ�(UInt8)
	MSRA_STATUS,	//״̬(UInt8)
	MSRA_ISFINSCH,  //�Ƿ��ʦ
};

/**
*@brief ��ʦ��ͽ�ʾ������������
*/
enum QuestionnaireActiveTimeType
{
	QAT_INVALID = 0,
	QAT_ALLDAY = 1, //24Сʱ��Ծ
	QAT_DAY = 2,    //����
	QAT_NIGHT = 3,  //����
	QAT_MAX = QAT_NIGHT,
};

/**
*@brief ��ʦ��ͽ�ʾ�ʦ������
*/
enum QuestionnaireMasterType
{
	QMT_INVALID = 0,
	QMT_STRENGTH = 1,	 //ʵ��ǿ����
	QMT_RESPONSIBLE = 2, //���渺����
	QMT_CHITCHAT = 3,	 //�����罻��
};

/**
*@brief ʦ���ճ�����״̬
*/
enum MasterDailyTaskState
{
	MDTST_UNPUBLISHED = 0,  //δ����
	MDTST_UNCOMPLETE = 1,   //�ѷ�����δ���
	MDTST_UNRECEIVED = 2,   //����ɣ�����ȡ
	MDTST_RECEIVEED = 3,	//����ɣ�����ȡ
	MDTST_UNPUB_UNRECE = 4, //δ����������ȡ ʦ���ſ��������״̬
};
//enum MasterDailyTaskState
//{
//	MDTST_UNPUBLISHED = 0,  //δ����
//	MDTST_UNREPORTED = 1,   //�ѷ�����δ�㱨
//	MDTST_UNRECEIVED = 2,   //�ѻ㱨������ȡ
//	MDTST_RECEIVEED = 3,	//�ѻ㱨������ȡ
//	MDTST_UNPUB_UNRECE = 4, //δ����������ȡ ʦ���ſ��������״̬
//};

/**
*@brief ʦ��ÿ���������״̬
*/
enum MasterDailyTaskCompleteState
{
	MDTCS_NOTCOMPLTETEED = 0, //δ���
	MDTCS_COMPLTETEED = 1,    //���
};

/**
*@brief ʦ��ϵͳ�������
*/
enum MasterSysGiftType
{
	MSGT_MASTER_DAILYTASK = 1,     //ʦ���ճ�ʦ���������
	MSGT_DISCIPLE_DAILYTASK = 2,   //ͽ���ճ�ʦ���������
	MSGT_DISCIPLE_ACADEMIC = 3,    //ͽ�ܳɳ����
	MSGT_FISNISHCL_MASTER = 4,     //ʦ����ʦ���
	MSGT_FISNISHCL_DISCIPLE = 5,   //ͽ�ܳ�ʦ���
};

/**
*@brief ��ȡʦ���ճ�����������
*/
enum MasterSysReceiveDailyTaskRewardType
{
	MSRDTR_MASTER = 1,    //ʦ����ȡ
	MARDTR_DISCIPLE = 2,  //ͽ����ȡ
};

/**
*@brief ʦ�ųɳ�������ȡ״̬
*/
enum MasterAcademicRewardRecvState
{
	MARRS_NOTRECVED = 0,  //û����ȡ
	MARRS_RECVED = 1,     //�Ѿ���ȡ
};

/**
*@brief �Զ���ʦ��������
*/
enum MasterSysAutoFinSchType
{
	MSAFST_MASTER = 1, //ʦ������
	MSAFST_DISCIPLE = 2, //ͽ�ܷ���
	MSAFST_NORMAL = 3, //������ʦ
};

/**
*@brief ʦͽ��������
*/
enum  RelationAnnounceType
{
	RAT_MASTER = 1, //ʦ��������
	RAT_DISCIPLE = 2, //ͽ�ܷ�����
};

#endif
#ifndef _CL_JOB_DATAENTRY_H_
#define _CL_JOB_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>

/**
*@brief ְҵ
*/
enum Occupation
{
    OCCU_NONE = 0,				//��ְҵ
    OCCU_GUIJIANSHI = 10,		//��ʿ
    OCCU_JIANHUN,
    OCCU_KUANGZHANSHI,
    OCCU_GUIQI,
    OCCU_AXIULUO,

    OCCU_SHENQIANGSHOU = 20,	//��ǹ��
    OCCU_MANYOU,
    OCCU_QIANGPAOSHI,
    OCCU_JIXIESHI,
    OCCU_DANYAOZHUANJIA,

    OCCU_MOFASHI = 30,			//ħ��ʦ
    OCCU_YUANSUSHI,
    OCCU_ZHANDOUFASHI,
    OCCU_ZHAOHUANSHI,

    OCCU_GEDOUJIA = 40,			//�񶷼�
    OCCU_QIGONGSHI,
    OCCU_SANDA,
    OCCU_JIEBA,
    OCCU_ROUDAOJIA,

	OCCU_SHENQIANGSHOU_NV = 50,	//��ǹ��Ů
	OCCU_MANYOU_NV,
	OCCU_QIANGPAOSHI_NV,
	OCCU_JIXIESHI_NV,
	OCCU_DANYAOZHUANJIA_NV,

	OCCU_SHENGZHIZHE = 60,	//ʥְ��
	OCCU_QUMO = 61,
	OCCU_SHENGQISHI = 62,	//ʥ��ʿ

	OCCU_MUSHI = 70,		//��ʦ

	//TODO
	OCCU_MAX = 128,
};

/**
*@brief ְҵ������
*/
class JobDataEntry : public Avalon::DataEntry
{
	typedef std::vector<UInt8>	OccuVec;
	typedef std::vector<UInt16>	SkillVec;

public:
    JobDataEntry();
    ~JobDataEntry();

public:
    UInt32 GetKey() const { return (UInt32)occu; }

    bool Read(Avalon::DataRow& row);

	bool CheckTargetOccu(UInt8 occu);

	UInt32 GetInitLevel(UInt16 id);

	UInt32 GetPreSkillLevel(UInt16 id);

	SkillVec GetInitSkill() { return skills; }

	static Occupation GetBaseOccu(UInt8 occu) { return (Occupation)(occu / 10 * 10); }

public:
    Occupation	occu;

	std::string name;

	//�Ƿ񿪷�
	bool	isOpen;

	//ְҵ���ͼ���
	SkillVec skills;

	//�������ͼ���
	SkillVec awakenSkills;

	//Ԥתְ����
	SkillVec preJobSkills;

	//����ְҵ�б�
	OccuVec targetOccus;

	//�ܱ�����
    bool        canCreateRole;

	//תְ������Ʒ
	UInt32	itemId;

	//��ƽ�������ܼ��ܵ���
	UInt32 EqualPvPSP;

	//�Ƿ��ǳԼ�ְҵ
	UInt32 chiJiOccu;
};


class JobDataEntryMgr : public Avalon::DataEntryMgrBase<JobDataEntry>
	, public Avalon::Singleton<JobDataEntryMgr>
{
public:
	virtual bool AddEntry(JobDataEntry* entry);

	// ��ȡ���ŵ�תְְҵ����
	UInt32 GetOpenChangeOccuNum() { return m_OpenChangeOccuVec.size(); }

	// ��ȡ�Լ�ְҵ�б�
	std::vector<UInt32>& GetChiJiOccuVec() { return m_ChiJiOccuVec; }

private:
	std::vector<UInt8> m_OpenChangeOccuVec;

	std::vector<UInt32> m_ChiJiOccuVec;
};

#endif

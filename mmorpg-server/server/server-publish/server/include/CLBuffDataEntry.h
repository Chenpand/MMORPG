#ifndef _CL_BUFF_DATAENTRY_H_
#define _CL_BUFF_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/*enum UnionEntryType
{
	// ��Ч
	UNION_ENTRY_INVALID,
	// �̶���ֵ
	UNION_ENTRY_FIX,
	// ��ȼ��Զ����㣨��ֵ��
	UNION_ENTRY_GROW,
	// ���ȼ���ֵ
	UNION_ENTRY_ARRAY,
};*/

/*struct UnionEntry
{
	UnionEntry()
	{
		type = UNION_ENTRY_INVALID;
		fixValue = 0;
		growValue.a = 0;
		growValue.k = 0;
	}

	UnionEntryType	type;
	UInt32		fixValue;
	struct {
		// ��Ӧ��ֵ���� a + k * (level - 1)
		UInt32	a;
		UInt32	k;
	}			growValue;
	std::vector<UInt32> arrayValue;

	UInt32 GetValue(UInt8 level = 1)
	{
		if (type == UNION_ENTRY_FIX)
		{
			return fixValue;
		}
		else if (type == UNION_ENTRY_GROW)
		{
			return growValue.a + (level - 1) * growValue.k;
		}
		else if (type == UNION_ENTRY_ARRAY)
		{
			if (level >= arrayValue.size())
			{
				return 0;
			}
			return arrayValue[level - 1];
		}

		return 0;
	}

	bool DecodeFromString(std::string str)
	{
		// ��ʽ˵����
		// �̶�ģʽ��ֻ������
		// �ɳ�ģʽ��a;k
		// ����ģʽ��a,b,c,d,e...

		if (str.find(';') != std::string::npos)
		{
			type = UNION_ENTRY_GROW;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ";");
			if (params.size() != 2)
			{
				return false;
			}
			growValue.a = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
			growValue.k = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
		}
		else if (str.find(',') != std::string::npos)
		{
			type = UNION_ENTRY_ARRAY;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ",");
			for (auto& param : params)
			{
				arrayValue.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
			}
		}
		else
		{
			type = UNION_ENTRY_FIX;
			fixValue = Avalon::StringUtil::ConvertToValue<UInt32>(str);
		}

		return true;
	}
};*/

/**
 *@brief buffЧ����������
 */
#define BUFF_EFFECT_PARAM_NUM	5

/**
 *@brief buff������
 */
struct BuffDataEntry  : public Avalon::DataEntry
{
public:
	BuffDataEntry();
	~BuffDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//buff id
	UInt32	id;
	//buff����
	Int16	buffType;
	//��������
	UInt8	workType;
	//��������
	UInt8	overLay;
	//��������
	UInt8	overLayLimit;
	//����ӳɰٷֱ�
	UnionEntry	expAddRate;
	//��Ҽӳɰٷֱ�
	UnionEntry	goldAddRate;
	//������������ٷֱ�
	UInt8	taskDropNumRate;
	//��������ٷֱ�
	UInt8	pinkDropRate;
	//����(����)����ٷֱ�
	UInt8	chestDropRate;
	//����ʱ������
	UInt8	durationType;
	//����ʱ��
	UInt32	duration;
	//��Ч����
	UInt32	effectTimes;
	//���õ��³�
	std::vector<UInt32> applyDungeons;
	// ����1
	Int32 param1;
	// ����2
	Int32 param2;
};

typedef Avalon::DataEntryMgr<BuffDataEntry> BuffDataEntryMgr;

/**
*@brief buff��Ϣ������
*/
struct BuffInfoEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32	id;
	//buffer id
	UInt32	bufferId;
};

typedef Avalon::DataEntryMgr<BuffInfoEntry> BuffInfoEntryMgr;

#endif

#ifndef __CL_EQUIP_SCHEME_DEFINE_H__
#define __CL_EQUIP_SCHEME_DEFINE_H__

#include <CLDefine.h>
#include <AvalonNetStream.h>

//װ����������
enum EquipSchemeType
{
	EQST_NONE	= 0,	
	EQST_EQUIP	= 1,  //װ�����ƺ�
	
};

//װ��������Ϣ
struct EquipSchemeInfo
{
	EquipSchemeInfo()
	{
		guid = 0;
		type = 0;
		id = 0;
		weared = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & guid & type & id & weared & equips;
	}
	//Ψһid
	UInt64		guid;
	//��������
	UInt8		type;
	//����id
	UInt32		id;
	//�Ƿ񴩴�
	UInt8		weared;
	//װ��id
	std::vector<UInt64>	 equips;
};

struct RaceEquipScheme
{
	RaceEquipScheme()
	{
		type = 0;
		id = 0;
		isWear = 0;
		title = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & id & isWear & equips & title;
	}

	//��������
	UInt8		type;
	//����id
	UInt32		id;
	//�Ƿ���
	UInt8		isWear;
	//װ��
	std::vector<UInt64>	equips;
	//������
	//std::vector<UInt64>	assistEquip;
	//�ƺ�
	UInt64		title;
};

#endif
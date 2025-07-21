#include "CLEquipInitAttriOnMakeDataEntry.h"

bool EquipInitAttriOnMakeDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	//Ψһid
	id = row.ReadUInt32();
	//����id
	itemId = row.ReadUInt32();
	//ǿ���ȼ�
	strenthLv = row.ReadUInt32();
	//Ʒ��
	qualityLv = row.ReadUInt32();
	//�Ƿ�����
	isLease = row.ReadUInt8();
	//�Ƽ�ְҵ
	std::string str = row.ReadString();
	if (str.size() > 0)
	{
		std::vector<std::string>	strv;
		Avalon::StringUtil::Split(str, strv, "|");
		for (size_t i = 0; i < strv.size(); ++i)
		{
			fitOccus.push_back(Avalon::StringUtil::ConvertToValue<Int32>(strv[i]));
		}
	}

	//UInt32		fitOccu = row.ReadUInt32();
	//��Դ
	sourceId = row.ReadUInt32();
	
	return true;
}
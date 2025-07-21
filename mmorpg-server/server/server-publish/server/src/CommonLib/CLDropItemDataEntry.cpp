#include "CLDropItemDataEntry.h"

DropItemDataEntry::DropItemDataEntry()
{
	minVipLv = 0;
	maxVipLv = 0;
	vipDropLimitId = 0;
	extra = 0;
}

DropItemDataEntry::~DropItemDataEntry()
{

}

bool DropItemDataEntry::Read(Avalon::DataRow& row)
{
    static UInt32 ID_SEED = 0;
    id = ++ID_SEED;

    projID = row.ReadUInt32();
    std::string strChooseNumSet = row.ReadString();
    if (strChooseNumSet != "-")
    {
        std::vector<std::string> chooseNumSetStrs;
        Avalon::StringUtil::Split(strChooseNumSet, chooseNumSetStrs, "|");

        for (auto str : chooseNumSetStrs)
        {
            UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(str);
            chooseNumSet.push_back(num);
        }
    }

    std::string strNumProbSet = row.ReadString();
    if (strNumProbSet != "-")
    {
        std::vector<std::string> numProbSetStrs;
        Avalon::StringUtil::Split(strNumProbSet, numProbSetStrs, "|");

        for (auto str : numProbSetStrs)
        {
            UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(str);
            numProbSet.push_back(num);
        }
    }

    dataType = (DropDataType)row.ReadUInt32();
    itemID = row.ReadUInt32();
    itemProb = row.ReadUInt32();

    std::string strItemNum = row.ReadString();
    if (strItemNum != "-")
    {
        std::vector<std::string> itemNumStrs;
        Avalon::StringUtil::Split(strItemNum, itemNumStrs, "|");
        if (itemNumStrs.size() == 1) 
        {
            itemMinNum = itemMaxNum = Avalon::StringUtil::ConvertToValue<UInt32>(itemNumStrs[0]);
        }
        else if (itemNumStrs.size() == 2)
        {
            itemMinNum = Avalon::StringUtil::ConvertToValue<UInt32>(itemNumStrs[0]);
            itemMaxNum = Avalon::StringUtil::ConvertToValue<UInt32>(itemNumStrs[1]);
        }
        else
        {
            return false;
        }

		if (itemMinNum > itemMaxNum)
		{
			std::swap(itemMinNum, itemMaxNum);
		}
    }

    isRareControl = (row.ReadUInt32() != 0);
    taskId = row.ReadUInt32();

    std::string strOccuAddition = row.ReadString();
    if (strOccuAddition != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(strOccuAddition, strs, "|");
        for (auto& str : strs)
        {
            occuAddition.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
        }
    }

    occuAdditionProb = row.ReadInt32();
    itemType = row.ReadUInt8();
	notice = row.ReadUInt32();

	std::string strActivityId = row.ReadString();
	if (strActivityId != "-")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(strActivityId, strs, "|");
		for (auto& str : strs)
		{
			activityIds.insert(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
	
	std::string markss = row.ReadString();
	if (markss != "_")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(markss, strs, "|");
		for (auto& str : strs)
		{
			markFlag.set(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
		
	std::string vips = row.ReadString();
	if (vips != "_")
	{
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(vips, ss, "|");
		if (ss.size() != 0 && ss.size() != 2)
		{
			ErrorLogStream << "vips size != 2. id = " << id << LogStream::eos;
			return false;
		}
		if (ss.size() == 2)
		{
			minVipLv = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
			maxVipLv = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1]);
		}
	}

	vipDropLimitId = row.ReadUInt32();

	std::string extraStr = row.ReadString();
	std::bitset<32> tempBitSet;
	if (extraStr != "")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(extraStr, strs, "|");
		for (auto& str : strs)
		{
			tempBitSet.set(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
	extra = tempBitSet.to_ulong();

    return true;
}
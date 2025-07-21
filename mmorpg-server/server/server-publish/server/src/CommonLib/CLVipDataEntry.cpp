#include "CLVipDataEntry.h"
#include "CLVipDefine.h"
#include "DropItemMgr.h"
#include "CLActivityAttributeDefine.h"


VipDataEntry::VipDataEntry()
{
}

VipDataEntry::~VipDataEntry()
{
}

bool VipDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "column num(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	level = row.ReadUInt8();
	giftId = row.ReadUInt8();
	exp = row.ReadUInt32();
	
	std::istringstream stream(row.ReadString());
	char split = 0;
	while (stream)
	{
		ItemReward item;
		item.DecodeString(stream);
		if (item.id == 0) break;
		giftItems.push_back(item);
		stream >> split;
	}

	giftPrePrice = row.ReadUInt32();
	giftDiscountPrice = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");

		std::string str2 = row.ReadString();
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(str2, strVec2, "|");

		if (strVec.size() != strVec2.size())
		{
			ErrorLogStream << "artifact jar discount rate size(" << (UInt32)strVec.size() << ") is not match prob size(" << (UInt32)strVec2.size() << ")!" << LogStream::eos;
			return false;
		}

		for (size_t idx = 0; idx < strVec.size(); ++idx)
		{
			std::vector<std::string> strVec3;
			Avalon::StringUtil::Split(strVec[idx], strVec3, "_");
			if (strVec3.size() != 2)
			{
				ErrorLogStream << "atrifact jar discount rate format(" << str << ") is error!" << LogStream::eos;
				return false;
			}

			ArtifactJarDidscountRandInfo info;
			info.value = Avalon::StringUtil::ConvertToValue<UInt32>(strVec3[0]);
			info.prob = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[idx]);
			info.plan = Avalon::StringUtil::ConvertToValue<UInt8>(strVec3[1]);
			if (info.prob > 0)
			{
				artifactJarDiscountRateInfo.push_back(info);
			}

			if (info.value == 0)
			{
				ErrorLogStream << "Artifact jar discount rate is zero!" << LogStream::eos;
				return false;
			}

			if (info.value > ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE)
			{
				ErrorLogStream << "Artifact jar discount rate(" << info.value << ") is more than max(" << ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE << ")!" << LogStream::eos;
				return false;
			}

			if (info.plan >= 3)
			{
				ErrorLogStream << "Error rand plan(" << info.plan << ")!" << LogStream::eos;
				return false;
			}
		}
	}

	{
		std::string str = row.ReadString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");

		std::string str2 = row.ReadString();
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(str2, strVec2, "|");

		if (strVec.size() != strVec2.size())
		{
			ErrorLogStream << "artifact jar discount effect times size(" << (UInt32)strVec.size() << ") is not match prob size(" << (UInt32)strVec2.size() << ")!" << LogStream::eos;
			return false;
		}

		for (size_t idx = 0; idx < strVec.size(); ++idx)
		{
			ArtifactJarDidscountRandInfo info;
			info.value = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[idx]);
			info.prob = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[idx]);
			if (info.prob > 0)
			{
				artifactJarDiscountEffectTimesInfo.push_back(info);
			}
		}
	}

	{
		std::string str = row.ReadString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");

		std::string str2 = row.ReadString();
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(str2, strVec2, "|");

		if (strVec.size() != strVec2.size())
		{
			ErrorLogStream << "artifact jar discount effect times size(" << (UInt32)strVec.size() << ") is not match prob size(" << (UInt32)strVec2.size() << ")!" << LogStream::eos;
			return false;
		}

		for (size_t idx = 0; idx < strVec.size(); ++idx)
		{
			ArtifactJarDidscountRandInfo info;
			info.value = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[idx]);
			info.prob = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[idx]);
			if (info.prob > 0)
			{
				artifactJarDiscountEffectTimesInfo2.push_back(info);
			}
		}
	}

	return true;
}

VipDataEntryMgr::VipDataEntryMgr():m_MaxVipLevel(0) { }

VipDataEntryMgr::~VipDataEntryMgr() { }

UInt32 VipDataEntryMgr::GetMaxVipLevel()
{
	return m_MaxVipLevel;
}

bool VipDataEntryMgr::AddEntry(VipDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<VipDataEntry>::AddEntry(entry)) return false;

	if (m_MaxVipLevel < entry->level) m_MaxVipLevel = entry->level;

	return true;
}

bool VipDataEntryMgr::RandArtifactJarDiscount(UInt32 vipLevel, UInt32& discountRate, UInt32& discountEffectTimes)
{
	auto vipData = FindEntry(vipLevel);
	if (!vipData)
	{
		ErrorLogStream << "Can't find vip data(" << vipLevel << ")!" << LogStream::eos;
		return false;
	}

	UInt32 plan = 0;
	{
		DiscountRandInfoVec datas = WeightRander::DisorganizeData<ArtifactJarDidscountRandInfo>(vipData->artifactJarDiscountRateInfo);

		std::vector<UInt32> wts;
		for (const auto& data : datas)
		{
			wts.push_back(data.prob);
		}
		Int32 hitIdx = WeightRander::RandWeight(wts);
		if (hitIdx >= (Int32)datas.size() || hitIdx < 0)
		{
			return false;
		}

		discountRate = datas[hitIdx].value;
		plan = datas[hitIdx].plan;
	}

	if (plan == 1)
	{
		DiscountRandInfoVec datas = WeightRander::DisorganizeData<ArtifactJarDidscountRandInfo>(vipData->artifactJarDiscountEffectTimesInfo);

		std::vector<UInt32> wts;
		for (const auto& data : datas)
		{
			wts.push_back(data.prob);
		}
		Int32 hitIdx = WeightRander::RandWeight(wts);
		if (hitIdx >= (Int32)datas.size() || hitIdx < 0)
		{
			return false;
		}

		discountEffectTimes = datas[hitIdx].value;
	}
	else if (plan == 2)
	{
		DiscountRandInfoVec datas = WeightRander::DisorganizeData<ArtifactJarDidscountRandInfo>(vipData->artifactJarDiscountEffectTimesInfo2);

		std::vector<UInt32> wts;
		for (const auto& data : datas)
		{
			wts.push_back(data.prob);
		}
		Int32 hitIdx = WeightRander::RandWeight(wts);
		if (hitIdx >= (Int32)datas.size() || hitIdx < 0)
		{
			return false;
		}

		discountEffectTimes = datas[hitIdx].value;
	}
	else
	{
		ErrorLogStream << "Error rand plan(" << plan << ") in vip level(" << vipLevel << ")!" << LogStream::eos;
		return false;
	}

	return true;
}


VipPrivilegeDataEntry::VipPrivilegeDataEntry()
{
}

VipPrivilegeDataEntry::~VipPrivilegeDataEntry()
{
}

bool VipPrivilegeDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	privilegeType = (VipPrivilegeType)row.ReadUInt32();

	UInt32 maxLevel = VipDataEntryMgr::Instance()->GetMaxVipLevel();

	for (UInt32 i = 0; i <= maxLevel; ++i)
	{
		UInt32 value = row.ReadUInt32();
		values.push_back(value);
	}

	return true;
}


VipPrivilegeDataEntryMgr::VipPrivilegeDataEntryMgr()
{
}

VipPrivilegeDataEntryMgr::~VipPrivilegeDataEntryMgr()
{
}

bool VipPrivilegeDataEntryMgr::AddEntry(VipPrivilegeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<VipPrivilegeDataEntry>::AddEntry(entry)) return false;

	m_VipPrivilegeMap[entry->privilegeType] = entry;

	return true;
}

UInt32 VipPrivilegeDataEntryMgr::FindPrivilegeValueById(UInt32 id, UInt32 level)
{
	VipPrivilegeDataEntry* entry = FindEntry(id);
	if (entry == NULL || level >= entry->values.size()) return 0;
	return entry->values[level];
}

UInt32 VipPrivilegeDataEntryMgr::FindPrivilegeValue(VipPrivilegeType type, UInt32 level)
{
	std::map<VipPrivilegeType, VipPrivilegeDataEntry*>::iterator iter = m_VipPrivilegeMap.find(type);
	if (iter != m_VipPrivilegeMap.end() && level < iter->second->values.size())
	{
		return iter->second->values[level];
	}
	return 0;
}


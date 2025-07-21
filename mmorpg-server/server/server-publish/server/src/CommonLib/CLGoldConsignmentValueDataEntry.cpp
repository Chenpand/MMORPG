/**********************************************************************************

   ע��:
           1. _CUSTOM_*_BEGIN��_CUSTOM_*_END֮��Ĵ����ǿ����ֶ��޸ĵġ�
           2. �����ط��Ĵ��붼��Ҫ�Ķ�!!!!

*********************************************************************************/

#include "CLGoldConsignmentValueDataEntry.h"

// �Զ���ͷ�ļ�
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GoldConsignmentValueDataEntry::GoldConsignmentValueDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GoldConsignmentValueDataEntry::~GoldConsignmentValueDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GoldConsignmentValueDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GoldConsignmentValueDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	value = row.ReadUInt32();
// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GoldConsignmentValueDataEntryMgr::AddEntry(GoldConsignmentValueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GoldConsignmentValueDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	// ����
	if (IsCheckZeroValue(entry->id) && entry->value == 0)
	{
		ErrorLogStream << "gold consignment value error! :" << entry->id << LogStream::eos;
		return false;
	}

	// ���н���ʱ�䲻��С��6�㣬6���ʱ����
	if (GC_VALUE_CLOSE_END_TIME == entry->id && entry->value < 6)
	{
		ErrorLogStream << "close end time error! " << LogStream::eos;
		return false;
	}

	// ���ۼ��
	if (GC_VALUE_MIN_GOLD_NUM == entry->id)
	{
		UInt32 tradeUnitBase = GetValue(GC_VALUE_TRADE_UNIT_BASE);
		if (tradeUnitBase != 0 && entry->value > tradeUnitBase)
		{
			ErrorLogStream << "trade unit base error!" << LogStream::eos;
			return false;
		}
	}

	// ���ۼ��
	if (GC_VALUE_TRADE_UNIT_BASE == entry->id)
	{
		UInt32 minGold = GetValue(GC_VALUE_MIN_GOLD_NUM);
		if (minGold != 0 && minGold > entry->value)
		{
			ErrorLogStream << "trade unit base error!" << LogStream::eos;
			return false;
		}
	}

	m_ValueMap[entry->id] = entry->value;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

void GoldConsignmentValueDataEntryMgr::OnReload()
{
	class TeamCopyValueEntryVisitor :public Avalon::DataEntryVisitor<GoldConsignmentValueDataEntry>
	{
	public:
		TeamCopyValueEntryVisitor() {}

		bool operator()(GoldConsignmentValueDataEntry* entry)
		{
			valueMap[entry->id] = entry->value;
			return true;
		}

	public:
		std::map<UInt32, UInt32>& GetValueMap() { return valueMap; }

	private:
		std::map<UInt32, UInt32> valueMap;
	};

	TeamCopyValueEntryVisitor visitor;
	GoldConsignmentValueDataEntryMgr::Instance()->Visit(visitor);

	m_ValueMap.clear();
	m_ValueMap = visitor.GetValueMap();
}

UInt32 GoldConsignmentValueDataEntryMgr::GetValue(UInt32 type)
{
	auto iter = m_ValueMap.find(type);
	return iter != m_ValueMap.end() ? iter->second : 0;
}

bool GoldConsignmentValueDataEntryMgr::IsCheckZeroValue(UInt32 id)
{
	switch (id)
	{
	case GC_VALUE_AVERAGE_PRICE:
	case GC_VALUE_HANG_UP_TIME:
	case GC_VALUE_MIN_GOLD_NUM:
	case GC_VALUE_MAX_GOLD_NUM:
	case GC_VALUE_COMMISSIOM:
	case GC_VALUE_PRICE_WAVE_RATE:
	case GC_VALUE_RECENT_AVERAGE_PRICE_TIME:
	case GC_VALUE_TRADE_UNIT_BASE:
		return true;

	default:break;
	}
	
	return false;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__


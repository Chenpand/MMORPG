#include "CLMasterSysGiftDataEntry.h"
#include "CLMasterSystemDefine.h"

MasterSysGiftDataEntryBase::MasterSysGiftDataEntryBase()
{
}

MasterSysGiftDataEntryBase::~MasterSysGiftDataEntryBase()
{
}

bool MasterSysGiftDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		ErrorLogStream <<"MasterSysGiftDataEntryBase::Read(Avalon::DataRow& row)" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false; 
	}	

	iD = row.ReadUInt32();
	type = row.ReadUInt32();
	param = row.ReadUInt32();
	giftIdstr = row.ReadString();
	counterKey = row.ReadString();
	rate = row.ReadUInt32();

	return InternalRead();
}

bool MasterSysGiftDataEntry::InternalRead()
{
	std::vector<std::string>	giftIdstrs;
	Avalon::StringUtil::Split(giftIdstr, giftIdstrs, "|");
	for (size_t i = 0; i < giftIdstrs.size(); ++i)
	{
		giftIds.push_back(Avalon::StringUtil::ConvertToValue<Int32>(giftIdstrs[i]));
	}
	return true;
}

bool MasterSysGiftMapDataEntryMgr::AddEntry(MasterSysGiftDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MasterSysGiftDataEntry>::AddEntry(entry)) return false;

	OnAddEntry(entry);

	return true;
}

void MasterSysGiftMapDataEntryMgr::OnAddEntry(MasterSysGiftDataEntry* entry)
{
	if (entry->type == MSGT_MASTER_DAILYTASK)
	{
		m_MasterDailyTaskGift = entry;
	}
	else if (entry->type == MSGT_DISCIPLE_DAILYTASK)
	{
		m_DiscipleDailyTaskGift = entry;
	}
	else if (entry->type == MSGT_DISCIPLE_ACADEMIC)
	{
		m_DiscipleAcademicTaskGiftVec.push_back(entry);
	}
	else if (entry->type == MSGT_FISNISHCL_MASTER)
	{
		m_MasterFinishSchoolGiftVec.push_back(entry);
	}
	else if (entry->type == MSGT_FISNISHCL_DISCIPLE)
	{
		m_DiscipleFinishSchoolGiftVec.push_back(entry);
	}

}

//获取出师礼包最低出师礼包
const MasterSysGiftDataEntry*  MasterSysGiftMapDataEntryMgr::GetMinFinishSchoolDataEntrySpecType(UInt8 type)
{
	class GiftDataVisiter : public MasterSysGiftDataEntryVisitor
	{
	public:
		GiftDataVisiter(UInt8 giftType) : m_giftType(giftType), m_dataEntry(NULL){}
		~GiftDataVisiter()  {}
		virtual bool operator()(MasterSysGiftDataEntry* dataEntry)
		{
			if (dataEntry && dataEntry->type == m_giftType)
			{
				if (!m_dataEntry || dataEntry->param < m_dataEntry->param)
				{
					m_dataEntry = dataEntry;
					return true;
				}
			}
			return true;
		}

		UInt8					m_giftType;				//礼包类型
		MasterSysGiftDataEntry* m_dataEntry;			//领取的礼包数据对象
	};

	GiftDataVisiter visiter(type);
	this->Visit(visiter);
	return visiter.m_dataEntry;
}

//获取指定成长值可获取的礼包
const MasterSysGiftDataEntry* MasterSysGiftMapDataEntryMgr::GetDataEntrySpecic(UInt8 type, UInt32 acadegrowth)
{
	class GiftDataVisiter : public MasterSysGiftDataEntryVisitor
	{
	public:
		GiftDataVisiter(UInt8 giftType, UInt32 growth) : m_giftType(giftType), m_growth(growth), m_dataEntry(NULL){}
		~GiftDataVisiter()  {}
		virtual bool operator()(MasterSysGiftDataEntry* dataEntry)
		{
			if (dataEntry && dataEntry->type == m_giftType)
			{
				if ((!m_dataEntry && dataEntry->param <= m_growth) 
					|| (m_dataEntry && dataEntry->param > m_dataEntry->param && dataEntry->param <= m_growth))
				{
					m_dataEntry = dataEntry;
					return true;
				}
			}
			return true;
		}

		UInt8					m_giftType;				//礼包类型
		UInt32					m_growth;				//成长值
		MasterSysGiftDataEntry* m_dataEntry;			//领取的礼包数据对象
	};

	GiftDataVisiter visiter(type, acadegrowth);
	this->Visit(visiter);

	if (visiter.m_dataEntry == NULL)
	{
		return this->GetMinFinishSchoolDataEntrySpecType(type);
	}
	
	return visiter.m_dataEntry;
}
#include "CLAuctionMagicExpectUpDataEntry.h"

bool AuctionMagicExpectUpDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	type = row.ReadUInt8();

	quality = row.ReadUInt8();

	strengthenLv = row.ReadUInt8();

	expectUpTimes = row.ReadUInt32();

	return true;
}

UInt32 AuctionMagicExpectUpDataEntryMgr::GetExpectUpTimes(UInt8 quality, UInt8 strengthen, UInt8 type)
{
	class Visitior : public AuctionMagicExpectUpDataEntryVisitor
	{
	public:
		Visitior(UInt8 quality_, UInt8 strengthen_, UInt8 type_)
			:m_quality(quality_), m_strengthen(strengthen_), m_type(type_), m_additon(0){}
		virtual bool operator()(AuctionMagicExpectUpDataEntry* dataEntry)
		{
			if (dataEntry->quality == m_quality
				&& dataEntry->strengthenLv == m_strengthen
				&& dataEntry->type == m_type)
			{
				m_additon = dataEntry->expectUpTimes;
				return false;
			}
			return true;
		}

	public:
		UInt8  m_quality;
		UInt8  m_strengthen;
		UInt8  m_type;
		UInt32 m_additon;
	};

	Visitior visitior(quality, strengthen, type);
	Visit(visitior);
	return visitior.m_additon;
}
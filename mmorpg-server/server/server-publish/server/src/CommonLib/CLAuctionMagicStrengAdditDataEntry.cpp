#include "CLAuctionMagicStrengAdditDataEntry.h"

bool AuctionMagicStrengAdditDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	type = row.ReadUInt8();

	strengthenLv = row.ReadUInt8();

	color = row.ReadUInt8();

	addition = row.ReadUInt32();

	return true;
}

UInt32 AuctionMagicStrengAdditDataEntryMgr::GetAddition(UInt8 color, UInt8 strengthen, UInt8 type)
{
	class Visitior : public AuctionMagicStrengAdditDataEntryVisitor
	{
	public:
		Visitior(UInt8 color_, UInt8 strengthen_, UInt8 type)
			: m_type(type), m_color(color_), m_strengthen(strengthen_), m_additon(0){}
		virtual bool operator()(AuctionMagicStrengAdditDataEntry* dataEntry)
		{
			if (dataEntry->color == m_color && dataEntry->strengthenLv == m_strengthen 
				&& dataEntry->type == m_type)
			{
				m_additon = dataEntry->addition;
				return false;
			}
			return true;
		}

	public:
		UInt8  m_type;
		UInt8  m_color;
		UInt8  m_strengthen;
		UInt32 m_additon;
	};

	Visitior visitior(color, strengthen, type);
	Visit(visitior);
	return visitior.m_additon;
}
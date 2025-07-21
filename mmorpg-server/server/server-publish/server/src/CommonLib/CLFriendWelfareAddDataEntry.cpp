#include "CLFriendWelfareAddDataEntry.h"

bool FriendWelfareAddDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();

	std::string intimacySpanStr = row.ReadString();
	std::vector<std::string>	intimacySpanStrVec;
	Avalon::StringUtil::Split(intimacySpanStr, intimacySpanStrVec, "|");
	if (intimacySpanStrVec.size() == 2)
	{
		/*intimacySpans.resize(2);
		intimacySpans[0] = Avalon::StringUtil::ConvertToValue<Int32>(intimacySpanStrVec[0]);
		intimacySpans[1] = Avalon::StringUtil::ConvertToValue<Int32>(intimacySpanStrVec[1]);*/
		intimacySpans.push_back(Avalon::StringUtil::ConvertToValue<Int32>(intimacySpanStrVec[0]));
		intimacySpans.push_back(Avalon::StringUtil::ConvertToValue<Int32>(intimacySpanStrVec[1]));
	}
	expAddProb = row.ReadUInt32();
	return true;
}

UInt32 FriendWelfareAddDataEntryMgr::GetExpAddProb(UInt32 intimacy)
{
	class GetDataEntryVisitor : public FriendWelfareAddDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 intimacy) : m_intimacy(intimacy), m_ratio(0){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(FriendWelfareAddDataEntry* dataEntry)
		{
			if (dataEntry->intimacySpans.size() != 2) return true;
		
			if (dataEntry->intimacySpans[1] == -1 && (Int32)m_intimacy >= dataEntry->intimacySpans[0])
			{
				m_ratio = dataEntry->expAddProb;
				return false;
			}
			else if ((Int32)m_intimacy >= dataEntry->intimacySpans[0] && (Int32)m_intimacy < dataEntry->intimacySpans[1])
			{
				m_ratio = dataEntry->expAddProb;
				return false;
			}
			return true;
		}
	public:
		UInt32 m_intimacy;
		UInt32 m_ratio;
	};

	GetDataEntryVisitor visitor(intimacy);
	this->Visit(visitor);
	return visitor.m_ratio;
}
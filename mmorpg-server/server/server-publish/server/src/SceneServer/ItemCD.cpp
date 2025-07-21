#include "ItemCD.h"

#include "Player.h"

ItemCD::ItemCD()
{
	m_pOwner = NULL;
}

ItemCD::~ItemCD()
{
}

void ItemCD::SetUseTime(UInt8 id, UInt64 usetime, UInt32 maxTime)
{
	CDTime time(usetime, maxTime);
	m_CDs[id] = time;
	SetDirty();
}

UInt64 ItemCD::GetUseTime(UInt8 id) const
{
	CDMap::const_iterator iter = m_CDs.find(id);
	if(iter != m_CDs.end()) return iter->second.endTime;
	return 0;
}

void ItemCD::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	for(CDMap::iterator iter = m_CDs.begin(); iter != m_CDs.end(); ++iter)
	{
		if(iter->second.endTime > CURRENT_TIME.MSec())
		{
			stream & UInt8(iter->first) & UInt32(iter->second.endTime / 1000) & iter->second.maxTime;
		}
	}
	stream & UInt8(0);
}

void ItemCD::Decode(Avalon::NetInputStream& stream)
{
	UInt8 id = 0;
	UInt32 cd = 0;
	UInt32 max = 0;

	stream & id;
	while(id != 0)
	{
		stream & cd & max;
		CDTime time(CURRENT_TIME.MSec() + cd, max);
		//m_CDs.insert(std::make_pair(id, time));
		stream & id;

		InfoLogStream << PLAYERINFO(GetOwner()) << " item cd(" << id << ") deadline(" << time.endTime << ")." << LogStream::eos;
	}
}

void ItemCD::EncodeString(std::ostringstream& os)
{
	for (CDMap::iterator iter = m_CDs.begin(); iter != m_CDs.end(); ++iter)
	{
		if (iter->second.endTime > CURRENT_TIME.MSec())
		{
			os << UInt32(iter->first) << ',' << iter->second.endTime << ',' <<
				iter->second.maxTime << ",1;";

			if (os.str().size() > 256)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " itemcd(size:" << (UInt32)os.str().size() << "," << os.str() << ") is reach max(256)!" << LogStream::eos;

				std::string overStr = os.str().substr(0, 256);
				auto pos = overStr.find_last_of(";");
				std::string correctStr = os.str().substr(0, pos + 1);
				os.str(correctStr);
				return;
			}
		}
	}
}

void ItemCD::DecodeString(std::istringstream& is)
{
	if (is.str().empty())
	{
		return;
	}

	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(is.str(), strVec, ";");
	for (const auto& elem : strVec)
	{
		if (elem.empty())
		{
			continue;
		}

		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(elem, strVec2, ",");
		
		if (strVec2.size() == 3)
		{
			UInt8 id = Avalon::StringUtil::ConvertToValue<UInt8>(strVec2[0]);
			UInt64 cd = Avalon::StringUtil::ConvertToValue<UInt64>(strVec2[1]);
			UInt64 endTime = CURRENT_TIME.MSec() + cd;
			UInt32 maxTime = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[2]);

			CDTime cdTime(endTime, maxTime);
			m_CDs.insert(std::make_pair(id, cdTime));
			SetDirty();
			InfoLogStream << PLAYERINFO(GetOwner()) << " itemcd(" << (UInt32)id << ", eT:" << endTime << ")." << LogStream::eos;
		}
		else if (strVec2.size() == 4)
		{
			UInt8 id = Avalon::StringUtil::ConvertToValue<UInt8>(strVec2[0]);
			UInt64 endTime = Avalon::StringUtil::ConvertToValue<UInt64>(strVec2[1]);
			UInt32 maxTime = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[2]);

			CDTime cdTime(endTime, maxTime);
			m_CDs.insert(std::make_pair(id, cdTime));
		}
		else
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " itemcd format is error!" << LogStream::eos;
		}
	}
}

/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAdventureTeamGradeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

AdventureTeamGradeDataEntry::AdventureTeamGradeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AdventureTeamGradeDataEntry::~AdventureTeamGradeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AdventureTeamGradeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AdventureTeamGradeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	adventureTeamGrade = row.ReadString();
	roleValueTotalScore = row.ReadString();
	singleServerRanking = row.ReadUInt32();
	canJoinSortList = row.ReadUInt32();
	optionalTasks = row.ReadString();

// __CUSTOM_READ_BEGIN__

	if (adventureTeamGrade.empty())
	{
		ErrorLogStream << "Adventure team grade is empty!" << LogStream::eos;
		return false;
	}

	{
		std::vector<std::string> tmpVec;
		Avalon::StringUtil::Split(roleValueTotalScore, tmpVec, "|");
		if (tmpVec.size() == 1)
		{
			if (AdventureTeamGradeDataEntryMgr::Instance()->HasHigestGradeData())
			{
				ErrorLogStream << "There had higest grade, but now another higest grade(" << roleValueTotalScore << ") appear!" << LogStream::eos;
				return false;
			}

			lowerScoreLimit = Avalon::StringUtil::ConvertToValue<UInt64>(tmpVec[0]);
			upperScoreLimit = UINT64_MAX;
		}
		else if (tmpVec.size() == 2)
		{
			lowerScoreLimit = Avalon::StringUtil::ConvertToValue<UInt64>(tmpVec[0]);
			upperScoreLimit = Avalon::StringUtil::ConvertToValue<UInt64>(tmpVec[1]);
		}
		else
		{
			ErrorLogStream << "Role value total score(" << roleValueTotalScore << ")'s format is error!" << LogStream::eos;
			return false;
		}

		if (lowerScoreLimit == 0 || upperScoreLimit == 0)
		{
			ErrorLogStream << "Role value total score(" << roleValueTotalScore << ")'s format is error!" << LogStream::eos;
			return false;
		}
	}

	{
		std::vector<std::string> tmpVec;
		Avalon::StringUtil::Split(optionalTasks, tmpVec, "|");
		for (const auto& elem : tmpVec)
		{
			std::vector<std::string> tmpVec2;
			Avalon::StringUtil::Split(elem, tmpVec2, "_");
			if (tmpVec2.size() != 2)
			{
				ErrorLogStream << "Optional tasks(" << optionalTasks << ")'s format is error!" << LogStream::eos;
				return false;
			}
			UInt32 grade = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[1]);
			UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[0]);
			if (optionalTaskMap.find(grade) != optionalTaskMap.end())
			{
				ErrorLogStream << "Optional tasks(" << optionalTasks << ")'s format is error!" << LogStream::eos;
				return false;
			}
			optionalTaskMap.insert(std::make_pair(grade, num));
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__

bool AdventureTeamGradeDataEntry::IsInScoreRange(UInt64 totalScore)
{
	if (lowerScoreLimit <= totalScore && totalScore <= upperScoreLimit)
	{
		return true;
	}
	return false;
}

bool AdventureTeamGradeDataEntry::IsInRankingRange(UInt32 socreRanking)
{
	if (singleServerRanking == 0)
	{
		return true;
	}

	if (socreRanking >= singleServerRanking)
	{
		return true;
	}

	return false;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool AdventureTeamGradeDataEntryMgr::AddEntry(AdventureTeamGradeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventureTeamGradeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

bool AdventureTeamGradeDataEntryMgr::HasHigestGradeData()
{
	class MyDataEntryVisitor : public Avalon::DataEntryVisitor<AdventureTeamGradeDataEntry>
	{
	public:
		MyDataEntryVisitor() :hasMaxGrade(false){}

		bool operator()(AdventureTeamGradeDataEntry* entry)
		{
			if (entry != NULL)
			{
				if (entry->upperScoreLimit == UINT64_MAX)
				{
					hasMaxGrade = true;
					return false;
				}
			}
			return true;
		}

		// 是否已经有最高评级数据
		bool hasMaxGrade;
	};

	MyDataEntryVisitor visitor;
	Visit(visitor);

	return visitor.hasMaxGrade;
}

AdventureTeamGradeDataEntry* AdventureTeamGradeDataEntryMgr::CalcAdventureTeamGrade(UInt64 totalScore, UInt32 scoreRanking)
{
	class MyDataEntryVisitor : public Avalon::DataEntryVisitor<AdventureTeamGradeDataEntry>
	{
	public:
		MyDataEntryVisitor(UInt64 totalScore, UInt32 scoreRanking)
			: data(NULL), m_TotalScore(totalScore), m_ScoreRanking(scoreRanking)
		{}

		bool operator()(AdventureTeamGradeDataEntry* entry)
		{
			if (!entry) return true;

			if (!entry->IsInScoreRange(m_TotalScore))
			{
				return true;
			}

			if (entry->IsInRankingRange(m_ScoreRanking))
			{
				data = entry;
				return false;
			}

			if (entry->GetKey() <= 1)
			{
				ErrorLogStream << "Adventure team grade data(" << entry->GetKey() << ") is lowerest!" << LogStream::eos;
				return false;
			}

			UInt32 lowerEntryKey = entry->GetKey();
			while (lowerEntryKey)
			{
				lowerEntryKey--;

				auto lowerEntry = AdventureTeamGradeDataEntryMgr::Instance()->FindEntry(lowerEntryKey);
				if (!lowerEntry)
				{
					ErrorLogStream << "Can't find adventure team grad data(" << lowerEntryKey << ")!" << LogStream::eos;
					continue;
				}

				if (lowerEntry->IsInRankingRange(m_ScoreRanking))
				{
					data = lowerEntry;
					return false;
				}
			}

			return true;
		}

		AdventureTeamGradeDataEntry* data;

	private:
		UInt64 m_TotalScore;
		UInt32 m_ScoreRanking;
	};

	MyDataEntryVisitor visitor(totalScore, scoreRanking);
	Visit(visitor);

	return visitor.data;
}

bool AdventureTeamGradeDataEntryMgr::CanJoinSortList(UInt64 score)
{
	class MyDataEntryVisitor : public Avalon::DataEntryVisitor<AdventureTeamGradeDataEntry>
	{
	public:
		MyDataEntryVisitor(UInt64 score)
			: bCanJoin(false), m_Score(score)
		{}

		bool operator()(AdventureTeamGradeDataEntry* entry)
		{
			if (!entry) return true;

			if (entry->canJoinSortList > 0)
			{
				if (entry->lowerScoreLimit <= m_Score)
				{
					bCanJoin = true;
					return false;
				}
			}

			return true;
		}

		bool bCanJoin;

	private:
		UInt64 m_Score;
	};

	MyDataEntryVisitor visitor(score);
	Visit(visitor);

	return visitor.bCanJoin;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__


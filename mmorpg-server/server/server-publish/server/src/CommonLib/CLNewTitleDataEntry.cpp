#include "CLNewTitleDataEntry.h"
#include "CLNewTitleDefine.h"

bool NewTitleItemDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "NewTitleItemDataEntry::Read" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	nameType = row.ReadUInt8();

	ownerType = row.ReadInt8();

	category = row.ReadUInt8();

	categoryParam = row.ReadUInt32();

	name = row.ReadString();

	style = row.ReadInt8();
	return true;
}

UInt32 NewTitleItemDataEntryMgr::GetMasterTitleId()
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(UInt8 category) : m_category(category), m_titleId(0)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->category == m_category)
			{
				m_titleId = dataEntry->GetKey();
				return false;
			}
			return true;
		}
	public:
		UInt8	m_category;
		UInt32  m_titleId;
	};

	Visitor visitor(NWCGRY_MASTER);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);
	return visitor.m_titleId;
}

UInt32 NewTitleItemDataEntryMgr::GetGuidTitleId(UInt8 guidPost)
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(UInt8 category, UInt8 category_param)
			: m_category(category), m_category_param(category_param), m_titleId(0)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->category == m_category
				&& dataEntry->categoryParam == m_category_param)
			{
				m_titleId = dataEntry->GetKey();
				return false;
			}
			return true;
		}
	public:
		UInt8	m_category;
		UInt32  m_category_param;
		UInt32  m_titleId;
	};

	Visitor visitor(NWCGRY_GUILD, guidPost);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);
	return visitor.m_titleId;
}

UInt32 NewTitleItemDataEntryMgr::GetYbtTitleId()
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(UInt8 category) : m_category(category), m_titleId(0)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->category == m_category &&  dataEntry->categoryParam == 0)
			{
				m_titleId = dataEntry->GetKey();
				return false;
			}
			return true;
		}
	public:
		UInt8	m_category;
		UInt32  m_titleId;
	};

	Visitor visitor(NWCGRY_YBT);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);
	return visitor.m_titleId;
}

UInt32 NewTitleItemDataEntryMgr::GetYbtTitleIdForSortList(UInt32 sortRanking)
{
	if (sortRanking >10)
	{
		return 0;
	}
	if (sortRanking >=2 && sortRanking <= 10)
	{
		sortRanking = 2;
	}

	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(UInt8 category, UInt32 categoryParam) : m_category(category), m_categoryParam(categoryParam), m_titleId(0)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->category == m_category && dataEntry->categoryParam == m_categoryParam)
			{
				m_titleId = dataEntry->GetKey();
				return false;
			}
			return true;
		}
	public:
		UInt8	m_category;
		UInt32	m_categoryParam;
		UInt32  m_titleId;
	};

	Visitor visitor(NWCGRY_YBT, sortRanking);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);
	return visitor.m_titleId;
}
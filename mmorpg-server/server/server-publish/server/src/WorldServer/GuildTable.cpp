#include "GuildTable.h"
#include "Guild.h"
#include <CLErrorCode.h>

GuildTable::GuildTable()
{
	
}

UInt32 GuildTable::JoinMember(ObjID_t id, UInt8 seat, UInt8 type)
{
	if (IsFull())
	{
		return ErrorCode::GUILD_TABLE_FULL;
	}

	if (seat >= TABLE_FULL_MEMBER_NUM)
	{
		return ErrorCode::GUILD_TABLE_SEAT_INVALID;
	}

	for (UInt32 i = 0; i < m_Members.size(); i++)
	{
		if (m_Members[i].id == id)
		{
			return ErrorCode::GUILD_TABLE_REPEAT;
		}
		else if (m_Members[i].seat == seat)
		{
			return ErrorCode::GUILD_TABLE_SEAT_HAS_PLAYER;
		}
	}

	WorldGuildTableMember member;
	member.id = id;
	member.seat = seat;
	member.type = type;
	m_Members.push_back(member);

	SetDirty();

	return ErrorCode::SUCCESS;
}

UInt8 GuildTable::GetEmptyPos()
{
	for (UInt8 i = 0; i < TABLE_FULL_MEMBER_NUM; i++)
	{
		bool hasMember = false;
		for (auto member : m_Members)
		{
			if (member.seat == i)
			{
				hasMember = true;
				continue;
			}
		}

		if (!hasMember)
		{
			return i;
		}
	}

	return 0xFF;
}

bool GuildTable::IsFull() const
{
	return m_Members.size() >= TABLE_FULL_MEMBER_NUM;
}

bool GuildTable::IsInTable(ObjID_t id) const
{
	for (UInt32 i = 0; i < m_Members.size(); i++)
	{
		if (m_Members[i].id == id)
		{
			return true;
		}
	}

	return false;
}

void GuildTable::ClearMembers()
{
	m_Members.clear();
	SetDirty();
}

void GuildTable::EraseMember(ObjID_t id)
{
	for (UInt32 i = 0; i < m_Members.size(); i++)
	{
		if (m_Members[i].id == id)
		{
			m_Members.erase(m_Members.begin() + i);
			return;
		}
	}

	SetDirty();
}

std::vector<WorldGuildTableMember> GuildTable::GetMembers()
{
	return m_Members;
}

void GuildTable::DecodeString(std::istringstream& is)
{
	WorldGuildTableMember member;
	UInt8 split;
	UInt32 seat;
	UInt32 type;

	while (is >> member.id)
	{
		is >> split >> seat >> split >> type >> split;
		member.seat = (UInt8)seat;
		member.type = (UInt8)type;

		m_Members.push_back(member);
	}
}

void GuildTable::EncodeString(std::ostringstream& os)
{
	for (auto member : m_Members)
	{
		os << member.id << ":" << (UInt32)member.seat << ":" << (UInt32)member.type << "|";
	}
}
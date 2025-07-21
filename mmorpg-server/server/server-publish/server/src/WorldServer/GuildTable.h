#ifndef __GUILD_TABLE_H__
#define __GUILD_TABLE_H__

#include <CLGuildDefine.h>
#include <CLObjectProperty.h>

// 圆桌会议类型
enum GuildTableType
{
	// 第一次加入
	GUILD_TABLE_TYPE_FIRST,
	// 协助（有奖励）
	GUILD_TABLE_TYPE_HELP,
	// 协助（无奖励）
	GUILD_TABLE_TYPE_HELP_NO_REWARD,
};

struct WorldGuildTableMember
{
	ObjID_t id;
	UInt8 seat;
	// 类型，对应枚举（GuildTableType）
	UInt8 type;
};

class GuildTable : public CLComplexProperty
{
	const static UInt32 TABLE_FULL_MEMBER_NUM = 7;
public:
	GuildTable();

	/**
	*@brief 添加成员，这里会检测目标位置是不是已经有人
	*/
	UInt32 JoinMember(ObjID_t id, UInt8 seat, UInt8 type);

	/**
	*@brief 获取一个空的位置
	*/
	UInt8 GetEmptyPos();

	/**
	*@brief 是否已经人满
	*/
	bool IsFull() const;

	/**
	*@brief 是否在圆桌中
	*/
	bool IsInTable(ObjID_t id) const;

	/**
	*@brief 清空成员
	*/
	void ClearMembers();

	/**
	*@brief 删除成员
	*/
	void EraseMember(ObjID_t id);

	/**
	*@brief 获取所有成员
	*/
	std::vector<WorldGuildTableMember> GetMembers();

public:
	// 编码相关
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	// 各个位置的成员ID
	std::vector<WorldGuildTableMember>		m_Members;
};

#endif
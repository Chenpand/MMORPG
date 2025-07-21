#ifndef __GUILD_TABLE_H__
#define __GUILD_TABLE_H__

#include <CLGuildDefine.h>
#include <CLObjectProperty.h>

// Բ����������
enum GuildTableType
{
	// ��һ�μ���
	GUILD_TABLE_TYPE_FIRST,
	// Э�����н�����
	GUILD_TABLE_TYPE_HELP,
	// Э�����޽�����
	GUILD_TABLE_TYPE_HELP_NO_REWARD,
};

struct WorldGuildTableMember
{
	ObjID_t id;
	UInt8 seat;
	// ���ͣ���Ӧö�٣�GuildTableType��
	UInt8 type;
};

class GuildTable : public CLComplexProperty
{
	const static UInt32 TABLE_FULL_MEMBER_NUM = 7;
public:
	GuildTable();

	/**
	*@brief ��ӳ�Ա���������Ŀ��λ���ǲ����Ѿ�����
	*/
	UInt32 JoinMember(ObjID_t id, UInt8 seat, UInt8 type);

	/**
	*@brief ��ȡһ���յ�λ��
	*/
	UInt8 GetEmptyPos();

	/**
	*@brief �Ƿ��Ѿ�����
	*/
	bool IsFull() const;

	/**
	*@brief �Ƿ���Բ����
	*/
	bool IsInTable(ObjID_t id) const;

	/**
	*@brief ��ճ�Ա
	*/
	void ClearMembers();

	/**
	*@brief ɾ����Ա
	*/
	void EraseMember(ObjID_t id);

	/**
	*@brief ��ȡ���г�Ա
	*/
	std::vector<WorldGuildTableMember> GetMembers();

public:
	// �������
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	// ����λ�õĳ�ԱID
	std::vector<WorldGuildTableMember>		m_Members;
};

#endif
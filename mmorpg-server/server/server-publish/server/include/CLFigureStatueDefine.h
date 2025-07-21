#ifndef _CL_FIGURE_STATUE_H_
#define _CL_FIGURE_STATUE_H_


#include <CLGameDefine.h>

// �����������
enum FigureStatueType
{
	FST_INVALID = 0,
	FST_GUILD,					// �������
	FST_GUILD_ASSISTANT,		// ���ḱ�᳤����
	FST_GUILD_ASSISTANT_TWO,	// ���ḱ�᳤����2

	FST_GUILD_DUNGEON_FIRST,	// ������³��˺���һ��
	FST_GUILD_DUNGEON_SECOND,	// ������³��˺��ڶ���
	FST_GUILD_DUNGEON_THIRD,	// ������³��˺�������
};



// �������
struct FigureStatueInfo
{
	FigureStatueInfo()
	{
		accId = 0;
		roleId = 0;
		occu = 0;
		statueType = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & accId & roleId & name & occu & avatar & statueType & guildName;
	}

	// �˺�id
	UInt32			accId;
	// ��ɫid
	UInt64			roleId;
	// �������
	std::string		name;
	// ���ְҵ
	UInt8			occu;
	// ������
	PlayerAvatar	avatar;
	// �����������
	UInt8			statueType;
	// ��������
	std::string		guildName;
};

typedef std::vector<FigureStatueInfo> FigureStatueVec;

#endif //_CL_FIGURE_STATUE_H_


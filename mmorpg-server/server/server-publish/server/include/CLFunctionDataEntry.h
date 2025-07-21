#ifndef _CL_FUNCTION_DATAENTRY_H_
#define _CL_FUNCTION_DATAENTRY_H_

#include "CLDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLJobDataEntry.h"
#include "CLItemDefine.h"

enum FuncNotifyType
{
	FUNC_ACHIEVEMENT = 4,
	FUNC_PK = 7,
	FUNC_GUILD = 15,
	FUNC_TEAM = 30,
	FUNC_FRIEND = 31,
	FUNC_JAR = 36,
	FUNC_ADVENTURE_TEAM = 86,
	FUNC_ZJSL = 90,
	FUNC_TEAM_COPY = 91,
	FUNC_ADVENTUREPASSSEASON = 93,
	FUNC_TEAM_COPY_TWO = 102,
};

inline bool IsTeamCoyFun(UInt32 funId)
{
	return funId == FUNC_TEAM_COPY || funId == FUNC_TEAM_COPY_TWO;
}

/**
 *@brief ����Ԥ���
 */
class FuncNotifyDataEntry : public Avalon::DataEntry
{
public:
	FuncNotifyDataEntry();
	~FuncNotifyDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	bool GetRewards(const Occupation occu, std::vector<ItemReward>& itemRewards);

public:
	//id
	UInt8	id;

	//��������
	UnlockType type;

	//��������
	UInt32	beginTask;
	//�����ȼ�
	UInt16	beginLevel;
	//��������
	UInt32	endTask;
	//�����ȼ�
	UInt16	endLevel;

	//������Ʒ
	std::vector<OccuItemReward> rewards;
};

typedef Avalon::DataEntryMgr<FuncNotifyDataEntry> FuncNotifyDataEntryMgr;

#endif

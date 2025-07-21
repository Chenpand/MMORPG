#ifndef _WORLD_CHALLENGE_SCRIPT_H_
#define _WORLD_CHALLENGE_SCRIPT_H_

#include <CLDefine.h>

#include <vector>
#include <AvalonScriptFile.h>

struct ChallengeGroup 
{
	ChallengeGroup():id(0), minLevel(0), maxLevel(0){}

	UInt32 id;
	UInt16 minLevel;
	UInt16 maxLevel;
};

/**
 *@brief 跨服挑战赛脚本                                                                     
 */
class WorldChallengeScript : public Avalon::ScriptFile
{
	typedef std::vector<ChallengeGroup> ChallengeGroupVec;

public:
	WorldChallengeScript(Avalon::ScriptState* state);
	~WorldChallengeScript();

public:
	void Init();

public:
	UInt32 GetBeginTime() const{ return m_BeginTime; }
	UInt32 GetEndTime() const{ return m_EndTime; }

	UInt32 GetGroupByLevel(UInt16 level) const;
	UInt32 GetPreGroupByGroup(UInt32 id) const;
	const ChallengeGroup* GetGroup(UInt32 group) const;

public://接口
	/**
	 *@brief 设置活动时间                                                                     
	 */
	void SetActivityTime(const char* begintime, const char* endtime);

	/**
	 *@brief 设置赛区                                                                     
	 */
	void SetGroupInfo(UInt32 id, UInt16 minLevel, UInt16 maxLevel);

private:
	UInt32 StrToTime(const std::string& str);

private:
	//活动开始时间
	UInt32	m_BeginTime;
	//活动结束时间
	UInt32	m_EndTime;

	//分组配置
	ChallengeGroupVec	m_Groups;
};

#endif

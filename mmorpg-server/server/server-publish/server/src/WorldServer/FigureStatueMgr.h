#ifndef _WAX_FIGURE_STATUE_MGR_H_
#define _WAX_FIGURE_STATUE_MGR_H_

#include "FigureStatue.h"

#include <CLGameDefine.h>
#include <CLFigureStatueDefine.h>
#include <AvalonSimpleTimer.h>

class WSPlayer;


// 人物雕像管理类
class FigureStatueMgr : public Avalon::Singleton<FigureStatueMgr>
{
public:
	FigureStatueMgr();
	~FigureStatueMgr();

public:
	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 添加一个人物雕像
	*/
	void AddFigureStatue(FigureStatue* statue);

	/**
	*@brief 创建人物雕像
	*/
	void OnCreateFigureStatue(UInt64 roleId, UInt8 statueType, const std::string& guildName);
	void OnCreateFigureStatue(const FigureStatueInfo& info);

	/**
	*@brief 同步人物雕像
	*/
	void SyncFigureStatues(WSPlayer* player);

	/**
	*@brief 开服更新数据
	*/
	void OnSceneConnected(UInt32 id);

	/**
	*@brief 转换外观
	*/
	PlayerAvatar ConvertToPlayerAvatar(const std::string& avatarStr);
	std::string ConverToPlayerAvatarStr(const PlayerAvatar& avatar);

	/**
	*@brief 获取人物雕像信息流
	*/
	std::string GetFigureStatueInfoStream(const FigureStatueInfo& info);

	/**
	*@brief 公会地下城雕像
	*/
	void AddGuildDungeonFigureStatue(FigureStatue* statue, bool isSave = false);
	void CreateGuildDungeonFigureStatue(UInt64 guildId, UInt64 roleId, UInt8 statueType);
	void ClearGuildDungeonFigureStatue();
	void SyncGuildDungeonFigureStatue(WSPlayer* player);
	void GetGuildDungeonFigureStatue(UInt64 guildId, std::vector<FigureStatueInfo>& statueVec);
	bool IsExistGuildDungeonStatue(UInt64 guildId);

protected:
	/**
	*@brief 获取人物雕像(用于将雕像同步到客户端和scene)
	*/
	void _GetFigureStatueInfos(std::vector<FigureStatueInfo>& infos);
	
	/**
	*@brief 根据雕像类型查找
	*/
	FigureStatue* _FindFigureStatueByStatueType(UInt8 statueType);

	/**
	*@brief 同步人物雕像
	*/
	void _BroadCastFigureStatues();

private:

	typedef std::map<UInt8, FigureStatue*> FigureStatueMap;

	// 索引人物雕像 雕像类型-->雕像结构
	FigureStatueMap	m_IdxFigureStatues;

	// 公会地下城雕像key->公会ID
	std::map<UInt64, FigureStatueMap> m_GuildDungeonFigureStatue;
};

#endif
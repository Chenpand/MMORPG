#ifndef _WAX_FIGURE_STATUE_MGR_H_
#define _WAX_FIGURE_STATUE_MGR_H_

#include "FigureStatue.h"

#include <CLGameDefine.h>
#include <CLFigureStatueDefine.h>
#include <AvalonSimpleTimer.h>

class WSPlayer;


// ������������
class FigureStatueMgr : public Avalon::Singleton<FigureStatueMgr>
{
public:
	FigureStatueMgr();
	~FigureStatueMgr();

public:
	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ���һ���������
	*/
	void AddFigureStatue(FigureStatue* statue);

	/**
	*@brief �����������
	*/
	void OnCreateFigureStatue(UInt64 roleId, UInt8 statueType, const std::string& guildName);
	void OnCreateFigureStatue(const FigureStatueInfo& info);

	/**
	*@brief ͬ���������
	*/
	void SyncFigureStatues(WSPlayer* player);

	/**
	*@brief ������������
	*/
	void OnSceneConnected(UInt32 id);

	/**
	*@brief ת�����
	*/
	PlayerAvatar ConvertToPlayerAvatar(const std::string& avatarStr);
	std::string ConverToPlayerAvatarStr(const PlayerAvatar& avatar);

	/**
	*@brief ��ȡ���������Ϣ��
	*/
	std::string GetFigureStatueInfoStream(const FigureStatueInfo& info);

	/**
	*@brief ������³ǵ���
	*/
	void AddGuildDungeonFigureStatue(FigureStatue* statue, bool isSave = false);
	void CreateGuildDungeonFigureStatue(UInt64 guildId, UInt64 roleId, UInt8 statueType);
	void ClearGuildDungeonFigureStatue();
	void SyncGuildDungeonFigureStatue(WSPlayer* player);
	void GetGuildDungeonFigureStatue(UInt64 guildId, std::vector<FigureStatueInfo>& statueVec);
	bool IsExistGuildDungeonStatue(UInt64 guildId);

protected:
	/**
	*@brief ��ȡ�������(���ڽ�����ͬ�����ͻ��˺�scene)
	*/
	void _GetFigureStatueInfos(std::vector<FigureStatueInfo>& infos);
	
	/**
	*@brief ���ݵ������Ͳ���
	*/
	FigureStatue* _FindFigureStatueByStatueType(UInt8 statueType);

	/**
	*@brief ͬ���������
	*/
	void _BroadCastFigureStatues();

private:

	typedef std::map<UInt8, FigureStatue*> FigureStatueMap;

	// ����������� ��������-->����ṹ
	FigureStatueMap	m_IdxFigureStatues;

	// ������³ǵ���key->����ID
	std::map<UInt64, FigureStatueMap> m_GuildDungeonFigureStatue;
};

#endif
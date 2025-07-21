#ifndef _BS_SORT_LIST_MGR_H_
#define _BS_SORT_LIST_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>

struct ChijiSortListEntry
{
	ChijiSortListEntry() : sortType(0), playerId(0), score(0), connId(0), playerOccu(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sortType & serverName & playerId & playerName & playerOccu & score;
	}

	UInt8			sortType;
	std::string		serverName;
	UInt64			playerId;
	std::string		playerName;
	UInt32			score;
	UInt32			connId;
	UInt32			playerOccu;

	void SerializeForClient(Avalon::NetOutputStream& stream)
	{
		stream & playerId & playerName & playerOccu & score;
	}

	UInt64 GetSortFactor() const
	{
		return score;
	}
};

struct ChiJiPlayerBestRank
{
	ChiJiPlayerBestRank() : playerId(0), connId(0), rank(0) {}

	UInt64 playerId;
	UInt32 connId;
	UInt32 rank;
};

class GameZone;

class BSSortListMgr : public Avalon::Singleton<BSSortListMgr>
{
	typedef std::list<ChijiSortListEntry>	SortEntryList;

public:
	BSSortListMgr();
	~BSSortListMgr();

	bool Init();

	void HandleSortListReq(GameZone* zone, ObjID_t playerId, UInt32 sortType, UInt16 start, UInt16 count);

	/**
	*@brief �������а�
	*/
	void UpdateSortList(ChijiSortListEntry& entry);

	/**
	*@brief ������а�
	*/
	void ClearSortList();

	/**
	*@brief ��������
	*/
	ChijiSortListEntry* FindEntry(ObjID_t id);

	/**
	*@brief ���ø��˻���
	*/
	void SetPlayerScore(UInt64 playerID, UInt32 score);

	/**
	*@brief ���Ҹ��˻���
	*/
	UInt32 GetPlayerScore(UInt64 playerID);

	/**
	*@brief ���ø�����óɼ�
	*/
	void SetPlayerBestRank(UInt64 playerID, UInt32 connId, UInt32 rank);

	/**
	*@brief �������������
	*/
	void ClearPlayerBestRank();

	/**
	*@brief ֪ͨ������óɼ�
	*/
	void NotifyPlayerBestRank(UInt64 playerID);

private:

	void _AddToList(ChijiSortListEntry& entry);

	void _InsertData(ChijiSortListEntry& entry);

	void _UpdateData(ChijiSortListEntry& entry);

	void _deleteData(UInt64 playerId);

private:

	// ��������
	SortEntryList m_SortEntryList;

	// ��һ���
	std::map<UInt64, UInt32> m_PlayerScoreMap;

	// ����������
	std::map<UInt64, ChiJiPlayerBestRank> m_PlayerBestRankMap;
};

#endif
#ifndef __DIG_MAP_H__
#define __DIG_MAP_H__

#include "Dig.h"
#include "AvalonProtocol.h"

class WSPlayer;

class DigMap
{
public:
	DigMap() :m_GoldDigNum(0), m_SilverDigNum(0), m_DigMapDataEntry(NULL){}
	~DigMap(){}

	bool Init(DigMapDataEntry* dataEntry);

public:
	UInt32 OpenMap(ObjID_t playerId, std::vector<DigInfo>& infos, UInt32& playerSize);

	UInt32 CloseMap(ObjID_t playerId);

	UInt32 WatchDig(UInt32 index, DigDetailInfo& info);

	UInt32 OpenDig(WSPlayer* player, UInt32 index);

	void RandomDigNum();

	void Refresh(UInt32 refreshTime, DigType digType);

	void Refresh(UInt32 refreshTime);

	void Supply(DigType digType, UInt32 num, UInt32 supplyTime);

	UInt32 GMOpenDig(WSPlayer* player, UInt32 index, DigRecordInfo& info);

public:
	UInt32 GetDigNum(DigType digType, DigStatus digStatus);

	UInt32 GetDigNumByOpenTime(DigType digType, UInt32 nowTime, UInt32 interval);

	UInt32 GetRefreshDigNum(UInt32 remainDigNum, UInt32 digNum, UInt32 minDigNum, UInt32 remainRefreshDigNum, std::vector<Dig*>& randomDigs, UInt32 refreshTime);

	void GetDigInfos(std::vector<DigInfo>& infos);

	void SyncToPlayers(Avalon::Protocol& sync);

	void SyncRefreshToPlayers();

	UInt32 GetDigMapId() { return m_DigMapDataEntry->id; }
	UInt32 GetGoldDigMaxNum() { return m_DigMapDataEntry->goldDigMaxNum; }
	UInt32 GetGoldDigMinNum() { return m_DigMapDataEntry->goldDigMinNum; }
	UInt32 GetSilverDigMaxNum() { return m_DigMapDataEntry->silverDigMaxNum; }
	UInt32 GetSilverDigMinNum() { return m_DigMapDataEntry->silverDigMinNum; }
	bool IsGoldRefreshHour(UInt32 hour) { return m_DigMapDataEntry->IsGoldRefreshHour(hour); }
	bool IsSilverRefreshHour(UInt32 hour) { return m_DigMapDataEntry->IsSilverRefreshHour(hour); }

private:

	void RefreshDigs(std::vector<Dig*>& randomDigs, UInt32 count, UInt32 refreshTime, DigType digType = DigType::DIG_INVALID);

	void SyncPlayerSize();

private:

	//��ע�˵�ͼ����б�
	std::vector<ObjID_t> m_Players;

	//�˵�ͼ���ڱ���
	std::vector<Dig> m_Digs;

	//���ڱ�������
	UInt32 m_GoldDigNum;

	//���ڱ�������
	UInt32 m_SilverDigNum;

	//������
	DigMapDataEntry* m_DigMapDataEntry;
};

#endif
#ifndef __PK_INFO_MGR_H__
#define __PK_INFO_MGR_H__

#include "PkStatistic.h"

class Player;
class PkStatisticMgr : public CLComplexProperty
{
public:
    PkStatisticMgr();
    ~PkStatisticMgr();

public:
    /**
    *@brief 设置获取拥有者
    */
    void SetOwner(Player* owner) { m_pOwner = owner; }
    Player* GetOwner() const { return m_pOwner; }

    /**
    *@brief 同步到客户端
    */
    void OnOnline();

    /**
    *@brief 新增战绩
    */
    void OnUpdatePkStatistic(PkType type, Occupation opponentOccu, PkRaceResult result);

	/**
	*@brief 清空战绩
	*/
	void OnClearPkStatistic();

    /**
    *@brief 获取战绩信息
    */
    PkStatistic* FindPkStatistic(PkType type);

    /**
    *@brief 加载战绩信息
    */
    bool LoadPkStatistic(PkStatistic* pkStatistic);

private:
    void _RecordPkStatistic(PkStatistic* info, bool isNew, bool isSyncToPlayer);

public:
    /**
    *@brief 场景服务器间同步地下城数据的编码、解码
    */
    void Input(Avalon::NetInputStream &stream);
    void Output(Avalon::NetOutputStream &stream);

private:
    //拥有者
    Player*		                m_pOwner;

    PkStatistic*                m_pkStatistices[PK_NUM];
};

#endif
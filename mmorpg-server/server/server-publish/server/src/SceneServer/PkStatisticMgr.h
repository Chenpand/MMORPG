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
    *@brief ���û�ȡӵ����
    */
    void SetOwner(Player* owner) { m_pOwner = owner; }
    Player* GetOwner() const { return m_pOwner; }

    /**
    *@brief ͬ�����ͻ���
    */
    void OnOnline();

    /**
    *@brief ����ս��
    */
    void OnUpdatePkStatistic(PkType type, Occupation opponentOccu, PkRaceResult result);

	/**
	*@brief ���ս��
	*/
	void OnClearPkStatistic();

    /**
    *@brief ��ȡս����Ϣ
    */
    PkStatistic* FindPkStatistic(PkType type);

    /**
    *@brief ����ս����Ϣ
    */
    bool LoadPkStatistic(PkStatistic* pkStatistic);

private:
    void _RecordPkStatistic(PkStatistic* info, bool isNew, bool isSyncToPlayer);

public:
    /**
    *@brief ������������ͬ�����³����ݵı��롢����
    */
    void Input(Avalon::NetInputStream &stream);
    void Output(Avalon::NetOutputStream &stream);

private:
    //ӵ����
    Player*		                m_pOwner;

    PkStatistic*                m_pkStatistices[PK_NUM];
};

#endif
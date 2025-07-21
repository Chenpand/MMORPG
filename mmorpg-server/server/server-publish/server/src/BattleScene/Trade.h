#ifndef _TRADE_H_
#define _TRADE_H_

#include <CLGameDefine.h>
#include <AvalonObjectPool.h>

class Player;

/**
 *@brief ����������Ϣ
 */
struct TradeInfo
{
	TradeInfo():player(NULL),silver(0)
	{
		status = TRADE_STATUS_UNLOCK;
	}

	//���
	Player* player;
	//�����б�
	std::map<ObjID_t, UInt16> items;
	//����
	UInt32	silver;
	//״̬
	UInt8	status;
};

/**
 *@brief ����
 */
class Trade
{
public:
	Trade(Player* player1, Player* player2);
	~Trade();

public:
	/**
	 *@brief �򿪹رս���
	 */
	void Open();
	void Cancel();

	/**
	 *@brief ���Թرս���
	 */
	void Close();

public:
	/**
	 *@brief ����������
	 */
	void SetSilver(Player* player, UInt32 silver);

	/**
	 *@brief ��ӵ���
	 */
	void AddItem(Player* player, ObjID_t itemid, UInt16 num);
	void RemoveItem(Player* player, ObjID_t itemid);

	/**
	 *@brief ���ý���״̬
	 */
	void SetStatus(Player* player, UInt8 status);

	TradeInfo* GetInfo(Player* player);
	TradeInfo* GetOtherInfo(Player* player);

private:
	//ͬ��״̬
	void SyncStatus();
	//ͬ������
	void SyncSilver();

private:
	//������Ϣ
	TradeInfo	m_TradeInfo[2];
};

#endif

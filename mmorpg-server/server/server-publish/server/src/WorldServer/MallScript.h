#ifndef _WS_MALL_SCRIPT_H_
#define _WS_MALL_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include <CLMallDefine.h>


/**
 *@brief �̳ǽű�
 */
class MallScript : public Avalon::ScriptFile
{
public:
	MallScript(Avalon::ScriptState* state);
	~MallScript();

	void Init();

	/**
	*@brief ���ȫ����Ϣ
	*/
	void ClearMall();

	/**
	*@brief ���һ������������Ϣ
	*/
	void AddNormalLimitItem(UInt32 uIndex, const char *szItem);

	/**
	*@brief ��ӿ�����������
	*/
	void AddGSLimitItem(const char *szItem);

	/**
	*@brief ��ӽ�����������
	*/
	void AddFestivalLimitItem(const char *szActivity, const char *szItem);

	/**
	*@brief ��ӽ�ʯ�̳ǵ���
	*/
	void AddGoldStoneItem(const char *szItem);

	/**
	*@brief �����ʯ�̳ǵ���
	*/
	void AddSilverStoneItem(const char *szItem);

	/**
	*@brief ���õ�ǰ����������
	*/
	void SetCurLimitedItems();
	void SetCurFestivalItems();

	/**
	*@brief ������ҿɹ���Ŀ�������
	*/
	void GetGSItems(UInt32 uBuyRecord, std::list<UInt32> &listBuy);

	/**
	*@brief ��ÿ����������ߵ�����
	*/
	UInt32 GetGSItemIndex(UInt32 uId);

	// ����̳ǵ���
	void AddMallItem(UInt8 type, UInt8 subType, UInt8 job, const char *szItem);

	// ����������
	const char* GetGiftDetail(UInt32 giftId);

	//�ϼ���Ʒ
	void OnSaleMallWaitGoods(UInt32 goodId);

	//�¼���Ʒ
	void OffSaleMallWaitGoods(UInt32 goodId);

private:
	void DecodeItemInfo(MallItemInfo &item, const char *szItem);

	void SetGSItems();
	bool SetCurNormalItems(UInt32 uRecords[4]);

private:
	//typedef std::vector<MallItemInfo>			VecItem;

	//std::map<ItemKey, VecItem>	m_allItems;

	// һ������������Ϣ
	std::vector<MallItemInfo>	m_vecLimitedInfo[4];
	// ��������������Ϣ
	std::vector<MallItemInfo>	m_vecGSItems;
	// ������������
	std::map<std::string, std::vector<MallItemInfo> >	m_mapFestivalItems;
	// ��ʯ�̳�
	std::vector<MallItemInfo>	m_vecGoldStoneItems;
	// ��ʯ�̳�
	std::vector<MallItemInfo>	m_vecSilverStoneItems;
};

#endif

#ifndef AUCTION_ATTENT_RECORD_H_
#define AUCTION_ATTENT_RECORD_H_
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>


class WSPlayer;
namespace zjy_auction {
	struct AuctionInfo;
}


//�����й�ע��
class AuctionAttentRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AuctionAttentRecord, Avalon::NullMutex)
public:
	AuctionAttentRecord();
	~AuctionAttentRecord();
	
	//����ʱ��
	static const UInt32 retent_time = 600;
public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner_id", m_owner_id)
	CL_DBPROPERTY("auction_id", m_auction_id)
	CL_DBPROPERTY("attent_time", m_attent_time)
	CL_DBPROPERTY("delete_time", m_delete_time)
	CL_DBPROPERTY_END()
public:
	void Update();
	void Insert();
	void Delete();
	bool IsShouldDelete(const Avalon::Time &now);
	bool IsValid() { return this->m_delete_time == 0; }
	void OnAttented();
	void OnCanceed();
public:
	//���guid
	ObjUInt64	m_owner_id;
	//��������Ʒid
	ObjUInt64	m_auction_id;
	//��עʱ��
	ObjUInt32	m_attent_time;
	//ɾ��ʱ�� 
	ObjUInt32	m_delete_time;
};

//��ע������
class AuctionAttentMgr : public Avalon::Singleton<AuctionAttentMgr>
{
public:
	//��ע����
	typedef std::map<UInt64, AuctionAttentRecord*> AuctionAttentMap;
	//��ҹ�ע����
	typedef std::map<ObjID_t, std::list<AuctionAttentRecord*>> PlayerAuctionAttents;
	//������Ʒ��ע����
	typedef std::map<UInt64, UInt32> AuctionAttentNum;

public:
	AuctionAttentMgr();
	~AuctionAttentMgr();

	/**
	*@��ʼ��
	*/
	void Init();

	/**
	*@brief �������ݿ��¼
	*/
	void LoadRecords();

	/**
	*@brief ������ؼ�¼
	*/
	bool OnLoadRecordsRet(CLRecordCallback *pCallback);

	/**
	*@brief tick����
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@Bbrief �����ע����
	*/
	UInt32 HandleAttentReq(WSPlayer* player, UInt64 auction_guid);

	/**
	*@brief ��ҹ�ע��������Ʒ
	*/
	UInt32 HandleAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief ���ȡ����ע��������Ʒ
	*/
	UInt32 HandleCancelAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief ������������Ʒ�¼�(������,����,ȡ��)
	*/
	void OnAuctionOffSale(const zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief ��ȡ������Ʒ�Ĺ�ע��Ϣ
	*/
	void GetAuctionAttentInfo(zjy_auction::AuctionInfo& auctionInfo, ObjID_t playerId);

	/**
	*@brief ��ȡ��ҹ�ע����������Ʒ
	*/
	void GetAttentedAuctions(ObjID_t playerId, std::vector<UInt64>& auctionIds);
protected:
	AuctionAttentRecord* GetRecord(UInt64 uid);

	AuctionAttentRecord* GetRecord(ObjID_t playerId, UInt64 auctionId);

	void RemoveAttentRecord(UInt64 auctionId);

	AuctionAttentRecord* GetAttentedRecord(ObjID_t playerId, UInt64 auction_guid);

	void AddOneAttentedRecord(ObjID_t playerId, AuctionAttentRecord* record);

	void RemoveAttentedRecord(ObjID_t playerId, AuctionAttentRecord* record);
	
	void RemoveAttentedRecordsByOnAuctionOffSale(UInt64 auctionId);

	AuctionAttentRecord* AddNewRecord(ObjID_t playerId, UInt64 auctionId);

	bool AddOneRecord(AuctionAttentRecord* record);

	bool CheckAttentIsFull(ObjID_t playerId);

	void IncAuctionAttentNum(UInt64 auctionId);

	void DecAuctionAttentNum(UInt64 auctionId);

	UInt32 GetAuctionAttentNum(UInt64 auctionId);

	void RemoveAuctionAttentNum(UInt64 auctionId);
private:
	AuctionAttentMap		m_records;
	PlayerAuctionAttents	m_player_auct_attents;
	AuctionAttentNum		m_auct_attent_num;

	// ���ɾ����¼��ʱ��
	Avalon::SimpleTimer	    m_checkTimer;
};

#endif	
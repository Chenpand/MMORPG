#ifndef AUCTION_ATTENT_RECORD_H_
#define AUCTION_ATTENT_RECORD_H_
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>


class WSPlayer;
namespace zjy_auction {
	struct AuctionInfo;
}


//拍卖行关注类
class AuctionAttentRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AuctionAttentRecord, Avalon::NullMutex)
public:
	AuctionAttentRecord();
	~AuctionAttentRecord();
	
	//存留时间
	static const UInt32 retent_time = 600;
public:
	//编解码
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
	//玩家guid
	ObjUInt64	m_owner_id;
	//拍卖行商品id
	ObjUInt64	m_auction_id;
	//关注时间
	ObjUInt32	m_attent_time;
	//删除时间 
	ObjUInt32	m_delete_time;
};

//关注管理类
class AuctionAttentMgr : public Avalon::Singleton<AuctionAttentMgr>
{
public:
	//关注集合
	typedef std::map<UInt64, AuctionAttentRecord*> AuctionAttentMap;
	//玩家关注集合
	typedef std::map<ObjID_t, std::list<AuctionAttentRecord*>> PlayerAuctionAttents;
	//拍卖商品关注数量
	typedef std::map<UInt64, UInt32> AuctionAttentNum;

public:
	AuctionAttentMgr();
	~AuctionAttentMgr();

	/**
	*@初始化
	*/
	void Init();

	/**
	*@brief 加载数据库记录
	*/
	void LoadRecords();

	/**
	*@brief 处理加载记录
	*/
	bool OnLoadRecordsRet(CLRecordCallback *pCallback);

	/**
	*@brief tick函数
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@Bbrief 处理关注请求
	*/
	UInt32 HandleAttentReq(WSPlayer* player, UInt64 auction_guid);

	/**
	*@brief 玩家关注拍卖行物品
	*/
	UInt32 HandleAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief 玩家取消关注拍卖行物品
	*/
	UInt32 HandleCancelAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief 处理拍卖行物品下架(被购买,到期,取消)
	*/
	void OnAuctionOffSale(const zjy_auction::AuctionInfo& auctionInfo);

	/**
	*@brief 获取拍卖物品的关注信息
	*/
	void GetAuctionAttentInfo(zjy_auction::AuctionInfo& auctionInfo, ObjID_t playerId);

	/**
	*@brief 获取玩家关注的拍卖行物品
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

	// 检测删除记录定时器
	Avalon::SimpleTimer	    m_checkTimer;
};

#endif	
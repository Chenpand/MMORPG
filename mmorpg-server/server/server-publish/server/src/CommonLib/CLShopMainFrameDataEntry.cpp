#include "CLShopMainFrameDataEntry.h"

bool ShopMainframeDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "ShopMainframeDataEntry::Read(Avalon::DataRow& row)" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();
	std::string str = row.ReadString();

	std::vector<std::string>	childrenShopIdstrs;
	Avalon::StringUtil::Split(str, childrenShopIdstrs, "|");
	for (size_t i = 0; i < childrenShopIdstrs.size(); ++i)
	{
		childrenShopIds.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(childrenShopIdstrs[i]));
	}
	return true;
}



UInt32 ShopMainframeDataEntryMgr::FindKeyByShopId(UInt32 shopId)
{	
	class DataVisitor : public ShopMainframeDataVisitor
	{
	public:
		DataVisitor(UInt32 shopId) : m_shopId(shopId), m_key(0){}
		~DataVisitor() {}

		virtual bool operator()(ShopMainframeDataEntry* dataEntry)
		{
			for (size_t i = 0; i < dataEntry->childrenShopIds.size(); ++i)
			{
				if (dataEntry->childrenShopIds[i] == m_shopId)
				{
					m_key = dataEntry->iD;
					return false;
				}
			}
			return true;
		}

		UInt32 m_shopId;
		UInt32 m_key;
	};

	DataVisitor visitor(shopId);
	Visit(visitor);
	return visitor.m_key;
}
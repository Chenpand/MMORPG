#ifndef _ITEM_PROPERTY_H_
#define _ITEM_PROPERTY_H_

#include <CLItemDefine.h>
#include <CLSkillDefine.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLItemDataEntry.h>
#include <sstream>
#include <CLErrorCode.h>

/**
 *@brief 道具属性
 *@param MaxNum	属性最大个数
 */
template<UInt32 MaxNum>
class ItemRandProperty : public CLComplexProperty
{
public:
	/**
	 *@brief 单个值
	 */
	struct Property
	{
		ItemRandProp	randPropType;		//随机属性类型
		UInt32			value;				//值
	};

public:
	ItemRandProperty();
	~ItemRandProperty();

public:
	/**
	 *@brief 加到数值管理器中
	 */
	//void InflictEffect(PkModValue& values, UInt32 num = MaxNum, UInt32 addFactor = 10000);
	//void InflictPropertyEffect(PkModValue& values, UInt32 attrNo, float rate, UInt32 addFactor = 10000);

	/**
	 *@brief 获取属性数
	 */
	UInt32 GetPropertyNum() const;

	/**
	 *@brief 增加一个属性
	 */
	void AddProperty(ItemRandProp type, UInt32 value);
	void SetProperty(Int32 index, ItemRandProp type, UInt32 value);
	const Property* GetProperty(Int32 index);
	UInt32 GetPropertyNo(Int32 index);

	/**
	 *@brief 删除属性
	 *@param lockFlag 锁元标志位
	 */
	void RemoveProperties(UInt32 lockFlag);

	/**
	 *@brief 计算战斗力
	 */
	//float CalcPower(UInt16 level, UInt32 num = MaxNum, UInt32 factor = 10000); 

public://编解码相关
	void Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//属性列表
	std::vector<Property>	m_Properties;
};

template<UInt32 MaxNum>
ItemRandProperty<MaxNum>::ItemRandProperty()
{
	m_Properties.clear();
}

template<UInt32 MaxNum>
ItemRandProperty<MaxNum>::~ItemRandProperty()
{
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty)
{
	stream & (UInt8)m_Properties.size();
	for(UInt32 i = 0; i < m_Properties.size(); ++i)
	{
		stream & (UInt8)m_Properties[i].randPropType & m_Properties[i].value;
	}
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	UInt8			randPropType = 0;
	UInt32			value = 0;			
	UInt8 num = 0;
	stream & num;
	for (int i = 0; i < num; ++i)
	{
		stream & randPropType;
		stream & value;
		Property prop;
		prop.randPropType = (ItemRandProp)randPropType;
		prop.value = value;
		m_Properties.push_back(prop);
	}
}

// template<UInt32 MaxNum>
// void ItemRandProperty<MaxNum>::InflictEffect(PkModValue& values, UInt32 num, UInt32 addFactor)
// {
// 	for(UInt32 i = 0; i < MaxNum && i < num; ++i)
// 	{
// 		if(m_Properties[i].pkValueID == 0) continue;
// 
// 		double value = m_Properties[i].value;
// 		if(m_Properties[i].modPkValueType != MOD_PKVALUE_RATE)
// 		{
// 			value = m_Properties[i].value * (addFactor / 10000.f);
// 		}
// 		Int32 v = Int32(value > 0 ? value + 0.5 : value - 0.5);
// 
// 		values.AddValue(PkValueID(m_Properties[i].pkValueID), ModPkValueType(m_Properties[i].modPkValueType), v);
// 	}
// }

// template<UInt32 MaxNum>
// void ItemRandProperty<MaxNum>::InflictPropertyEffect(PkModValue& values, UInt32 attrNo, float rate, UInt32 addFactor)
// {
// 	if(attrNo >= MaxNum) return;
// 	if(m_Properties[attrNo].pkValueID == 0) return;
// 	
// 	float value = float(m_Properties[attrNo].value) * rate;
// 	if(m_Properties[attrNo].modPkValueType != MOD_PKVALUE_RATE)
// 	{
// 		value = value * (addFactor / 10000.f);
// 	}
// 	Int32 v = Int32(value > 0 ? value + 0.5 : value - 0.5);
// 
// 	values.AddValue(PkValueID(m_Properties[attrNo].pkValueID),
// 			ModPkValueType(m_Properties[attrNo].modPkValueType), v);
// }

template<UInt32 MaxNum>
UInt32 ItemRandProperty<MaxNum>::GetPropertyNum() const
{
	return m_Properties.size();
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::AddProperty(ItemRandProp type, UInt32 value)
{
	if (m_Properties.size() >= MaxNum)
		return;

	Property prop;
	prop.randPropType = type;
	prop.value = value;
	m_Properties.push_back(prop);
	SetDirty();
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::SetProperty(Int32 index, ItemRandProp type, UInt32 value)
{
	if(index >= int(MaxNum)) return;

	m_Properties[index].randPropType = type;
	m_Properties[index].value = value;
	SetDirty();
}

template<UInt32 MaxNum>
const typename ItemRandProperty<MaxNum>::Property* ItemRandProperty<MaxNum>::GetProperty(Int32 index)
{
	if(index >= int(MaxNum)) return NULL;
	if (index >= (Int32)m_Properties.size())	return NULL;
	
	return &m_Properties[index];
}

template<UInt32 MaxNum>
UInt32 ItemRandProperty<MaxNum>::GetPropertyNo(Int32 index)
{
	return 0;
	//if(index >= int(MaxNum)) return 0;
	//return RandAttrDataEntryMgr::Instance()->GetSerialNoByType(m_Properties[index].pkValueID, m_Properties[index].modPkValueType);
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::RemoveProperties(UInt32 lockFlag)
{
	for(UInt32 i = 0; i < m_Properties.size(); ++i)
	{
		if(m_Properties[i].randPropType != 0 && (lockFlag & (1 << i)) == 0)
		{
			m_Properties.erase(m_Properties->begin() + i);
			SetDirty();
		}
	}
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::EncodeString(std::ostringstream& os)
{
	for(UInt32 i = 0; i < m_Properties.size(); ++i)
	{
		if(m_Properties[i].randPropType != 0)
		{
			os << Int32(m_Properties[i].randPropType) << ';' << Int32(m_Properties[i].value) << ';';
		}
	}
}

template<UInt32 MaxNum>
void ItemRandProperty<MaxNum>::DecodeString(std::istringstream& is)
{
	Int32 randPropType = 0;
	Int32 value = 0;
	char split;

	while((is >> randPropType) && randPropType != 0)
	{
		is >> split >> value >> split;

		if(m_Properties.size() < MaxNum && randPropType < IRP_MAX)
		{
			Property prop;
			prop.randPropType = (ItemRandProp)randPropType;;
			prop.value = value;
			m_Properties.push_back(prop);
		}
	}
}

// template<UInt32 MaxNum>
// float ItemRandProperty<MaxNum>::CalcPower(UInt16 level, UInt32 num, UInt32 factor)
// {
// 	float power = 0;
// 	for(UInt32 i = 0; i < MaxNum && i < num; ++i)
// 	{
// 		if(m_Properties[i].pkValueID == 0) continue;
// 
// 		if(m_Properties[i].modPkValueType == MOD_PKVALUE_BASE)
// 		{
// 			power += std::abs(m_Properties[i].value) * POWER_COEFFICIENT[m_Properties[i].pkValueID] * (factor / 10000.f);
// 		}
// 		else
// 		{
// 			power += CalPercentAttrPower(std::abs(m_Properties[i].value), level);
// 		}
// 	}
// 	return power;
// }


//附魔
template<UInt32 MaxNum>
class ItemMagicProperty : public CLComplexProperty
{
public:
	struct MagicProp
	{
		UInt8  m_type;		//1.随机属性，2.buff
		UInt32 m_param1;	//随机属性: 属性id，buff:buffid
		UInt32 m_param2;	//随机属性: 属性值，buff:无用
	};

	ItemMagicProperty();
	~ItemMagicProperty();

	UInt32 GetPropertyNum() const;
	
	bool AddMagicProp(UInt8 tid, UInt32 p1, UInt32 p2);
	void ClearMagicProp();
	void SetMagciCard(UInt32 id);
	UInt32 GetMagicCard();

	void SetMagicCardLev(UInt8 level);
	UInt8 GetMagicCardLev();

	const MagicProp* GetProperty(Int32 index);

public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	std::vector<MagicProp>	m_Properties;
	UInt32 m_magicCard;
	UInt8  m_level;
};

template<UInt32 MaxNum>
const typename ItemMagicProperty<MaxNum>::MagicProp* ItemMagicProperty<MaxNum>::GetProperty(Int32 index)
{
	if (index >= int(MaxNum)) return NULL;
	if (index >= m_Properties.size()) return NULL;
	
	return &m_Properties[index];
}

template<UInt32 MaxNum>
UInt32 ItemMagicProperty<MaxNum>::GetMagicCard()
{
	return m_magicCard;
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::SetMagciCard(UInt32 id)
{
	m_magicCard = id;
	SetDirty();
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::SetMagicCardLev(UInt8 level)
{
	m_level = level;
	SetDirty();
}

template<UInt32 MaxNum>
UInt8 ItemMagicProperty<MaxNum>::GetMagicCardLev()
{
	return m_level;
}

template<UInt32 MaxNum>
ItemMagicProperty<MaxNum>::~ItemMagicProperty()
{

}

template<UInt32 MaxNum>
ItemMagicProperty<MaxNum>::ItemMagicProperty()
{
	m_magicCard = 0;
	m_level = 0;
}

template<UInt32 MaxNum>
UInt32 ItemMagicProperty<MaxNum>::GetPropertyNum() const
{
	return m_Properties.size();
}

template<UInt32 MaxNum>
bool ItemMagicProperty<MaxNum>::AddMagicProp(UInt8 tid, UInt32 p1, UInt32 p2)
{
	if (m_Properties.size() > MaxNum)
		return false;

	if (tid > 2)
		return false;

	MagicProp prop;
	prop.m_type = tid;
	prop.m_param1 = p1;
	prop.m_param2 = p2;

	m_Properties.push_back(prop);

	SetDirty();
	return true;
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::ClearMagicProp()
{
	m_Properties.clear();
	SetDirty();
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
// 	stream & (UInt8)m_Properties.size();
// 	for (UInt32 i = 0; i < m_Properties.size(); ++i)
// 	{
// 		stream & (UInt8)m_Properties[i].m_type & m_Properties[i].m_param1 & m_Properties[i].m_param2;
// 	}

	stream & m_magicCard & (UInt8)m_level;
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
// 	UInt8 num = 0;
// 	stream & num;
// 	for (int i = 0; i < num; ++i)
// 	{
// 		MagicProp prop;
// 
// 		stream & prop.m_type & prop.m_param1 & prop.m_param2;
// 		m_Properties.push_back(prop);
// 	}

	stream & m_magicCard & m_level;
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::EncodeString(std::ostringstream& os)
{
// 	for (UInt32 i = 0; i < m_Properties.size(); ++i)
// 	{
// 		os << UInt32(m_Properties[i].m_type) << "," << m_Properties[i].m_param1 << "," << m_Properties[i].m_param2;
// 	}
// 	os << 0;

	os << m_magicCard << ',' << UInt32(m_level);
}

template<UInt32 MaxNum>
void ItemMagicProperty<MaxNum>::DecodeString(std::istringstream& is)
{
// 	UInt8 tid = 0;
// 	UInt32 param1 = 0;
// 	UInt32 param2 = 0;
// 	char split;
// 
// 	while ((is >> tid) && tid != 0)
// 	{
// 		is >> split >> param1 >> split >> param2;
// 
// 		if (m_Properties.size() < MaxNum)
// 		{
// 			MagicProp prop;
// 			prop.m_type = tid;
// 			prop.m_param1 = param1;
// 			prop.m_param2 = param2;
// 			m_Properties.push_back(prop);
// 		}
// 	}

	is >> m_magicCard;
	char split;
	if (is >> split)
	{
		UInt32 level;
		is >> level;
		m_level = (UInt8)level;
	}
}

// 宝珠镶嵌孔类型
enum PreciousBeadMountHoleType
{
	PBMHT_NONE = 0,
	PBMHT_COMMON = 1, //普通
	PBMHT_COLOUR = 2, //多彩
	PBMHT_MAX = 3,
};

// 宝珠镶嵌孔
class PreciousBeadMountHole
{
public:
	PreciousBeadMountHole() : index(0), type(0), preciousBeadId(0), buffId(0), auctionCoolTimeStamp(0), extirpeCnt(0), replaceCnt(0), auctionTransTimes(0){};
	~PreciousBeadMountHole() {};

public:
	void SetPreciousBead(UInt32 precBeadId) { this->preciousBeadId = precBeadId; }
	UInt32 GetPreciousBead() { return this->preciousBeadId; }

	void SetBuffId(UInt32 buffId) { this->buffId = buffId; }
	UInt32 GetBuffId() { return this->buffId; }
	
	void SetAuctionCoolTimeStamp(UInt32 timeStamp) { this->auctionCoolTimeStamp = timeStamp; }
	UInt32 GetAuctionCoolTimeStamp() { return this->auctionCoolTimeStamp; }

	void SetExtirpeCnt(UInt32 cnt) { this->extirpeCnt = cnt; }
	UInt32 GetExtirpeCnt() { return extirpeCnt; }

	void SetReplaceCnt(UInt32 cnt) { this->replaceCnt = cnt; }
	UInt32 GetReplaceCnt() { return replaceCnt; }

	void SetAuctionTransTimes(UInt32 auctionTransTimes) { this->auctionTransTimes = auctionTransTimes; }
	UInt32 GetAuctionTransTimes() {	return  this->auctionTransTimes; }
	void Clear()
	{
		preciousBeadId = 0;
		buffId = 0;
		auctionCoolTimeStamp = 0;
		extirpeCnt = 0;
		replaceCnt = 0;
		auctionTransTimes = 0;
	}
public:
	UInt8 index; //孔索引
	UInt8 type; //孔类型
	UInt32 preciousBeadId; //镶嵌宝珠id
	UInt32 buffId;	//附加buff id
	UInt32 auctionCoolTimeStamp; //拍卖行交易冷却时间戳(秒)
	UInt32 extirpeCnt;	// 摘除次数
	UInt32 replaceCnt;	// 置换次数
	UInt32 auctionTransTimes; //拍卖行交易次数
};

#define PRECBEAD_MAXNUM 2

// 宝珠
template<UInt32 MaxNum>
class PreciousBeadHoleProperty : public CLComplexProperty
{
public:
	PreciousBeadHoleProperty();
	~PreciousBeadHoleProperty();

	/**
	*@brief 孔操作
	*/
	void AddHole(UInt32 index, UInt8 type);
	PreciousBeadMountHole* FindHole(UInt8 index);

	/**
	*@brief 检查是否有类型的宝珠
	*/
	bool CheckHavePrecBeadByType(UInt32 type, UInt8 selfIndex);
	/**
	*@brief 设置获取宝珠id
	*/
	UInt32 SetPreciousBeadId(UInt8 holeIndex, UInt32 preciousBeadId);
	UInt32 GetPreciousBeadId(UInt8 holeIndex);
	UInt32 SetBuffId(UInt8 holeIndex, UInt32 buffId);
	UInt32 GetBuffId(UInt8 holeIndex);
	UInt32 SetAuctionCoolTimeStamp(UInt8 holeIndex, UInt32 timestamp);
	UInt32 GetAuctionCoolTimeStamp(UInt8 holeIndex);
	UInt32 SetExtirpeCnt(UInt8 holeIndex, UInt32 cnt);
	UInt32 GetExtirpeCnt(UInt8 holeIndex);
	UInt32 SetReplaceCnt(UInt8 holeIndex, UInt32 cnt);
	UInt32 GetReplaceCnt(UInt8 holeIndex);
	UInt32 SetAuctionTransTimes(UInt8 holeIndex, UInt32 timestamp);
	UInt32 GetAuctionTransTimes(UInt8 holeIndex);

	UInt32 HolesNum() { return m_Holes.size(); }
	UInt32 GetPrecBeadNum();
	void GetRaceInfo(std::vector<RacePrecBead>& raceinfo);

	PreciousBeadMountHole* GetFirstMountedHole();
	void ClearPrecbead();
public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	// 宝珠id（即道具id）
	//UInt32 m_PreciousBeadId;
	//镶嵌孔
	std::vector<PreciousBeadMountHole>	m_Holes;
};

template<UInt32 MaxNum>
PreciousBeadHoleProperty<MaxNum>::PreciousBeadHoleProperty()
{
	//m_PreciousBeadId = 0;
}

template<UInt32 MaxNum>
PreciousBeadHoleProperty<MaxNum>::~PreciousBeadHoleProperty()
{

}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::AddHole(UInt32 index, UInt8 type)
{
	PreciousBeadMountHole hole;
	hole.index = index;
	hole.type = type;
	hole.preciousBeadId = 0;
	m_Holes.push_back(hole);
	SetDirty();
}

template<UInt32 MaxNum>
PreciousBeadMountHole* PreciousBeadHoleProperty<MaxNum>::FindHole(UInt8 index)
{
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].index == index) return &m_Holes[i];
	}
	return NULL;
}

template<UInt32 MaxNum>
bool PreciousBeadHoleProperty<MaxNum>::CheckHavePrecBeadByType(UInt32 type, UInt8 selfIndex)
{
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].index == selfIndex)
		{
			continue;
		}
		if (m_Holes[i].preciousBeadId != 0)
		{
			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(m_Holes[i].preciousBeadId);
			if (dataEntry && dataEntry->thirdType == type)
			{
				return true;
			}
		}
	}
	return false;
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::GetRaceInfo(std::vector<RacePrecBead>& raceinfo)
{
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		RacePrecBead info;
		info.preciousBeadId = m_Holes[i].preciousBeadId;
		info.buffId = m_Holes[i].buffId;
		raceinfo.push_back(info);
	}
}

template<UInt32 MaxNum>
PreciousBeadMountHole* PreciousBeadHoleProperty<MaxNum>::GetFirstMountedHole()
{
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].preciousBeadId > 0)
		{
			return &m_Holes[i];
		}
	}
	return NULL;
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::ClearPrecbead()
{
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].preciousBeadId == 0)
		{
			continue;
		}
		m_Holes[i].Clear();
		SetDirty();
	}
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetPreciousBeadId(UInt8 holeIndex, UInt32 preciousBeadId)
{
	//m_PreciousBeadId = id;

	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetPreciousBead(preciousBeadId);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetBuffId(UInt8 holeIndex, UInt32 buffId)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetBuffId(buffId);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetBuffId(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->GetBuffId();
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetAuctionCoolTimeStamp(UInt8 holeIndex, UInt32 timestamp)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetAuctionCoolTimeStamp(timestamp);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetAuctionCoolTimeStamp(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->GetAuctionCoolTimeStamp();
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetExtirpeCnt(UInt8 holeIndex, UInt32 cnt)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetExtirpeCnt(cnt);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetExtirpeCnt(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->GetExtirpeCnt();
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetReplaceCnt(UInt8 holeIndex, UInt32 cnt)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetReplaceCnt(cnt);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetReplaceCnt(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->GetReplaceCnt();
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetPreciousBeadId(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->preciousBeadId;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetPrecBeadNum()
{
	UInt32 sum = 0;
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].preciousBeadId > 0)
		{
			++sum;
		}
	}
	return sum;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::SetAuctionTransTimes(UInt8 holeIndex, UInt32 auctionTransTimes)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_DATA_INVALID;
	pHole->SetAuctionTransTimes(auctionTransTimes);
	SetDirty();
	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 PreciousBeadHoleProperty<MaxNum>::GetAuctionTransTimes(UInt8 holeIndex)
{
	PreciousBeadMountHole* pHole = this->FindHole(holeIndex);
	if (!pHole) return 0;
	return pHole->GetAuctionTransTimes();
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	//stream & m_PreciousBeadId;
	stream & (UInt8)m_Holes.size();
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		stream & (UInt8)m_Holes[i].index & (UInt8)m_Holes[i].type & m_Holes[i].preciousBeadId 
			& m_Holes[i].buffId & m_Holes[i].auctionCoolTimeStamp & m_Holes[i].extirpeCnt & m_Holes[i].replaceCnt
			& m_Holes[i].auctionTransTimes;
	}
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	//stream & m_PreciousBeadId;
	UInt8	index = 0;
	UInt8	type = 0;
	UInt32  preciousBeadId = 0;
	UInt32  buffId = 0;
	UInt8   num = 0;
	UInt32  auctionCoolTimeStamp = 0;
	UInt32	extirpeCnt = 0;
	UInt32	replaceCnt = 0;
	UInt32 auctionTransTimes = 0;

	stream & num;
	for (int i = 0; i < num; ++i)
	{
		stream & index;
		stream & type;
		stream & preciousBeadId;
		stream & buffId;
		stream & auctionCoolTimeStamp;
		stream & extirpeCnt;
		stream & replaceCnt;
		stream & auctionTransTimes;

		PreciousBeadMountHole hole;
		hole.index = index;
		hole.type = type;
		hole.preciousBeadId = preciousBeadId;
		hole.buffId = buffId;
		hole.auctionCoolTimeStamp = auctionCoolTimeStamp;
		hole.extirpeCnt = extirpeCnt;
		hole.replaceCnt = replaceCnt;
		hole.auctionTransTimes = auctionTransTimes;
		m_Holes.push_back(hole);
	}
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::EncodeString(std::ostringstream& os)
{
	//os << m_PreciousBeadId;
	for (UInt32 i = 0; i < m_Holes.size(); ++i)
	{
		os << UInt32(m_Holes[i].index) << ',' << UInt32(m_Holes[i].type) << ','
			<< m_Holes[i].preciousBeadId << ',' << m_Holes[i].buffId
			<< ',' << m_Holes[i].auctionCoolTimeStamp << ',' << m_Holes[i].extirpeCnt << 
			',' << m_Holes[i].replaceCnt << ',' << m_Holes[i].auctionTransTimes <<  ';';
	}
}

template<UInt32 MaxNum>
void PreciousBeadHoleProperty<MaxNum>::DecodeString(std::istringstream& is)
{
	//is >> m_PreciousBeadId;
	UInt32   index = 0;
	UInt32   type = 0;
	UInt32   preciousBeadId = 0;
	UInt32   buffId = 0;
	UInt32   auctionCoolTimeStamp = 0;
	UInt32	 extirpeCnt = 0;
	UInt32	 replaceCnt = 0;
	UInt32	 auctionTransTimes = 0;

	//char split;
	//while (is >> index)
	//{
	//	if (index == 0)//兼容旧数据
	//	{
	//		PreciousBeadMountHole hole;
	//		hole.index = 1;
	//		hole.type = (UInt8)PBMHT_COMMON;
	//		hole.preciousBeadId = 0;
	//		hole.buffId = 0;
	//		hole.auctionCoolTimeStamp = 0;
	//		hole.auctionTransTimes = 0;
	//		m_Holes.push_back(hole);
	//		SetDirty();
	//		break;
	//	}
	//	if (index > 2)//兼容旧数据
	//	{
	//		PreciousBeadMountHole hole;
	//		hole.index = 1;
	//		hole.type = (UInt8)PBMHT_COMMON;
	//		hole.preciousBeadId = index;
	//		hole.buffId = 0;
	//		hole.auctionCoolTimeStamp = 0;
	//		m_Holes.push_back(hole);
	//		SetDirty();
	//		break;
	//	}
	//	is >> split >> type >> split >> preciousBeadId >> split >> buffId >> 
	//		split >> auctionCoolTimeStamp >> split >> extirpeCnt >> split >> replaceCnt >> split;
	//	if (type != 0)
	//	{
	//		PreciousBeadMountHole hole;
	//		hole.index = (UInt8)index;
	//		hole.type = (UInt8)type;
	//		hole.preciousBeadId = preciousBeadId;
	//		hole.buffId = buffId;
	//		hole.auctionCoolTimeStamp = auctionCoolTimeStamp;
	//		hole.extirpeCnt = extirpeCnt;
	//		hole.replaceCnt = replaceCnt;
	//		m_Holes.push_back(hole);
	//	}
	//	index = 0;
	//	type = 0;
	//	preciousBeadId = 0;
	//	buffId = 0;
	//	auctionCoolTimeStamp = 0;
	//	extirpeCnt = 0;
	//	replaceCnt = 0;
	//}

	std::string str;
	is >> str;
	std::vector<std::string> ss;
	Avalon::StringUtil::Split(str, ss, ";");
	if (ss.size() == 0)
	{
		PreciousBeadMountHole hole;
		hole.index = 1;
		hole.type = (UInt8)PBMHT_COMMON;
		hole.preciousBeadId = 0;
		hole.buffId = 0;
		hole.auctionCoolTimeStamp = 0;
		hole.auctionTransTimes = 0;
		m_Holes.push_back(hole);
		SetDirty();
		return;
	}

	for (auto s : ss)
	{
		if (s == "")
		{
			continue;
		}
		std::vector<std::string> sv;
		Avalon::StringUtil::Split(s, sv, ",");
		if (sv.size() >= 1)
		{
			index = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
		}
		if (sv.size() >= 4)
		{
			type = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
			preciousBeadId = Avalon::StringUtil::ConvertToValue<UInt32>(sv[2]);
			buffId = Avalon::StringUtil::ConvertToValue<UInt32>(sv[3]);
		}
		if (sv.size() >= 5)
		{
			auctionCoolTimeStamp = Avalon::StringUtil::ConvertToValue<UInt32>(sv[4]);
		}
		if (sv.size() >= 7)
		{
			extirpeCnt = Avalon::StringUtil::ConvertToValue<UInt32>(sv[5]);
			replaceCnt = Avalon::StringUtil::ConvertToValue<UInt32>(sv[6]);
		}
		if (sv.size() >= 8)
		{
			auctionTransTimes = Avalon::StringUtil::ConvertToValue<UInt32>(sv[7]);
		}
		if (index == 0)	//兼容旧数据
		{
			m_Holes.clear();
			PreciousBeadMountHole hole;
			hole.index = 1;
			hole.type = (UInt8)PBMHT_COMMON;
			hole.preciousBeadId = 0;
			hole.buffId = 0;
			hole.auctionCoolTimeStamp = 0;
			hole.auctionTransTimes = 0;
			m_Holes.push_back(hole);
			SetDirty();
			break;
		}

		if (index > 2) //兼容旧数据
		{
			m_Holes.clear();
			PreciousBeadMountHole hole;
			hole.index = 1;
			hole.type = (UInt8)PBMHT_COMMON;
			hole.preciousBeadId = index;
			hole.buffId = 0;
			hole.auctionCoolTimeStamp = 0;
			m_Holes.push_back(hole);
			SetDirty();
			break;
		}

		if (type != 0)
		{
			PreciousBeadMountHole hole;
			hole.index = (UInt8)index;
			hole.type = (UInt8)type;
			hole.preciousBeadId = preciousBeadId;
			hole.buffId = buffId;
			hole.auctionCoolTimeStamp = auctionCoolTimeStamp;
			hole.extirpeCnt = extirpeCnt;
			hole.replaceCnt = replaceCnt;
			hole.auctionTransTimes = auctionTransTimes;
			m_Holes.push_back(hole);
		}
		index = 0;
		type = 0;
		preciousBeadId = 0;
		buffId = 0;
		auctionCoolTimeStamp = 0;
		extirpeCnt = 0;
		replaceCnt = 0;
		auctionTransTimes = 0;
	}
}

// 宝珠
template<UInt32 MaxNum>
class PreciousBeadProperty : public CLComplexProperty
{
public:
	PreciousBeadProperty();
	~PreciousBeadProperty();

	/**
	*@brief 设置获取宝珠id
	*/
	void SetPreciousBeadId(UInt32 id);
	UInt32 GetPreciousBeadId() const;

public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	// 宝珠id（即道具id）
	UInt32 m_PreciousBeadId;
};

template<UInt32 MaxNum>
PreciousBeadProperty<MaxNum>::PreciousBeadProperty()
{
	m_PreciousBeadId = 0;
}

template<UInt32 MaxNum>
PreciousBeadProperty<MaxNum>::~PreciousBeadProperty()
{

}

template<UInt32 MaxNum>
UInt32 PreciousBeadProperty<MaxNum>::GetPreciousBeadId() const
{
	return m_PreciousBeadId;
}

template<UInt32 MaxNum>
void PreciousBeadProperty<MaxNum>::SetPreciousBeadId(UInt32 id)
{
	m_PreciousBeadId = id;
	SetDirty();
}

template<UInt32 MaxNum>
void PreciousBeadProperty<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_PreciousBeadId;
}

template<UInt32 MaxNum>
void PreciousBeadProperty<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	stream & m_PreciousBeadId;
}

template<UInt32 MaxNum>
void PreciousBeadProperty<MaxNum>::EncodeString(std::ostringstream& os)
{
	os << m_PreciousBeadId;
}

template<UInt32 MaxNum>
void PreciousBeadProperty<MaxNum>::DecodeString(std::istringstream& is)
{
	is >> m_PreciousBeadId;
}

//单局药水设置
template<UInt32 MaxNum>
class DungeonPotionSet : public CLOwnerComplexProperty
{
public:

	DungeonPotionSet();
	~DungeonPotionSet();

public:
	bool SetPotion(UInt8 pos, UInt32 potionId);
	std::vector<UInt32>& GetPosionPos();

public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	std::vector<UInt32>	m_PotionIds;
};

template<UInt32 MaxNum>
std::vector<UInt32>& DungeonPotionSet<MaxNum>::GetPosionPos()
{
	return m_PotionIds;
}

template<UInt32 MaxNum>
bool DungeonPotionSet<MaxNum>::SetPotion(UInt8 pos, UInt32 potionId)
{
	if (pos < MaxNum && m_PotionIds.size() > pos)
	{
		m_PotionIds[pos] = potionId;
		SetDirty();
		return true;
	}

	return false;
}

template<UInt32 MaxNum>
void DungeonPotionSet<MaxNum>::DecodeString(std::istringstream& is)
{
	UInt32 id = 0;
	char split;
	UInt32 i = 0;

	while ((is >> id) && id != 0)
	{
		is >> split;

		if (i < MaxNum)
		{
			m_PotionIds[i] = id;
		}
		
		i++;
	}
}

template<UInt32 MaxNum>
void DungeonPotionSet<MaxNum>::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_PotionIds.size(); ++i)
	{
		os << UInt32(m_PotionIds[i]) << ";";
	}
	os << 0;
}

template<UInt32 MaxNum>
void DungeonPotionSet<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	UInt8 sz = 0;
	stream & sz;

	if (sz > MaxNum)
	{
		return;
	}

	for (UInt32 i = 0; i < sz; ++i)
	{
		UInt32 id;
		stream & id;
		m_PotionIds[i] = id;
	}
}

template<UInt32 MaxNum>
void DungeonPotionSet<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & (UInt8)m_PotionIds.size();
	for (UInt32 i = 0; i < m_PotionIds.size(); ++i)
	{
		stream & m_PotionIds[i];
	}
}

template<UInt32 MaxNum>
DungeonPotionSet<MaxNum>::~DungeonPotionSet()
{

}

template<UInt32 MaxNum>
DungeonPotionSet<MaxNum>::DungeonPotionSet()
{
	for (UInt32 i = 0; i < MaxNum; ++i)
	{
		m_PotionIds.push_back(0);
	}
	
}

template<UInt32 MaxNum>
class PackageSize : public CLOwnerComplexProperty
{
public:
	PackageSize();
	~PackageSize();

	void SetSize(UInt32 pose, UInt32 val);
	UInt32 GetSize(UInt32 pose);

public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	UInt32 m_PackageSize[MaxNum];
};

template<UInt32 MaxNum>
PackageSize<MaxNum>::PackageSize()
{
	memset(m_PackageSize, 0, sizeof(m_PackageSize));
}

template<UInt32 MaxNum>
PackageSize<MaxNum>::~PackageSize()
{
	memset(m_PackageSize, 0, sizeof(m_PackageSize));
}

template<UInt32 MaxNum>
void PackageSize<MaxNum>::SetSize(UInt32 pose, UInt32 val)
{
	if (pose >= MaxNum)
	{
		return;
	}

	m_PackageSize[pose] = val;
	SetDirty();
}

template<UInt32 MaxNum>
UInt32 PackageSize<MaxNum>::GetSize(UInt32 pose)
{
	if (pose >= MaxNum)
	{
		return UINT32_MAX;
	}

	return m_PackageSize[pose];
}

template<UInt32 MaxNum>
void PackageSize<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & (Int8)MaxNum;
	stream.SeriaArray(m_PackageSize, MaxNum);
}

template<UInt32 MaxNum>
void PackageSize<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	Int8 num = 0;
	stream & num;

	if (num > (Int8)MaxNum)
	{
		return;
	}

	stream.SeriaArray(m_PackageSize, num);
}

template<UInt32 MaxNum>
void PackageSize<MaxNum>::EncodeString(std::ostringstream& os)
{
}

template<UInt32 MaxNum>
void PackageSize<MaxNum>::DecodeString(std::istringstream& is)
{
}

// 铭文镶嵌孔
class InscriptionMountHole
{
public:
	InscriptionMountHole() : index(0), type(0), inscriptionId(0) {};
	~InscriptionMountHole() {};

public:
	UInt32 index; //孔索引
	UInt32 type; //孔类型
	UInt32 inscriptionId; //铭文id
};

// 铭文镶嵌孔集合
template<UInt32 MaxNum>
class InscriptionHoleProperty : public CLComplexProperty
{
public:
	InscriptionHoleProperty();
	~InscriptionHoleProperty();

	/**
	*@brief 孔操作
	*/
	bool AddHole(UInt32 index, UInt32 type);
	InscriptionMountHole* FindHole(UInt32 index);

	/**
	*@brief 设置获取铭文id
	*/
	UInt32 SetInscriptionId(UInt32 index, UInt32 inscriptionId);
	UInt32 GetInscriptionId(UInt32 index);

	UInt32 HolesNum() { return m_Holes.size(); }

	void GetRaceInfo(std::vector<UInt32>& Ids);

	void Clear();
public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	//镶嵌孔
	std::vector<InscriptionMountHole>	m_Holes;
};

template<UInt32 MaxNum>
InscriptionHoleProperty<MaxNum>::InscriptionHoleProperty()
{

}

template<UInt32 MaxNum>
InscriptionHoleProperty<MaxNum>::~InscriptionHoleProperty()
{

}

template<UInt32 MaxNum>
bool InscriptionHoleProperty<MaxNum>::AddHole(UInt32 index, UInt32 type)
{
	if (FindHole(index) || m_Holes.size() >= MaxNum)
	{
		return false;
	}

	InscriptionMountHole hole;
	hole.index = index;
	hole.type = type;
	hole.inscriptionId = 0;

	m_Holes.push_back(hole);
	SetDirty();

	return true;
}

template<UInt32 MaxNum>
InscriptionMountHole* InscriptionHoleProperty<MaxNum>::FindHole(UInt32 index)
{
	for (size_t i=0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].index == index)
		{
			return &m_Holes[i];
		}
	}

	return NULL;
}

template<UInt32 MaxNum>
UInt32 InscriptionHoleProperty<MaxNum>::SetInscriptionId(UInt32 index, UInt32 inscriptionId)
{
	InscriptionMountHole* pHole = this->FindHole(index);
	if (!pHole)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	pHole->inscriptionId = inscriptionId;
	SetDirty();

	return ErrorCode::SUCCESS;
}

template<UInt32 MaxNum>
UInt32 InscriptionHoleProperty<MaxNum>::GetInscriptionId(UInt32 index)
{
	InscriptionMountHole* pHole = this->FindHole(index);
	if (pHole)
	{
		return pHole->inscriptionId;
	}

	return 0;
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::GetRaceInfo(std::vector<UInt32>& Ids)
{
	for (size_t i=0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].inscriptionId != 0)
		{
			Ids.push_back(m_Holes[i].inscriptionId);
		}
	}
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & (UInt16)m_Holes.size();
	for (size_t i=0; i < m_Holes.size(); ++i)
	{
		stream & m_Holes[i].index & m_Holes[i].type & m_Holes[i].inscriptionId;
	}
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	UInt16 num = 0;
	stream & num;

	if (num > MaxNum)
	{
		num = MaxNum;
	}

	for (UInt16 i=0; i < num; ++i)
	{
		UInt32 index = 0;
		UInt32 type = 0;
		UInt32 inscriptionId = 0;

		stream & index;
		stream & type;
		stream & inscriptionId;

		InscriptionMountHole hole;
		hole.index = index;
		hole.type = type;
		hole.inscriptionId = inscriptionId;

		m_Holes.push_back(hole);
	}
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::EncodeString(std::ostringstream& os)
{
	for (size_t i=0; i < m_Holes.size(); ++i)
	{
		os	<< m_Holes[i].index << ',' 
			<< m_Holes[i].type << ','
			<< m_Holes[i].inscriptionId << ';';
	}
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::DecodeString(std::istringstream& is)
{
	std::vector<std::string> tmpVar;
	Avalon::StringUtil::Split(is.str(), tmpVar, ";");
	for (const auto& elem : tmpVar)
	{
		if (m_Holes.size() >= MaxNum)
		{
			break;
		}

		std::vector<std::string> values;
		Avalon::StringUtil::Split(elem, values, ",");
		if (values.size() < 3) continue;

		InscriptionMountHole hole;
		hole.index = Avalon::StringUtil::ConvertToValue<UInt32>(values[0]);
		hole.type = Avalon::StringUtil::ConvertToValue<UInt32>(values[1]);
		hole.inscriptionId = Avalon::StringUtil::ConvertToValue<UInt32>(values[2]);

		m_Holes.push_back(hole);
	}
}

template<UInt32 MaxNum>
void InscriptionHoleProperty<MaxNum>::Clear()
{
	if (HolesNum() == 0)
	{
		return;
	}
	for (size_t i = 0; i < m_Holes.size(); ++i)
	{
		if (m_Holes[i].inscriptionId == 0)
		{
			continue;
		}
		m_Holes[i].inscriptionId = 0;
		SetDirty();
	}
	
}

template<UInt32 MaxNum>
class ItemKeyValues : public CLComplexProperty
{
public:
	ItemKeyValues();
	~ItemKeyValues();

public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

public:
	bool	SetValue(UInt32 key, Int32 value);
	Int32	GetValue(UInt32 key);

private:
	std::map<UInt32, Int32> m_KeyValues;
};

template<UInt32 MaxNum>
ItemKeyValues<MaxNum>::ItemKeyValues()
{}

template<UInt32 MaxNum>
ItemKeyValues<MaxNum>::~ItemKeyValues()
{}

template<UInt32 MaxNum>
void ItemKeyValues<MaxNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & (UInt16)m_KeyValues.size();
	for (auto it : m_KeyValues)
	{
		stream & it.first & it.second;
	}
}

template<UInt32 MaxNum>
void ItemKeyValues<MaxNum>::Decode(Avalon::NetInputStream& stream)
{
	UInt16 num = 0;
	stream & num;

	for (UInt16 i = 0; i < num; ++i)
	{
		UInt32 key = 0;
		UInt32 value = 0;

		stream & key;
		stream & value;

		m_KeyValues[key] = value;
	}
}

template<UInt32 MaxNum>
void ItemKeyValues<MaxNum>::EncodeString(std::ostringstream& os)
{
	for (auto it : m_KeyValues)
	{
		os << it.first << ','
			<< it.second << ','
			<< ";";
	}
}

template<UInt32 MaxNum>
void ItemKeyValues<MaxNum>::DecodeString(std::istringstream& is)
{
	std::vector<std::string> tmpVar;
	Avalon::StringUtil::Split(is.str(), tmpVar, ";");
	for (const auto& elem : tmpVar)
	{
		std::vector<std::string> values;
		Avalon::StringUtil::Split(elem, values, ",");
		if (values.size() < 2) continue;
		UInt32 key = Avalon::StringUtil::ConvertToValue<UInt32>(values[0]);
		Int32 value = Avalon::StringUtil::ConvertToValue<Int32>(values[1]);

		m_KeyValues[key] = value;
	}
}

template<UInt32 MaxNum>
bool	ItemKeyValues<MaxNum>::SetValue(UInt32 key, Int32 value)
{
	if (key >= IKVKT_MAX || key == IKVKT_NONE)
	{
		return false;
	}

	auto it = m_KeyValues.find(key);
	if (it != m_KeyValues.end()
		&& it->second == value)
	{
		return true;
	}

	m_KeyValues[key] = value;
	SetDirty();
	return true;
}

template<UInt32 MaxNum>
Int32	ItemKeyValues<MaxNum>::GetValue(UInt32 key)
{
	if (key >= IKVKT_MAX || key == IKVKT_NONE)
	{
		return 0;
	}

	auto it = m_KeyValues.find(key);
	if (it == m_KeyValues.end())
	{
		return 0;
	}

	return it->second;
}
#endif

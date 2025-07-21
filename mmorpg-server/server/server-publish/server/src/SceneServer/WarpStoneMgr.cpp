#include "WarpStoneMgr.h"
#include "Player.h"
#include "SSServiceSwitchMgr.h"

WarpStoneMgr::WarpStoneMgr()
{
	m_pOwner = NULL;
}

WarpStoneMgr::~WarpStoneMgr()
{
}

void WarpStoneMgr::SetOwner(Player* owner)
{
	m_pOwner = owner;
}

WarpStone* WarpStoneMgr::FindWarpStone(UInt32 id)
{
	class FindWarpStoneVisitor : public WarpStoneVisitor
	{
	public:
		FindWarpStoneVisitor(UInt32 id) :m_Id(id), m_WarpStone(NULL) {}

		bool operator()(WarpStone* ws)
		{
			if (ws->GetId() == m_Id)
			{
				m_WarpStone = ws;
				return false;
			}
			return true;
		}

		WarpStone* GetWarpStone() { return m_WarpStone; }

	private:
		UInt32 m_Id;
		WarpStone* m_WarpStone;
	};

	FindWarpStoneVisitor visitor(id);
	VisitWarpStone(visitor);
	return visitor.GetWarpStone();
}

UInt32 WarpStoneMgr::GetWarpStoneSize(WarpStoneType type, UInt32 level)
{
	class GetWarpStoneSizeVisitor : public WarpStoneVisitor
	{
	public:
		GetWarpStoneSizeVisitor(WarpStoneType type, UInt32 level) : m_WarpStoneType(type), m_Level(level), m_Size(0) {}

		bool operator()(WarpStone* ws)
		{
			if ((!IsWarpStone(m_WarpStoneType) || ws->GetType() == m_WarpStoneType) &&
				(m_Level == 0 || ws->GetLevel() >= m_Level))
			{
				m_Size += 1;
			}
			return true;
		}

		UInt32 GetSize() { return m_Size; }

	private:
		WarpStoneType m_WarpStoneType;
		UInt32 m_Level;
		UInt32 m_Size;
	};
	GetWarpStoneSizeVisitor visitor(type, level);
	VisitWarpStone(visitor);
	return visitor.GetSize();
}

bool MinLevelSortFunction(UInt32 i, UInt32 j) { return (i > j); }
UInt32 WarpStoneMgr::GetWarpStoneMinLevel(UInt32 level, UInt32 size, WarpStoneType type)
{
	if (size < 0) return 0;

	class GetWarpStoneLevelSortVisitor : public WarpStoneVisitor
	{
	public:
		GetWarpStoneLevelSortVisitor(WarpStoneType type) :m_WarpStoneType(type){}

		bool operator() (WarpStone* ws)
		{
			if ((!IsWarpStone(m_WarpStoneType) || ws->GetType() == m_WarpStoneType))
			{
				m_WarpStoneLevelList.push_back(ws->GetLevel());
			}
			return true;
		}

		std::vector<UInt32> GetWarpStoneLevelList() { return m_WarpStoneLevelList; }

	private:
		WarpStoneType m_WarpStoneType;
		std::vector<UInt32> m_WarpStoneLevelList;
	};

	GetWarpStoneLevelSortVisitor visitor(type);
	VisitWarpStone(visitor);
	std::vector<UInt32> warpStoneLevelList = visitor.GetWarpStoneLevelList();
	if (warpStoneLevelList.size() < size) return 0;

	std::sort(warpStoneLevelList.begin(), warpStoneLevelList.end(), MinLevelSortFunction);
	return warpStoneLevelList[size - 1];
}

void WarpStoneMgr::WarpStoneUpLevel(UInt32 id, UInt32 level)
{
	WarpStone* ws = FindWarpStone(id);
	if (ws == NULL) return;
	ws->SetLevel(level);
	SetDirty();
}

void WarpStoneMgr::VisitWarpStone(WarpStoneVisitor& visitor)
{
	WarpStoneVec::iterator iter = m_WarpStones.begin();
	while(iter != m_WarpStones.end())
	{
		WarpStone* ws = &(*iter);
		++iter;
		if (!visitor(ws)) return;
	}
}

UInt32 WarpStoneMgr::Unlock(UInt32 id)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_WARP_STONE))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	if (FindWarpStone(id) != NULL) return ErrorCode::WARP_STONE_UNLOCK_ERROR;
	WarpStone ws(GetOwner(), id);
	if (!ws.Init()) return ErrorCode::WARP_STONE_NOT_FOUNT;
	UInt32 nRes = ws.Unlock();
	if (nRes == ErrorCode::SUCCESS)
	{
		m_WarpStones.push_back(ws);
		SetDirty();
	}
	return nRes;
}

UInt32 WarpStoneMgr::Charge(UInt32 id, std::vector<ChargeInfo> chargeInfo)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_WARP_STONE))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	WarpStone* ws = FindWarpStone(id);
	if (ws == NULL) return ErrorCode::WARP_STONE_NOT_FOUNT;

	std::vector<EnergyStoneInfo> energyStone;
	for (UInt32 i = 0; i < chargeInfo.size(); ++i)
	{
		EnergyStoneInfo info(chargeInfo[i].itemId, chargeInfo[i].num);
		energyStone.push_back(info);
	}

	UInt32 nRes = ws->Charge(energyStone);
	if (nRes == ErrorCode::SUCCESS)
	{
		SetDirty();
	}

	return nRes;
}

void WarpStoneMgr::OnLevelup()
{
	//每次升级的时候检查是否解锁
	if (m_WarpStones.size() == 0)
	{
		WarpStoneDataEntryMgr::WarpStoneDataVec wss = WarpStoneDataEntryMgr::Instance()->GetLvlWarpStones((UInt8)GetOwner()->GetLevel());
		WarpStoneDataEntryMgr::WarpStoneDataVec::iterator iter = wss.begin();
		while (iter != wss.end())
		{
			if ((*iter)->isUnlock)
			{
				Unlock((*iter)->id);
			}
			++iter;
		}
	}
}

void WarpStoneMgr::Input(Avalon::NetInputStream& stream)
{
	WarpStone ws;
	stream & ws;
	while (ws.GetId() != 0)
	{
		if (ws.Init()) m_WarpStones.push_back(ws);
		stream & ws;
	}
}

void WarpStoneMgr::Output(Avalon::NetOutputStream& stream)
{
	class OutputVisitor : public WarpStoneVisitor
	{
	public:
		OutputVisitor(Avalon::NetOutputStream& stream) :m_Stream(stream) {}

		bool operator()(WarpStone* ws)
		{
			if (ws->IsDirty())
			{
				m_Stream & *ws;
				ws->ClearDirty();
			}
			
			return true;
		}

	private:
		Avalon::NetOutputStream& m_Stream;
	};

	OutputVisitor visitor(stream);
	VisitWarpStone(visitor);
	stream & UInt32(0);
}

void WarpStoneMgr::Decode(Avalon::NetInputStream& stream)
{
	UInt32 id = 0;
	UInt32 level = 0;
	UInt32 energy = 0;

	stream & id;
	while (id != 0)
	{
		stream & level & energy;
		WarpStone ws(GetOwner(), id, level, energy);
		if (ws.Init()) m_WarpStones.push_back(ws);
		stream & id;
	}
}

void WarpStoneMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	class EncodeVisitor : public WarpStoneVisitor
	{
	public:
		EncodeVisitor(Avalon::NetOutputStream& _stream, bool _bdirty)
			:m_Stream(_stream), m_bDirty(_bdirty) {}

		bool operator()(WarpStone* ws)
		{
			if ((m_bDirty && ws->IsDirty()) || (!m_bDirty))
			{
				UInt8 level = (UInt8)ws->GetLevel();
				m_Stream & ws->GetId() & level & ws->GetEnergy();
			}
			return true;
		}

	private:
		Avalon::NetOutputStream& m_Stream;
		bool	m_bDirty;
	};
	EncodeVisitor visitor(stream, bDirty);
	VisitWarpStone(visitor);
	stream & UInt32(0);
}

void WarpStoneMgr::DecodeString(std::istringstream& is)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_WARP_STONE))
	{
		// 功能未开放
		return;
	}

	UInt32 id = 0;
	char split = 0;
	UInt32 level = 0;
	UInt32 energy = 0;
	while ((is >> id) && id != 0)
	{
		is >> split >> level >> split >> energy >> split;
		WarpStone ws(GetOwner(), id, level, energy);
		if (ws.Init()) m_WarpStones.push_back(ws);
	}
}

void WarpStoneMgr::EncodeString(std::ostringstream& os)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_WARP_STONE))
	{
		// 功能未开放
		return;
	}

	class EncodeStringVisitor :public WarpStoneVisitor
	{
	public:
		EncodeStringVisitor(std::ostringstream& _os) :ostr(_os) {}

		bool operator()(WarpStone* ws)
		{
			ostr << ws->GetId() << "," << ws->GetLevel() << "," << ws->GetEnergy() << ";";
			return true;
		}

	private:
		std::ostringstream& ostr;
	};
	EncodeStringVisitor visitor(os);
	VisitWarpStone(visitor);
}


void WarpStoneMgr::ClearInvalidData()
{
	WarpStoneVec::iterator iter = m_WarpStones.begin();
	while (iter != m_WarpStones.end())
	{
		iter->ClearDirty();
		++iter;
	}
}

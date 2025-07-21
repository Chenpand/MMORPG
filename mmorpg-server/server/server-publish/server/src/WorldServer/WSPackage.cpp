#include "WSPackage.h"

WSPackage::WSPackage(UInt8 type)
{
	m_Type = type;
	m_Grids = new WSItem*[GetMaxSize()];
	memset(m_Grids,0,GetMaxSize() * sizeof(WSItem*));
	m_ItemNum = 0;
	m_Size = TribePackSize[m_Type];
}

WSPackage::~WSPackage()
{
	delete[] m_Grids;
}

bool WSPackage::SetSize(UInt8 size)
{
	if(size <= TribePackSize[m_Type])
	{
		m_Size = size;
		return true;
	}
	return false;
}

UInt8 WSPackage::GetMaxSize() const
{
	return TribePackSize[m_Type];
}

UInt8 WSPackage::GetSize() const
{
	return m_Size;
}

UInt8 WSPackage::GetCapacity() const
{
	return GetSize() - m_ItemNum;
}

WSItem* WSPackage::FindItemByPos(UInt8 grid) const
{
	if(grid >= GetSize()) return NULL;

	return m_Grids[grid];
}

bool WSPackage::AddItem(WSItem* item)
{
	if(item == NULL) return false;

	for(UInt8 index = 0; index < GetSize(); ++index)
	{
		if(m_Grids[index] == NULL)
		{
			m_Grids[index] = item;
			item->SetPos(ItemPos(m_Type,0));
			++m_ItemNum;

			return true;
		}
	}
	return false;
}

void WSPackage::RemoveItem(WSItem* item)
{
	if(item == NULL) return;

	ItemPos pos = item->GetPos();
	if(pos.pack != GetType()) return;

	for(UInt8 index = 0; index < GetSize(); ++index)
	{
		if(m_Grids[index] == item)
		{
			m_Grids[index] = NULL;
			--m_ItemNum;
			return;
		}
	}
}

void WSPackage::Clear()
{
	memset(m_Grids,0,GetMaxSize() * sizeof(WSItem*));
	m_ItemNum = 0;
}

void WSPackage::VisitItems(WSItemVisitor& visitor)
{
	for(UInt8 index = 0; index < GetSize(); ++index)
	{
		if(m_Grids[index] != NULL)
		{
			if(!visitor(m_Grids[index])) return;
		}
	}
}



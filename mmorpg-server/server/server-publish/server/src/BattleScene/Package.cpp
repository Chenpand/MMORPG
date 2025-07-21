#include "Package.h"

#include "Player.h"

Package::Package(UInt8 type)
{
	m_Type = type;
	m_Grids = new Item*[GetMaxSize()];
	memset(m_Grids, 0, GetMaxSize() * sizeof(Item*));
	m_ItemNum = 0;
	m_Size = PackSize[m_Type];
}

Package::~Package()
{
	delete[] m_Grids;
}

bool Package::SetSize(UInt32 size)
{
	if(size <= PackSize[m_Type])
	{
		m_Size = size;
		return true;
	}
	return false;
}

UInt32 Package::GetMaxSize() const
{
	return PackSize[m_Type];
}

UInt32 Package::GetSize() const
{
	return m_Size;
}

UInt32 Package::GetCapacity() const
{
	return GetSize() - GetItemNum();
}

Item* Package::FindItemByPos(UInt32 grid) const
{
	if(grid >= GetSize()) return NULL;

	return m_Grids[grid];
}

bool Package::AddItem(Item* item, UInt32 grid)
{
	if(item == NULL) return false;
	if(grid < GetSize())
	{
		if(m_Grids[grid] != NULL) return false;
		
		m_Grids[grid] = item;
		item->SetPos(ItemPos(m_Type,grid));
		++m_ItemNum;
		return true;
	}
	else
	{
		for (UInt32 index = 0; index < GetSize(); ++index)
		{
			if(m_Grids[index] == NULL)
			{
				m_Grids[index] = item;
				item->SetPos(ItemPos(m_Type,index));
				++m_ItemNum;

				return true;
			}
		}
	}
	return false;
}


bool Package::RemoveItem(Item* item)
{
	if(item == NULL) return false;

	ItemPos pos = item->GetPos();
	if(pos.pack != GetType() || pos.gridIndex >= GetSize()) return false;
	if(m_Grids[pos.gridIndex] != item) return false;

	m_Grids[pos.gridIndex] = NULL;
	--m_ItemNum;

	return true;
}

void Package::Clear()
{
	memset(m_Grids,0,GetMaxSize() * sizeof(Item*));
	m_ItemNum = 0;
}

void Package::RemoveOverSizeItem()
{
	Int32 overSize = (Int32)GetItemNum() - (Int32)GetSize();
	if (overSize <= 0)
	{
		return;
	}

	ErrorLogStream << "Package(" << GetType() << ")'s items were over flow(size=" << GetItemNum() 
		<< "), actual size(" << GetSize() << ")." << LogStream::eos;

	for (UInt32 index = GetItemNum() - 1; index != GetSize() - 1; --index)
	{
		Item* item = m_Grids[index];
		if (item != NULL)
		{
			RemoveItem(item);
		}
	}
}

void Package::VisitItems(ItemVisitor& visitor)
{
	for (UInt32 index = 0; index < GetSize(); ++index)
	{
		if(m_Grids[index] != NULL)
		{
			if(!visitor(m_Grids[index])) return;
		}
	}
}



bool Storage::AddItem(Item* item, UInt32 grid)
{
	if(item == NULL) return false;

	for (UInt32 index = 0; index < GetSize(); ++index)
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

bool Storage::RemoveItem(Item* item)
{
	if(item == NULL) return false;

	ItemPos pos = item->GetPos();
	if(pos.pack != GetType()) return false;

	for (UInt32 index = 0; index < GetSize(); ++index)
	{
		if(m_Grids[index] == item)
		{
			m_Grids[index] = NULL;
			--m_ItemNum;
			return true;
		}
	}
	return false;
}


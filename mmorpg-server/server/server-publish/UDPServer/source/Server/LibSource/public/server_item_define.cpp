#include <stdlib.h>

#include "public/server_item_define.h"


namespace KingNet { namespace Server {

int goods_entry_compare(const void* v1, const void* v2)
{
	stGoodsEntry* p1 = (stGoodsEntry*)v1;
	stGoodsEntry* p2 = (stGoodsEntry*)v2;

	return int(p1->m_iID - p2->m_iID);

}

int item_entry_compare(const void* v1, const void* v2)
{
	stItemEntry* p1 = (stItemEntry*)v1;
	stItemEntry* p2 = (stItemEntry*)v2;

	return int(p1->m_iID - p2->m_iID);
}

int charmitem_entry_compare(const void* v1, const void* v2)
{
	stCharmItemEntry* p1 = (stCharmItemEntry*)v1;
	stCharmItemEntry* p2 = (stCharmItemEntry*)v2;

	return int(p1->m_iID - p2->m_iID);
}

int chest_entry_compare(const void* v1, const void* v2)
{
	stChestEntry* p1 = (stChestEntry*)v1;
	stChestEntry* p2 = (stChestEntry*)v2;

	return int(p1->m_iID - p2->m_iID);
}

int32_t sort_goods_entries( stGoodsEntry* entries, int32_t count )
{
	if (NULL == entries || count <= 0)
	{
		return fail;
	}

	qsort(entries, size_t(count), sizeof(stGoodsEntry), goods_entry_compare);
	return success;
}

int32_t sort_item_entries( stItemEntry* entries, int32_t count )
{
	if (NULL == entries || count <= 0)
	{
		return fail;
	}

	qsort(entries, size_t(count), sizeof(stItemEntry), item_entry_compare);
	return success;
}

int32_t sort_charmitem_entries( stCharmItemEntry* entries, int32_t count )
{
	if (NULL == entries || count <= 0)
	{
		return fail;
	}

	qsort(entries, size_t(count), sizeof(stCharmItemEntry), charmitem_entry_compare);
	return success;
}

int32_t sort_chest_entries(stChestEntry* entries, int32_t count)
{
	if (NULL == entries || count <= 0)
	{
		return fail;
	}

	qsort(entries, size_t(count), sizeof(stChestEntry), chest_entry_compare);
	return success;
}

stGoodsEntry* search_goods_entry_by_id( int32_t id, stGoodsEntry* entries, int32_t count )
{
	stGoodsEntry key;
	key.m_iID = id;

	void* result = NULL;

	result = bsearch((const void*)&key, entries, size_t(count), sizeof(stGoodsEntry), goods_entry_compare);
	
	return (stGoodsEntry*)result;
}

stItemEntry* search_item_entry_by_id( int32_t id, stItemEntry* entries, int32_t count )
{
	stItemEntry key;
	key.m_iID = id;

	void* result = NULL;

	result = bsearch((const void*)&key, entries, size_t(count), sizeof(stItemEntry), item_entry_compare);

	return (stItemEntry*)result;
}

stCharmItemEntry* search_charmitem_entry_by_id( int32_t id, stCharmItemEntry* entries, int32_t count )
{
	stCharmItemEntry key;
	key.m_iID = id;

	void* result = NULL;

	result = bsearch((const void*)&key, entries, size_t(count), sizeof(stCharmItemEntry), charmitem_entry_compare);

	return (stCharmItemEntry*)result;
}

stChestEntry* search_chest_entry_by_id( int32_t id, stChestEntry* entries, int32_t count )
{
	stChestEntry key;
	key.m_iID = id;

	void* result = NULL;

	result = bsearch((const void*)&key, entries, size_t(count), sizeof(stChestEntry), chest_entry_compare);

	return (stChestEntry*)result;
}


}}//namespace KingNet { namespace Server {

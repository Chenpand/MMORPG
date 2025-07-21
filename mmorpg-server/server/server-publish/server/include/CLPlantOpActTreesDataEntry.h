#ifndef _CL_PLANT_OPACT_TREES_DATAENTRY_H_
#define _CL_PLANT_OPACT_TREES_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct TreeItem 
{
	TreeItem()
		:itemId(0), profic(0), weight(0)
	{
	}
	UInt32  itemId;	//��ľid
	UInt32	profic;	//������
	UInt32	weight;	//Ȩ��
};

/**
*@brief ��ֲ����������
*/
struct PlantOpActTreesDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// Ψһid
	UInt32	id;
	// ����������
	std::vector<UInt32> proficSect;
	// ��ľ��
	std::vector<TreeItem> trees;
};

typedef Avalon::DataEntryVisitor<PlantOpActTreesDataEntry> PlantOpActTreesDataEntryVisitor;

class PlantOpActTreesDataEntryMgr : public Avalon::DataEntryMgrBase<PlantOpActTreesDataEntry>
	, public Avalon::Singleton<PlantOpActTreesDataEntryMgr>
{
public:
	TreeItem*  RandomOneTreeItemByProf(UInt32	prof);
};


#endif

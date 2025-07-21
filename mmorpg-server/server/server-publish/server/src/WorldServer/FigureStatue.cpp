#include "FigureStatue.h"
#include "CLRecordClient.h"


FigureStatue::FigureStatue()
{

}

FigureStatue::~FigureStatue()
{

}

FigureStatue* FigureStatue::Create()
{
	FigureStatue* ptr = new FigureStatue;

	return ptr;
}

void FigureStatue::Destroy(FigureStatue*& ptr)
{
	CL_SAFE_DELETE(ptr);
}

void FigureStatue::UpdateToDB(bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_figure_statue", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void FigureStatue::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_figure_statue", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}
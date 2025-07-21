#include "GASSelectInitDataCallBack.h"
#include "GASApplication.h"

SelectInitDataCallback::SelectInitDataCallback()
{
	GASApplication::Instance()->IncInitDataLoadCount();
}

SelectInitDataCallback::~SelectInitDataCallback()
{
}

void SelectInitDataCallback::OnFinished()
{
	if (!OnLoadFinished())
	{
		return;
		OnFailed(0);
	}
	GASApplication::Instance()->DecInitDataLoadCount();
}

void SelectInitDataCallback::OnFailed(UInt32 errorcode)
{
	ErrorLogStream << "load init data failed!" << LogStream::eos;
	GASApplication::Instance()->Exit();
}

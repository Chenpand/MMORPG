#include "WSSelectInitDataCallback.h"
#include "WSApplication.h"

SelectInitDataCallback::SelectInitDataCallback()
{
	WSApplication::Instance()->IncInitDataLoadCount();
}

SelectInitDataCallback::~SelectInitDataCallback()
{
}

void SelectInitDataCallback::OnFinished()
{
	if(!OnLoadFinished())
	{
		return;
		OnFailed(0);
	}
	WSApplication::Instance()->DecInitDataLoadCount();
}

void SelectInitDataCallback::OnFailed(UInt32 errorcode)
{
	ErrorLogStream << "load init data failed!" << LogStream::eos;
	WSApplication::Instance()->Exit();
}

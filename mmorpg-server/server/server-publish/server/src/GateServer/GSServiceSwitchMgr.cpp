#include "GSServiceSwitchMgr.h"


void GSServiceSwitchMgr::OnOpenService(ServiceType type)
{
	InfoLogStream << "service(" << type << ") opened..." << LogStream::eos;

}

void GSServiceSwitchMgr::OnCloseService(ServiceType type)
{
	InfoLogStream << "service(" << type << ") closed..." << LogStream::eos;

}
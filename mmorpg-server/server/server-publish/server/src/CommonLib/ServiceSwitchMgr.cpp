#include "ServiceSwitchMgr.h"

ServiceSwitchMgr::ServiceSwitchMgr()
{
	for (int i = 0; i < SERVICE_NUM; i++)
	{
		m_serviceConfigs[i].type = (ServiceType)i;
	}
}

ServiceSwitchMgr::~ServiceSwitchMgr()
{

}

void ServiceSwitchMgr::OpenService(ServiceType type)
{
	if (type >= SERVICE_NUM)
	{
		return;
	}

	if (IsOpen(type))
	{
		return;
	}

	m_serviceConfigs[type].open = true;
	OnOpenService(type);
}

void ServiceSwitchMgr::CloseService(ServiceType type)
{
	if (type >= SERVICE_NUM)
	{
		return;
	}

	if (!IsOpen(type))
	{
		return;
	}

	m_serviceConfigs[type].open = false;
	OnCloseService(type);
}

bool ServiceSwitchMgr::IsOpen(ServiceType type)
{
	if (type >= SERVICE_NUM)
	{
		return false;
	}

	return m_serviceConfigs[type].open;
}
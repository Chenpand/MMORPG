#include "CLNetAddress.h"
#include <AvalonXmlDocument.h>
#include <AvalonStringUtil.h>

CLNetAddress::CLNetAddress()
{
}

CLNetAddress::~CLNetAddress()
{
}

bool CLNetAddress::Load(const std::string &config)
{
	Avalon::XmlDocument doc(config);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* element = doc.RootElement();
	if(element == NULL) return false;

	element = doc.FirstChildElement(element,"server");
	while(element != NULL)
	{
		UInt32 type = 0;
		if(!doc.GetAttribute(element,"type",type)
			|| type == 0)
		{
			continue;
		}
		UInt32 id = 0;
		doc.GetAttribute(element,"id",id);

		UInt32 nodeid = IsPlatformServer(type)? CL_MAKE_NODEID(0, type, id) : CL_MAKE_NODEID(ZONE_ID,type,id);//noted by aprilliu, 平台服务器不属于某个具体的区

		std::string str;
		if(doc.GetAttribute(element,"internaladdress",str))
		{
			Avalon::SockAddr addr;
			if(addr.FromString(str))
			{
				AddInternalAddress(nodeid,addr);
			}
		}

		if(doc.GetAttribute(element,"externaladdress",str))
		{
			Avalon::SockAddr addr;
			if(addr.FromString(str))
			{
				AddExternalAddress(nodeid,addr);
			}
		}

		if(doc.GetAttribute(element,"consoleaddress",str))
		{
			Avalon::SockAddr addr;
			if(addr.FromString(str))
			{
				AddConsoleAddress(nodeid, addr);
			}
		}

		if(doc.GetAttribute(element,"billingaddress",str))
		{
			Avalon::SockAddr addr;
			if(addr.FromString(str))
			{
				AddBillingAddress(nodeid, addr);
			}
		}

		if (doc.GetAttribute(element, "diraddress", str))
		{
			Avalon::SockAddr addr;
			if (!addr.FromString(str))
			{
				return false;
			}

			AddDirAddress(addr);
		}

		element = doc.NextElement(element,"server");
	}

	return true;
}

bool CLNetAddress::FindInternalAddress(UInt32 nodeid, Avalon::SockAddr &addr)
{
	SockAddrMap::iterator iter = m_InternalAddrs.find(nodeid);
	if(iter != m_InternalAddrs.end())
	{
		addr = iter->second;
		return true;
	}
	return false;
}

bool CLNetAddress::FindExternalAddress(UInt32 nodeid, Avalon::SockAddr &addr)
{
	SockAddrMap::iterator iter = m_ExternalAddrs.find(nodeid);
	if(iter != m_ExternalAddrs.end())
	{
		addr = iter->second;
		return true;
	}
	return false;
}

bool CLNetAddress::FindConsoleAddress(UInt32 nodeid, Avalon::SockAddr& addr)
{
	SockAddrMap::iterator iter = m_ConsoleAddrs.find(nodeid);
	if(iter != m_ConsoleAddrs.end())
	{
		addr = iter->second;
		return true;
	}
	return false;
}

bool CLNetAddress::FindBillingAddress(UInt32 nodeid, Avalon::SockAddr& addr)
{
	SockAddrMap::iterator iter = m_BillingAddrs.find(nodeid);
	if(iter != m_BillingAddrs.end())
	{
		addr = iter->second;
		return true;
	}
	return false;
}

bool CLNetAddress::AddInternalAddress(UInt32 nodeid, const Avalon::SockAddr &addr)
{
	return m_InternalAddrs.insert(std::make_pair(nodeid,addr)).second;
}

bool CLNetAddress::AddExternalAddress(UInt32 nodeid, const Avalon::SockAddr &addr)
{
	return m_ExternalAddrs.insert(std::make_pair(nodeid,addr)).second;
}

bool CLNetAddress::AddConsoleAddress(UInt32 nodeid, const Avalon::SockAddr& addr)
{
	return m_ConsoleAddrs.insert(std::make_pair(nodeid, addr)).second;
}

bool CLNetAddress::AddBillingAddress(UInt32 nodeid, const Avalon::SockAddr& addr)
{
	return m_BillingAddrs.insert(std::make_pair(nodeid, addr)).second;
}

void CLNetAddress::AddDirAddress(const Avalon::SockAddr& addr)
{
	m_DirServerAddr.push_back(addr);
}

bool CLNetAddress::GetInternalAddresses(UInt32 type, std::map<UInt32,Avalon::SockAddr> &addrs)
{
	bool ret = false;
	for(SockAddrMap::iterator iter = m_InternalAddrs.begin();iter != m_InternalAddrs.end();++iter)
	{
		if(CL_GET_SERVERTYPE_FROM_NODEID(iter->first) == type)
		{
			addrs.insert(std::make_pair(iter->first,iter->second));
			ret = true;
		}
	}

	return ret;
}


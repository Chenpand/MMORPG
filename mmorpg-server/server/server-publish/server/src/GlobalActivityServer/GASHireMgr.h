#ifndef _GAS_HIRE_MGR_H_
#define _GAS_HIRE_MGR_H_

#include <set>
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLRecordCallback.h>

struct BindedKey
{
	UInt32 accid;
	std::string platform;


	bool operator< (const BindedKey &rsh) const
	{
		if (accid < rsh.accid)
		{
			return true;
		}
		else if (accid == rsh.accid)
		{
			if (platform < rsh.platform)
			{
				return true;
			}
		}
		return false;
	}
};

class GASHireMgr : public Avalon::Singleton<GASHireMgr>
{
public:
	bool OnInit();
	bool IsAlreadyBind(UInt32 accid, std::string platform);
	void AddToBindedSet(UInt32 accid, std::string platform);

	bool LoadHireAccidCallBack(CLRecordCallback* callback);
private:
	std::set<BindedKey> m_BindedAccidSet;
};

#endif

#ifndef _CL_DATALOG_MGR_H_
#define _CL_DATALOG_MGR_H_

#include <string>
#include <AvalonSingleton.h>
#include <AvalonSocket.h>

/**
 *@brief ͳ����־������
 */
class CLDataLogMgr : public Avalon::Singleton<CLDataLogMgr>
{
public:
	CLDataLogMgr();
	~CLDataLogMgr();

public:
	bool Init(const std::string& config);
	void Final();

private:
	bool InitLogger(const std::string& logName, const Avalon::SockAddr& addr);
	bool InitLogger(const std::string& logName, const std::string& logDir);
};

#endif

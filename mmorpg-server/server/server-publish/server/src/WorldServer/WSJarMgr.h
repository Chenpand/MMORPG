#ifndef _WS_JAR_MGR_H_
#define _WS_JAR_MGR_H_

#include <CLItemDefine.h>

// 开罐记录
class JarRewardRecordsMgr : public Avalon::Singleton<JarRewardRecordsMgr>
{
public:
	JarRewardRecordsMgr();
	~JarRewardRecordsMgr();

	void Init();
	void QueryRecords(UInt32 id, std::vector<OpenJarRecord>& records);
	void QueryRecords(std::vector<UInt32>& jarIds, std::vector<OpenJarRecord>& records);
	void QueryEqrecRecords(std::vector<OpenJarRecord>& records);
	void AddRecord(UInt32 id, std::string name, UInt32 itemId, UInt32 num, UInt32 color);
	void OnLoadRecord(OpenJarRecord& record);

private:
	void _LoadDB();
	void _SyncDB(OpenJarRecord& record);

	//key 罐子id
	std::map<UInt32, std::vector<OpenJarRecord>>	m_records;
	//存放装备回收类型罐子 按时间先后顺序
	std::vector<OpenJarRecord> m_orderEqrecJars;
};

#endif


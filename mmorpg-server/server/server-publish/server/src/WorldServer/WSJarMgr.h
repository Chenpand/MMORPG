#ifndef _WS_JAR_MGR_H_
#define _WS_JAR_MGR_H_

#include <CLItemDefine.h>

// ���޼�¼
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

	//key ����id
	std::map<UInt32, std::vector<OpenJarRecord>>	m_records;
	//���װ���������͹��� ��ʱ���Ⱥ�˳��
	std::vector<OpenJarRecord> m_orderEqrecJars;
};

#endif


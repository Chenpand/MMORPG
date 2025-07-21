#ifndef _RS_TABLE_H_
#define _RS_TABLE_H_

#include <map>
#include <sstream>
#include <AvalonRetryDBConnection.h>
#include <AvalonTime.h>
#include <AvalonNetConnection.h>
#include "RSRecord.h"
#include "DBThread.h"

#define RECORD_ID "guid"

/**
 *@brief ����Ϣ
 */
class RSColumn
{
public:
	RSColumn():index(0),type(DBT_INVALID){}

	std::string	name;	//����
	int			index;	//������
	int			type;	//������
};

/**
 *@brief ���ݿ����
 */
class RSTable
{
public:
	typedef HashMap<std::string,RSColumn>	ColumnMap;
	typedef HashMap<DBKey_t,RSRecord>	RecordMap;

public:
	explicit RSTable();
	~RSTable();

public:
	/**
	 *@brief ��ʼ��
	 */
	bool Init(const std::string& name, const std::string& uniqueNameCol);

	/**
	 *@brief ��ջ���
	 */
	void Final();

	/**
	*@brief ��ȡ��Ϣ��Sequence
	*/
	UInt32 GenSequence();

public:
	/**
	 *@brief ��ȡ����
	 */
	const std::string& GetName() const{ return m_Name; }

	/**
	 *@brief ����һ��
	 */
	const RSColumn* FindColumn(const std::string& colname) const;
	const RSColumn* FindColumn(int index) const;

	/**
	 *@brief ��ȡΨһ����
	 */
	const RSColumn* GetUniqueNameColumn() const;

public:
	/**
	 *@brief ����һ�м�¼
	 */
	bool UpdateRecord(DBKey_t key, Avalon::NetInputStream& stream, bool bFlush);
	
	/**
	 *@brief ���������¼�¼
	 */
	bool UpdateRecord(Avalon::NetInputStream& stream);

	/**
	*@brief ����һ�м�¼
	*/
	bool NewUpdateRecord(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream, bool bFlush);

	/**
	*@brief ���������¼�¼
	*/
	bool NewUpdateRecord(UInt32 requestid, UInt32 connid, Avalon::NetInputStream& stream);

	/**
	 *@brief ����һ�м�¼
	 */
	UInt32 InsertRecord(DBKey_t key, Avalon::NetInputStream& stream);
	UInt32 NewInsertRecord(UInt32 requestid, UInt32 connId, DBKey_t key, Avalon::NetInputStream& stream);
	static bool SaveGuidSeed(DBKey_t key);
	UInt32 InsertAutoGuidRecord(Avalon::NetInputStream& stream, UInt32 requestid, UInt32 connId);

	/**
	 *@brief ɾ��һ�м�¼
	 */
	bool DeleteRecord(DBKey_t key);

	/**
	 *@brief ������ɾ����¼
	 */
	bool DeleteRecord(const std::string& cond);

	/**
	 *@brief ��ѯ��¼�����ͽ��
	 */
	void SelectRecord(UInt32 srcZoneId, UInt32 srcNodeId, UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn);

	/**
	*@brief ��ѯ��¼�����ͽ��
	*/
	void SelectRecordNew(UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn);

	/**
	 *@brief ����
	 */
	UInt32 ChangeName(DBKey_t key, const std::string& name, UInt8 isInsert);

	/**
	 *@brief tick
	 */
	bool OnTick(const Avalon::Time& now, UInt32 cachetime, bool bLog);

	/**
	 *@brief ��д��������
	 */
	void Flush();

private:
	/**
	 *@brief ֱ�Ӳ���һ�е����ݿ�
	 */
	bool UpdateRecordDirect(DBKey_t key, Avalon::NetInputStream& stream);

	/**
	*@brief ֱ�Ӳ���һ�е����ݿ�
	*/
	bool NewUpdateRecordDirect(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream);

	/**
	 *@brief ����ת��Ϊsql
	 */
	void ConvertToSql(Avalon::RetryDBConnection* conn, std::ostringstream& stream, const CLDatabaseData& data);

	/**
	 *@brief ��sql����л�ȡ������
	 */
	DBDataType GetColumnType(const std::string& typestr);

	/**
	 *@brief flush record
	 */
	bool FlushRecord(DBKey_t key, RSRecord& record);

	/**
	 *@brief batch flush record
	 */
	bool BatchFlushRecord(const std::vector<std::pair<DBKey_t, RSRecord*>>& records);

	/**
	 *@brief ����SQL
	 */
	bool BatchFlushSql(const std::ostringstream& sql, const std::ostringstream& whereSql, const std::map<std::string, std::string>& columnSqls);

	/**
	*@brief ������Ϣ
	*/
	void SendPacket(UInt32 requestid, Avalon::NetConnectionPtr conn, Avalon::Packet* packet, UInt32 srcZoneId, UInt32 srcNodeId);

private:
	//����
	std::string	m_Name;

	//���б�
	ColumnMap	m_Columns;
	//Ψһ��������
	int			m_NameIndex;
	//������,��indexΪ����
	std::vector<RSColumn>	m_ColumnVec;
	//���¼
	RecordMap	m_Records;
	//�������
	UInt32		m_InsertNum;
	//������ʱ��
	UInt64		m_InsertTime;
	//���´���
	UInt32		m_UpdateNum;
	//������ʱ��
	UInt64		m_UpdateTime;
	//ɾ������
	UInt32		m_DeleteNum;
	//ɾ����ʱ��
	UInt64		m_DeleteTime;
	//��ѯ����
	UInt32		m_SelectNum;
	//��ѯ��ʱ��
	UInt64		m_SelectTime;
	//���ݿ������߳�
	DBThread*	m_DBThread;
};

#endif

#ifndef _RS_TABLEMGR_H_
#define _RS_TABLEMGR_H_

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonXmlDocument.h>
#include <AvalonSimpleTimer.h>
#include "RSTable.h"

/**
 *@brief ���ݻ���
 */
class RSTableMgr : public Avalon::Singleton<RSTableMgr>
{
	typedef std::map<std::string,RSTable*>		TableMap;
	typedef std::map<std::string,std::string>	UniqueNameMap;


	/**
	 *@brief guid����
	 */
	struct GuidSeed
	{
		GuidSeed():nodetype(0), nodeid(0), seed(0){}

		UInt32 nodetype;	//�ڵ�����
		UInt32 nodeid;		//�ڵ�id
		UInt64 seed;		//�ڵ�����
	};
	typedef std::vector<GuidSeed>	GuidSeedVec;
 
public:
	RSTableMgr();
	~RSTableMgr();

	/**
	 *@brief ��ʼ��
	 */
	bool Init(Avalon::PropertySet* properties);

	/**
	 *@brief ��ֹ
	 */
	void Final();

public:
	/**
	 *@brief ��ȡ�������ݿ�����
	 */
	Avalon::RetryDBConnection* GetLocalDBConn() const{ return m_pLocalConn; }

	/**
	 *@brief ��ȡΨһ����֤����
	 */
	Avalon::RetryDBConnection* GetUniqueNameDBConn() const { return m_pUniqueNameConn; }
	/**
	 *@brief ���һ����
	 */
	bool AddTable(RSTable* table);

	/**
	 *@brief ����һ����
	 */
	RSTable* FindTable(const std::string& name);

	/**
	 *@brief ���û�ȡguid����
	 */
	bool SetGuidSeed(UInt32 nodetype,UInt32 nodeid,UInt64 seed);
	UInt64 GetGuidSeed(UInt32 nodetype,UInt32 nodeid) const;

	/**
	 *@brief ��д��������
	 */
	void Flush();

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief �������ô���һ�����ݿ�����
	 */
	Avalon::RetryDBConnection* CreateDBConn(Avalon::XmlDocument& doc, Avalon::XmlElement* node);

	/**
	 *@brief ��ȡ���еı��
	 */
	bool ShowTables(Avalon::RetryDBConnection* dbconn);

public:
	//�������ݿ�����
	Avalon::RetryDBConnection *m_pLocalConn;
	//��֤Ψһ�Ե�����
	Avalon::RetryDBConnection *m_pUniqueNameConn;

	//��д��鶨ʱ��
	Avalon::SimpleTimer	m_CheckFlushTimer;
	//��־��ʱ��
	Avalon::SimpleTimer m_LogTimer;

	//��
	TableMap	m_Tables;
	//����ʱ��
	UInt32		m_CacheTime;
	//Ψһ��map
	UniqueNameMap	m_UniqueNames;

	//guid����
	GuidSeedVec	m_GuidSeeds;
};

#endif

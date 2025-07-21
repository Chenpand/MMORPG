#ifndef _CL_RECORD_CALLBACK_H_
#define _CL_RECORD_CALLBACK_H_

#include "CLDatabaseDefine.h"

/**
 *@brief record����ص�
 */
class CLRecordCallback
{
	typedef std::map<std::string, size_t> ColumnMap;
	typedef std::vector<UInt8>	TypeVec;
	typedef std::vector<std::pair<DBKey_t, CLDatabaseData* > >	RowVec;

public:
	CLRecordCallback();
	virtual ~CLRecordCallback();

public:
	/**
	 *@brief ���
	 */
	virtual void OnFinished(){}

	/**
	 *@brief ʧ��
	 */
	virtual void OnFailed(UInt32 errorcode){}

	/**
	 *@brief �ƶ�����һ��
	 *@return true �ɹ� falseû����һ��
	 */
	bool NextRow();

	/**
	 *@brief ��ȡ��ǰ������
	 */
	DBKey_t GetKey() const;

	/**
	 *@brief ��ȡ��ǰ��һ������
	 */
	const CLDatabaseData& GetData(const std::string& name) const;

public:
	/**
	 *@brief ִ��
	 */
	void Execute();

	/**
	 *@brief ���ý��
	 */
	void SetResult(UInt32 result){ m_Result = result; }

	/**
	 *@brief ��������ʱ��
	 */
	void SetRequestTime(UInt32 time){ m_RequestTime = time; }
	UInt32 GetRequestTime() const{ return m_RequestTime; }

	/**
	 *@brief ��������Ϣ
	 */
	bool DecodeColumns(Avalon::NetInputStream& stream);

	/**
	 *@brief ����һ������
	 */
	bool DecodeRow(Avalon::NetInputStream& stream);

private:
	//������
	ColumnMap	m_Columns;
	//������
	TypeVec		m_Types;
	//������
	RowVec		m_Rows;
	//��ǰ������
	int		m_Index;

	//����ʱ��
	UInt32	m_RequestTime;
	//���
	UInt32	m_Result;
};

#endif

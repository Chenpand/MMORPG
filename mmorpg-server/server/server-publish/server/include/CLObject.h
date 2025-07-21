#ifndef _CL_OBJECT_H_
#define _CL_OBJECT_H_

#include "CLDefine.h"
#include <string>

/**
 *@brief object����
 */
class CLObject
{
public:
	CLObject(){ m_ID = 0; }
	virtual ~CLObject(){}

public:
	/**
	 *@brief ���û�ȡid
	 */
	void SetID(ObjID_t id) { m_ID = id; }
	ObjID_t GetID() const { return m_ID; }

	/**
	 *@brief ���û�ȡ����
	 */
	virtual void SetName(const std::string& name){ m_Name = name; }
	virtual const std::string& GetName() const { return m_Name; }

private:
	//id
	ObjID_t		m_ID;
	//��ȡ����
	std::string m_Name;
};

/**
 *@brief ������ģ��
 */
template<typename T>
class CLVisitor
{
public:
	virtual ~CLVisitor(){}

	virtual bool operator()(T* object) = 0;
};

/**
 *@brief ɾ��������
 */
template<typename T>
class CLDeleteVisitor : public CLVisitor<T>
{
public:
	bool operator()(T* object)
	{
		delete object;
		return true;
	}
};

#endif

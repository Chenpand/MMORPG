#ifndef _WS_PACKAGE_H_
#define _WS_PACKAGE_H_

#include "WSItem.h"

/**
 *@brief �������
 */
class WSPackage
{
public:
	WSPackage(UInt8 type);
	virtual ~WSPackage();

public:
	/**
	 *@brief ��ȡ��С����
	 */
	bool SetSize(UInt8 size);
	UInt8 GetSize() const;

	/**
	 *@brief ��ȡ����С
	 */
	UInt8 GetMaxSize() const;

	/**
	 *@brief ��ȡʣ��ռ�
	 */
	UInt8 GetCapacity() const;

	/**
	 *@brief ��ȡ��������
	 */
	UInt8 GetType() const { return m_Type; }

	/**
	 *@brief ���ݸ��������ҵ���
	 */
	WSItem* FindItemByPos(UInt8 grid) const;
	
	/**
	 *@brief ���һ�����ߣ����ϲ�
	 */
	virtual bool AddItem(WSItem* item);

	/**
	 *@brief �Ƴ�����
	 */
	virtual void RemoveItem(WSItem* item);

	/**
	 *@brief �������
	 */
	void Clear();

public:
	/**
	 *@brief �������е���
	 */
	void VisitItems(WSItemVisitor& visitor);

protected:
	//��������
	UInt8	m_Type;
	//��������
	WSItem**	m_Grids;
	//������
	UInt8	m_ItemNum;
	//��ǰ��С
	UInt8	m_Size;
};

#endif


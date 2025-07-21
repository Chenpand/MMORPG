#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "Item.h"
#include <set>

class Player;

/**
 *@brief ����
 */
class Package
{
public:
	Package(UInt8 type);
	virtual ~Package();

public:
	/**
	 *@brief ��ȡ��С����
	 */
	bool SetSize(UInt32 size);
	UInt32 GetSize() const;

	/**
	 *@brief ��ȡ����С
	 */
	UInt32 GetMaxSize() const;

	/**
	 *@brief ��ȡʣ��ռ�
	 */
	UInt32 GetCapacity() const;

	/**
	 *@brief ��ȡ������
	 */
	virtual UInt32 GetItemNum() const{ return m_ItemNum; }

	/**
	 *@brief ��ȡ��������
	 */
	UInt8 GetType() const { return m_Type; }

	/**
	 *@brief ���ݸ��������ҵ���
	 */
	virtual Item* FindItemByPos(UInt32 grid) const;
	
	/**
	 *@brief ���һ�����ߣ����ϲ�
	 */
	virtual bool AddItem(Item* item, UInt32 grid = INVALID_ITEMGRID);

	/**
	 *@brief �Ƴ�����
	 */
	virtual bool RemoveItem(Item* item);

	/**
	 *@brief �������
	 */
	virtual void Clear();

	/**
	 *@brief ����ܷ��ƶ�λ��
	 */
	virtual bool CheckItemMove(Item* item, UInt32 destGrid){ return true; }
	/**
	 *@brief ����ܷ���ӵ�ָ��λ��
	 */
	virtual bool CheckAddItem(Item* item, UInt32 destGrid){ return true; }

	/**
	*@brief �Ƴ����������ĵ���
	*/
	void RemoveOverSizeItem();

public://�¼�
	/**
	 *@brief ���߷����ı䣬���ӡ�ɾ����������λ�õı仯
	 */
	virtual void OnItemChanged(Player* owner){}

public:
	/**
	 *@brief �������е���
	 */
	virtual void VisitItems(ItemVisitor& visitor);

protected:
	//��������
	UInt8	m_Type;
	//��������
	Item**	m_Grids;
	//������
	UInt32	m_ItemNum;
	//��ǰ��С
	UInt32	m_Size;
};


/**
 *@brief �ֿ�
 */
class Storage : public Package
{
public:
	Storage(UInt8 type):Package(type){}
	~Storage(){}

public:
	bool AddItem(Item* item, UInt32 grid = INVALID_ITEMGRID);
	bool RemoveItem(Item* item);

private:

};

#endif

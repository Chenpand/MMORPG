#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "Item.h"
#include <set>

class Player;

/**
 *@brief 包裹
 */
class Package
{
public:
	Package(UInt8 type);
	virtual ~Package();

public:
	/**
	 *@brief 获取大小容量
	 */
	bool SetSize(UInt32 size);
	UInt32 GetSize() const;

	/**
	 *@brief 获取最大大小
	 */
	UInt32 GetMaxSize() const;

	/**
	 *@brief 获取剩余空间
	 */
	UInt32 GetCapacity() const;

	/**
	 *@brief 获取道具数
	 */
	virtual UInt32 GetItemNum() const{ return m_ItemNum; }

	/**
	 *@brief 获取包裹类型
	 */
	UInt8 GetType() const { return m_Type; }

	/**
	 *@brief 根据格子索引找道具
	 */
	virtual Item* FindItemByPos(UInt32 grid) const;
	
	/**
	 *@brief 添加一个道具，不合并
	 */
	virtual bool AddItem(Item* item, UInt32 grid = INVALID_ITEMGRID);

	/**
	 *@brief 移除道具
	 */
	virtual bool RemoveItem(Item* item);

	/**
	 *@brief 清除道具
	 */
	virtual void Clear();

	/**
	 *@brief 检查能否移动位置
	 */
	virtual bool CheckItemMove(Item* item, UInt32 destGrid){ return true; }
	/**
	 *@brief 检查能否添加到指定位置
	 */
	virtual bool CheckAddItem(Item* item, UInt32 destGrid){ return true; }

	/**
	*@brief 移除超出容量的道具
	*/
	void RemoveOverSizeItem();

public://事件
	/**
	 *@brief 道具发生改变，增加、删除、数量或位置的变化
	 */
	virtual void OnItemChanged(Player* owner){}

public:
	/**
	 *@brief 访问所有道具
	 */
	virtual void VisitItems(ItemVisitor& visitor);

protected:
	//包裹类型
	UInt8	m_Type;
	//包裹格子
	Item**	m_Grids;
	//道具数
	UInt32	m_ItemNum;
	//当前大小
	UInt32	m_Size;
};


/**
 *@brief 仓库
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

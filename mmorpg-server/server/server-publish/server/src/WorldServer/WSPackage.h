#ifndef _WS_PACKAGE_H_
#define _WS_PACKAGE_H_

#include "WSItem.h"

/**
 *@brief 部落包裹
 */
class WSPackage
{
public:
	WSPackage(UInt8 type);
	virtual ~WSPackage();

public:
	/**
	 *@brief 获取大小容量
	 */
	bool SetSize(UInt8 size);
	UInt8 GetSize() const;

	/**
	 *@brief 获取最大大小
	 */
	UInt8 GetMaxSize() const;

	/**
	 *@brief 获取剩余空间
	 */
	UInt8 GetCapacity() const;

	/**
	 *@brief 获取包裹类型
	 */
	UInt8 GetType() const { return m_Type; }

	/**
	 *@brief 根据格子索引找道具
	 */
	WSItem* FindItemByPos(UInt8 grid) const;
	
	/**
	 *@brief 添加一个道具，不合并
	 */
	virtual bool AddItem(WSItem* item);

	/**
	 *@brief 移除道具
	 */
	virtual void RemoveItem(WSItem* item);

	/**
	 *@brief 清除道具
	 */
	void Clear();

public:
	/**
	 *@brief 访问所有道具
	 */
	void VisitItems(WSItemVisitor& visitor);

protected:
	//包裹类型
	UInt8	m_Type;
	//包裹格子
	WSItem**	m_Grids;
	//道具数
	UInt8	m_ItemNum;
	//当前大小
	UInt8	m_Size;
};

#endif


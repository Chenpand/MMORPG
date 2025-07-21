#ifndef _ITEMCD_H_
#define _ITEMCD_H_

#include <map>
#include <CLObjectProperty.h>

class Player;

struct CDTime
{
	CDTime(){}
	CDTime(UInt64 _endTime, UInt32 _maxTime) :
	endTime(_endTime), maxTime(_maxTime){}

	UInt64 endTime;		//结束时间戳
	UInt32 maxTime;		//最大时间
};

/**
 *@brief 道具cd
 */
class ItemCD : public CLComplexProperty
{
	typedef std::map<UInt8, CDTime>	CDMap;

public:
	ItemCD();
	~ItemCD();

	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

public:
	/**
	 *@brief 获取设置使用时间
	 */
	void SetUseTime(UInt8 id, UInt64 usetime, UInt32 maxTime);
	UInt64 GetUseTime(UInt8 id) const;

public://编解码
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	//拥有者
	Player*	m_pOwner;
	//cd列表
	CDMap	m_CDs;
};

#endif

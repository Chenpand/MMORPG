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

	UInt64 endTime;		//����ʱ���
	UInt32 maxTime;		//���ʱ��
};

/**
 *@brief ����cd
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
	 *@brief ��ȡ����ʹ��ʱ��
	 */
	void SetUseTime(UInt8 id, UInt64 usetime, UInt32 maxTime);
	UInt64 GetUseTime(UInt8 id) const;

public://�����
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	//ӵ����
	Player*	m_pOwner;
	//cd�б�
	CDMap	m_CDs;
};

#endif

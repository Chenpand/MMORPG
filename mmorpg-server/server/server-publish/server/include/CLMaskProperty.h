#ifndef _CL_MASK_PROPERTY_H_
#define _CL_MASK_PROPERTY_H_

#include <AvalonBase64.h>
#include "CLObjectProperty.h"
#include "CLOwnerObjectProperty.h"

/**
 *@brief 掩码属性
 */
template<size_t BitNum>
class CLMaskProperty : public CLOwnerComplexProperty
{
	//字节数
	const static size_t CHAR_NUM = ((BitNum - 1) / 8 + 1);

public:
	CLMaskProperty(){ memset(m_Flags, 0, sizeof(m_Flags)); }
	~CLMaskProperty(){}

public:
	/**
	 *@brief 设置、取消、测试掩码
	 */
	void SetMask(UInt32 offset);
	void UnsetMask(UInt32 offset);
	bool CheckMask(UInt32 offset) const;

	void ClearMask();

public://编解码

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(m_Flags, CHAR_NUM);
	}
		
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//字节数组
	char	m_Flags[CHAR_NUM];
};

template<size_t BitNum>
void CLMaskProperty<BitNum>::SetMask(UInt32 offset)
{
	if(offset >= BitNum || CheckMask(offset)) return;

	int index = offset / 8;
	int bitoffset = offset % 8;
	m_Flags[index] = int(m_Flags[index]) | (1 << bitoffset);
	SetDirty();
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::UnsetMask(UInt32 offset)
{
	if(offset >= BitNum || !CheckMask(offset)) return;

	int index = offset / 8;
	int bitoffset = offset % 8;
	m_Flags[index] = int(m_Flags[index]) & ~(1 << bitoffset);
	SetDirty();
}

template<size_t BitNum>
bool CLMaskProperty<BitNum>::CheckMask(UInt32 offset) const
{
	if(offset >= BitNum) return false;

	int index = offset / 8;
	int bitoffset = offset % 8;
	return (int(m_Flags[index]) & (1 << bitoffset)) != 0;
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::ClearMask()
{
	memset(m_Flags, 0, sizeof(m_Flags));
	SetDirty();
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream.SeriaArray(m_Flags, CHAR_NUM);
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::Decode(Avalon::NetInputStream& stream)
{
	stream.SeriaArray(m_Flags, CHAR_NUM);
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::EncodeString(std::ostringstream& os)
{
	char buffer[CHAR_NUM * 2];
	Avalon::base64_encode(buffer, CHAR_NUM * 2, m_Flags, CHAR_NUM);
	os << buffer;
}

template<size_t BitNum>
void CLMaskProperty<BitNum>::DecodeString(std::istringstream& is)
{
	Avalon::base64_decode(m_Flags, CHAR_NUM, is.str().c_str(), is.str().length());
}

#endif

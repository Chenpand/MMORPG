/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	���ݱ�һ������
 */
#ifndef _AVALON_DATAROW_H_
#define _AVALON_DATAROW_H_

#include <AvalonDefine.h>
#include <AvalonStringUtil.h>
#include <string>

namespace Avalon
{
	/**
	 *@brief ������
	 */
	class DataRow
	{
		friend class DataTable;
	public:
		DataRow();
		~DataRow();

	public:
		void Clear();

		UInt32 GetColNum();
	public:

		std::string ReadString();

#define AVALON_DEFINE_DATAROW_READDATA(Type) \
		Type Read##Type() \
		{ \
			return StringUtil::ConvertToValue<Type>(ReadString()); \
		}

		AVALON_DEFINE_DATAROW_READDATA(Int8)
		AVALON_DEFINE_DATAROW_READDATA(UInt8)
		AVALON_DEFINE_DATAROW_READDATA(Int16)
		AVALON_DEFINE_DATAROW_READDATA(UInt16)
		AVALON_DEFINE_DATAROW_READDATA(Int32)
		AVALON_DEFINE_DATAROW_READDATA(UInt32)
		AVALON_DEFINE_DATAROW_READDATA(Int64)
		AVALON_DEFINE_DATAROW_READDATA(UInt64)
		AVALON_DEFINE_DATAROW_READDATA(Real32)
		AVALON_DEFINE_DATAROW_READDATA(Real64)

	private:
		//һ������
		std::string m_Data;
		//��ǰ����
		std::string::size_type m_Index;
	};
};

#endif

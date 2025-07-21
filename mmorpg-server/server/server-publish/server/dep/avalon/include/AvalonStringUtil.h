/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	定义了一些字符串辅助处理函数
 */
#ifndef _AVALON_STRINGUTIL_H_
#define _AVALON_STRINGUTIL_H_

#include "AvalonDefine.h"
#include <string>
#include <vector>
#include <cstdlib>

namespace Avalon
{
#define AVALON_STRINGVAL_LENGTH 64

	/**
	 *@brief 字符串工具类
	 */
	class StringUtil
	{
	public:

		/**
		 *@brief  数值转化为字符串
		 */
		template<typename T>
		static std::string ConvertToString(T val)
		{
			char buf[AVALON_STRINGVAL_LENGTH];
			ConvertToString(val, buf);
			return buf;
		}
		template<typename T>
		static void ConvertToString(T val, char* str);

		/**
		 *@brief  字符串转化为数值
		 */
		template<typename T>
		static T ConvertToValue(const std::string& str){ return ConvertToValue<T>(str.c_str()); }
		template<typename T>
		static T ConvertToValue(const char* str);

		/**
		 *@brief 去掉字符串两边的空白字符
		 */
		static void Trim(std::string& str);

		/**
		 *@brief 转化成小写
		 */
		static void ToLowerCase(std::string& str);

		/**
		 *@brief 转化成大写
		 */
		static void ToUpperCase(std::string& str);

		/**
		 *@brief 将一个字符串分割成用指定分割符分割的多个字符串
		 *@param str 原字符串
		 *@param strs 分割后字符串列表
		 */
		static void Split(const std::string& str,std::vector<std::string>& strs,const std::string& delims = " \t\r\n");

		/**
		 *@brief 16进制字符转化成数值
		 */
		static int HexToInt(char hex)
		{
			if(hex >= '0' && hex <= '9') return hex - '0';
			else if(hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
			else if(hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
			return 0;
		}

		/**
		 *@brief 数值转化为16进制数值                                                                     
		 */
		static char IntToHex(int val)
		{
			if(val >= 0 && val <= 9) return char('0' + val);
			else if(val <= 15) return char('A' + val - 10);
			return '0';
		}
	};

	template<>
	inline std::string StringUtil::ConvertToString<std::string>(std::string str)
	{
		return str;
	}
	
	template<>
	inline void StringUtil::ConvertToString<Int32>(Int32 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,"%d",val);
	}

	template<>
	inline void StringUtil::ConvertToString<UInt32>(UInt32 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,"%u",val);
	}

	template<>
	inline void StringUtil::ConvertToString<Int8>(Int8 val, char* str)
	{
		ConvertToString<Int32>(val, str);
	}

	template<>
	inline void StringUtil::ConvertToString<UInt8>(UInt8 val, char* str)
	{
		ConvertToString<Int32>(val, str);
	}

	template<>
	inline void StringUtil::ConvertToString<Int16>(Int16 val, char* str)
	{
		ConvertToString<Int32>(val, str);
	}

	template<>
	inline void StringUtil::ConvertToString<UInt16>(UInt16 val, char* str)
	{
		ConvertToString<Int32>(val, str);
	}

	template<>
	inline void StringUtil::ConvertToString<Int64>(Int64 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,AVALON_INT64_FORMAT,val);
	}

	template<>
	inline void StringUtil::ConvertToString<UInt64>(UInt64 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,AVALON_UINT64_FORMAT,val);
	}

	template<>
	inline void StringUtil::ConvertToString<Real32>(Real32 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,"%f",val);
	}

	template<>
	inline void StringUtil::ConvertToString<Real64>(Real64 val, char* str)
	{
		avalon_snprintf(str,AVALON_STRINGVAL_LENGTH,"%lf",val);
	}



	template<>
	inline Int32 StringUtil::ConvertToValue<Int32>(const char* str)
	{
		return atoi(str);
	}

	template<>
	inline Int8 StringUtil::ConvertToValue<Int8>(const char* str)
	{
		return (Int8)ConvertToValue<Int32>(str);
	}

	template<>
	inline UInt8 StringUtil::ConvertToValue<UInt8>(const char* str)
	{
		return (UInt8)ConvertToValue<Int32>(str);
	}

	template<>
	inline Int16 StringUtil::ConvertToValue<Int16>(const char* str)
	{
		return (Int16)ConvertToValue<Int32>(str);
	}

	template<>
	inline UInt16 StringUtil::ConvertToValue<UInt16>(const char* str)
	{
		return (UInt16)ConvertToValue<Int32>(str);
	}

	template<>
	inline UInt32 StringUtil::ConvertToValue<UInt32>(const char* str)
	{
		return (UInt32)avalon_atoll(str);
	}

	template<>
	inline Int64 StringUtil::ConvertToValue<Int64>(const char* str)
	{
		return avalon_atoll(str);
	}

	template<>
	inline UInt64 StringUtil::ConvertToValue<UInt64>(const char* str)
	{
		UInt64 val = 0;
		avalon_sscanf(str,AVALON_UINT64_FORMAT,&val);
		return val;
	}

	template<>
	inline Real32 StringUtil::ConvertToValue<Real32>(const char* str)
	{
		Real32 val = 0;
		avalon_sscanf(str,"%f",&val);
		return val;
	}

	template<>
	inline Real64 StringUtil::ConvertToValue<Real64>(const char* str)
	{
		return atof(str);
	}
}

#endif

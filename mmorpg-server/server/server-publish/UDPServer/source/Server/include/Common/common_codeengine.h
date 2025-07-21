#ifndef __COMMON_CODEENGINE_H__
#define __COMMON_CODEENGINE_H__

#include "common_base.h"
#include "common_namespace_definition.h"

SERVER_BEGIN

class CCodeEngine
{
public:

	static int32_t encode_int8(char **pOut,  uint8_t Src);
	static int32_t decode_int8(char **pIn,  uint8_t *pOut);
	static int32_t encode_int16(char **pOut,  uint16_t Src);
	static int32_t decode_int16(char **pIn,  uint16_t* pOut);

	static int32_t encode_int32(char **pOut,  uint32_t Src);
	static int32_t decode_int32(char **pIn,  uint32_t *pOut);

	/*
	* @method:    encode_string 编码string. 注意: 把'\0'也进行了编码,字符串长度包含'\0'
	* @fullname:  CCodeEngine::encode_string
	* @access:    public 
	* @param: char * * pOut
	* @param: const char * pSrc
	* @param: const int16_t nMaxStringLength 最大字符串长度.pSrc的最大长度
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	static int32_t encode_string(char **pOut, const char *pSrc, const int16_t nMaxStringLength);

	/*
	* @method:    decode_string 解码string
	* @fullname:  CCodeEngine::decode_string
	* @access:    public 
	* @param: char * * pIn
	* @param: char * pOut
	* @param: const int16_t nMaxStringLength pOut的最大长度
	* @return:   Game51::int32_t
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	static int32_t decode_string(char **pIn, char *pOut, const int16_t nMaxStringLength);

	static int32_t encode_memory(char **pOut, char *pSrc, const int32_t iMemorySize);
	static int32_t decode_memory(char **pIn, char *pOut, const int32_t iMemorySize);

	static int32_t encode_int64(char **pOut, uint64_t Src);
	static int32_t decode_int64(char **pIn, uint64_t *pOut);
protected:
private:
};

SERVER_END

#endif

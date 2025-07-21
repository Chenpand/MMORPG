/********************************************************************
created:	2008/02/18
created:	18:2:2008   9:52
filename: 	e:\51.COM\Source\Vss\Client\Lobby\include\Crypt.h
author:		veazhang

purpose:	
*********************************************************************/

#pragma once

#ifdef _MSC_VER
#include "../../../include/gameplatform_types.h"
#else
#include <stddef.h>
#include "gameplatform_types.h"
#include "Common/common_base.h"
#include "Common/common_string.h"
#endif
#include "common_namespace_definition.h"

SERVER_BEGIN


#define XTEA_KEY_LENGTH 16
#define MD5_HASH_LENGTH 16

class MD5Hash
{



public:
	typedef struct tagMD5HashValue
	{
		char arrby[MD5_HASH_LENGTH];
	}MD5HashValue;

};

class CXTEA
{
public:
	// 返回值是操作后的Buffer大小，失败则返回0
	static size_t Encrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16]);
	static size_t Decrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16]);

	static size_t EncryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16], int32_t n);
	static size_t DecryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16], int32_t n);

	// 获取足够大的加、解密输出Buffer的长度
	static size_t GetSafeEncryptOutBufferLength(size_t nInBufferLength);
	static size_t GetSafeDecryptOutBufferLength(size_t nInBufferLength);




public:
	typedef struct tagXTEAKey
	{
		char arrby[XTEA_KEY_LENGTH];
	}XTEAKey;

	enum{
		MAX_ENCRYPT_PADDING_LENGTH = 16,
	};
};


SERVER_END


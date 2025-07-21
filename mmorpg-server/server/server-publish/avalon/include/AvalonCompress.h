/**
 *@author	 
 *@email	 
 *@date		2012-10-22	
 *@brief	md5算法
 */
#ifndef _AVALON_COMPRESS_H_
#define _AVALON_COMPRESS_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief 压缩
	 */
	void snappy_compress(char* dest, size_t* destLen, const char* src, size_t srcLen);

	/**
	 *@brief 解压缩
	 */
	bool snappy_uncompress(char* dest, const char* src, size_t srcLen);

	/**
	 *@brief 获取压缩后最大大小
	 */
	size_t snappy_maxcompressedlen(size_t len);

	/**
	 *@brief 获取解压缩后大小
	 */
	bool snappy_uncompressedlen(const char* src, size_t srcLen, size_t* result);
}

#endif

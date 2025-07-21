/**
 *@author	 
 *@email	 
 *@date		2012-10-22	
 *@brief	md5�㷨
 */
#ifndef _AVALON_COMPRESS_H_
#define _AVALON_COMPRESS_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief ѹ��
	 */
	void snappy_compress(char* dest, size_t* destLen, const char* src, size_t srcLen);

	/**
	 *@brief ��ѹ��
	 */
	bool snappy_uncompress(char* dest, const char* src, size_t srcLen);

	/**
	 *@brief ��ȡѹ��������С
	 */
	size_t snappy_maxcompressedlen(size_t len);

	/**
	 *@brief ��ȡ��ѹ�����С
	 */
	bool snappy_uncompressedlen(const char* src, size_t srcLen, size_t* result);
}

#endif

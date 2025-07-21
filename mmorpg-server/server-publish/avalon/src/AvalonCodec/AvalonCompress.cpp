#include "AvalonCompress.h"
#include "snappy/snappy.h"

namespace Avalon
{
	void snappy_compress(char* dest, size_t* destLen, const char* src, size_t srcLen)
	{
		snappy::RawCompress(src, srcLen, dest, destLen);
	}

	bool snappy_uncompress(char* dest, const char* src, size_t srcLen)
	{
		return snappy::RawUncompress(src, srcLen, dest);
	}

	size_t snappy_maxcompressedlen(size_t len)
	{
		return snappy::MaxCompressedLength(len);
	}

	bool snappy_uncompressedlen(const char* src, size_t srcLen, size_t* result)
	{
		return snappy::GetUncompressedLength(src, srcLen, result);
	}
}

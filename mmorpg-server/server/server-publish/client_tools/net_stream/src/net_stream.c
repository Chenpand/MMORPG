#include "net_stream.h"

#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#pragma  comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#endif

int encode_int8(char *buf, int* pos, uint8_t val)
{
	if (!buf || !pos)
	{
		return -1;
	}

	*((uint8_t*)(buf + *pos)) = val;
	*pos += 1;

	return 0;
}

int decode_int8(char *buf, int* pos, uint8_t* val)
{
	if (!buf || !pos || !val)
	{
		return -1;
	}

	*val = *((uint8_t*)(buf + *pos));
	*pos += 1;

	return 0;
}

int encode_uint16(char *buf, int* pos, uint16_t val)
{
	if (!buf || !pos)
	{
		return -1;
	}

	*((uint16_t*)(buf + *pos)) = val;//htons(val);
	*pos += 2;

	return 0;
}

int decode_uint16(char *buf, int* pos, uint16_t* val)
{
	if (!buf || !pos || !val)
	{
		return -1;
	}

	*val = *((uint16_t*)(buf + *pos));//ntohs(*((uint16_t*)(buf + *pos)));
	*pos += 2;

	return 0;
}

int encode_int16(char *buf, int* pos, int16_t val)
{
    return encode_uint16(buf, pos, (uint16_t)val);
}

int decode_int16(char *buf, int* pos, int16_t* val)
{
    return decode_uint16(buf, pos, (uint16_t*)val);
}

int encode_uint32(char *buf, int* pos, uint32_t val)
{
	if (!buf || !pos)
	{
		return -1;
	}

	*((uint32_t*)(buf + *pos)) = val;//htonl(val);
	*pos += 4;

	return 0;
}

int decode_uint32(char *buf, int* pos, uint32_t* val)
{
	if (!buf || !pos || !val)
	{
		return -1;
	}

	*val = *((uint32_t*)(buf + *pos));// ntohl(*((unsigned int*)(buf + *pos)));
	*pos += 4;

	return 0;
}

int encode_int32(char *buf, int* pos, int32_t val)
{
    return encode_uint32(buf, pos, (uint32_t)val);
}

int decode_int32(char *buf, int* pos, int32_t* val)
{
    return decode_uint32(buf, pos, (uint32_t*)val);
}

int encode_uint64(char *buf, int* pos, uint64_t val)
{
	if (!buf || !pos)
	{
		return -1;
	}

	*((uint64_t*)(buf + *pos)) = val;// htonll(val);
	*pos += 8;

	return 0;
}

int decode_uint64(char *buf, int* pos, uint64_t* val)
{
	if (!buf || !pos || !val)
	{
		return -1;
	}

	*val = *((uint64_t*)(buf + *pos));// ntohll(*((unsigned long long*)(buf + *pos)));
	*pos += 8;

	return 0;
}

int encode_int64(char *buf, int* pos, int64_t val)
{
    return encode_uint64(buf, pos, (uint64_t)val);
}

int decode_int64(char *buf, int* pos, int64_t* val)
{
    return decode_uint64(buf, pos, (uint64_t*)val);
}

int encode_string(char *buf, int* pos, const char* str, uint16_t max_length)
{
	if (!buf || !pos || !str || max_length < 0)
	{
		return -1;
	}

	// 这里不包括'\0'
	uint16_t str_length = (uint16_t)strlen(str);
	if (str_length > max_length)
	{
		str_length = max_length;
	}
	
	encode_uint16(buf, pos, str_length);
	memcpy(buf + *pos, str, str_length);
	
	*pos += str_length;

	return 0;
}

int decode_string(char *buf, int* pos, char* str, uint16_t max_length)
{
	if (!buf || !pos || !str || max_length < 0)
	{
		return -1;
	}

    uint16_t str_length = 0;
	decode_uint16(buf, pos, &str_length);
	
    uint16_t copy_length = str_length;
	if (copy_length > max_length)
	{
		copy_length = max_length;
	}

	memcpy(str, buf + *pos, copy_length);
	*pos += str_length;

	return 0;
}

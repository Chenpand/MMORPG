#ifndef __NET_STREAM_H__
#define __NET_STREAM_H__

#include <stdint.h>

#ifdef STREAM_DLL_EXPORTS
#ifdef _WIN32
#define STREAM_API __declspec(dllexport)
#else
#define STREAM_API
#endif
#else
#ifdef _WIN32
#define STREAM_API __declspec(dllimport)
#else
#define STREAM_API
#endif
#endif

#ifdef __cplusplus 
extern "C" {
#endif

STREAM_API int encode_int8(char *buf, int* pos, uint8_t val);
STREAM_API int decode_int8(char *buf, int* pos, uint8_t* val);

STREAM_API int encode_uint16(char *buf, int* pos, uint16_t val);
STREAM_API int decode_uint16(char *buf, int* pos, uint16_t* val);

STREAM_API int encode_int16(char *buf, int* pos, int16_t val);
STREAM_API int decode_int16(char *buf, int* pos, int16_t* val);

STREAM_API int encode_uint32(char *buf, int* pos, uint32_t val);
STREAM_API int decode_uint32(char *buf, int* pos, uint32_t* val);

STREAM_API int encode_int32(char *buf, int* pos, int32_t val);
STREAM_API int decode_int32(char *buf, int* pos, int32_t* val);

STREAM_API int encode_uint64(char *buf, int* pos, uint64_t val);
STREAM_API int decode_uint64(char *buf, int* pos, uint64_t* val);

STREAM_API int encode_int64(char *buf, int* pos, int64_t val);
STREAM_API int decode_int64(char *buf, int* pos, int64_t* val);

STREAM_API int encode_string(char *buf, int* pos, const char* str, uint16_t max_length);
STREAM_API int decode_string(char *buf, int* pos, char* str, uint16_t max_length);

#ifdef __cplusplus 
}
#endif

#endif
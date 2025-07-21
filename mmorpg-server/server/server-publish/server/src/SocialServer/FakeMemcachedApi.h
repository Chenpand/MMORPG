#ifndef _FAKE_MEMCACHED_API_H_
#define _FAKE_MEMCACHED_API_H_

#include <AvalonDefine.h>

struct memcached_st{};

enum  memcached_return_t
{
	MEMCACHED_SUCCESS,
	MEMCACHED_FAILURE
};

struct memcached_server_st{};

typedef memcached_server_st* memcached_server_list_st;

inline memcached_st* memcached_create(memcached_st *ptr)
{
	static memcached_st default_st;
	return &default_st;
}

inline memcached_server_list_st memcached_server_list_append(memcached_server_list_st list, const char *hostname, unsigned int port, memcached_return_t *error)
{
	static memcached_server_st default_st;
	return &default_st;
}

inline memcached_return_t memcached_server_push(memcached_st *ptr, const memcached_server_st *list)
{
	return MEMCACHED_SUCCESS;
}

inline void memcached_server_list_free(memcached_server_list_st list){}

inline memcached_return_t  memcached_set  (memcached_st  *ptr,  const  char  *key,  size_t key_length, const char *value,
	 size_t value_length, time_t expiration, unsigned int flags)
{
	return MEMCACHED_SUCCESS;
}

inline memcached_return_t  memcached_add  (memcached_st  *ptr,  const  char  *key,  size_t key_length, const char *value,
	 size_t value_length, time_t expiration, unsigned int flags)
{
	return MEMCACHED_SUCCESS;
}

inline char * memcached_get(memcached_st *ptr, const char *key, size_t key_length, size_t *value_length, unsigned int *flags, memcached_return_t *error)
{
	return NULL;
}

inline memcached_return_t memcached_mget(memcached_st *ptr, const char* const *keys, const size_t* key_length, size_t number_of_keys)
{
	return MEMCACHED_SUCCESS;
}

inline memcached_return_t memcached_delete (memcached_st *ptr, const char *key, size_t key_length, time_t expiration)
{
	return MEMCACHED_SUCCESS;
}

inline void memcached_free(memcached_st *ptr){}

inline const char* memcached_strerror(memcached_st *ptr, memcached_return_t rc)
{
	return "";
}

inline char* memcached_fetch(memcached_st* ptr, char* key, size_t *key_length, size_t *value_length, UInt32 *flags, memcached_return_t *error)
{
	return NULL;
}

struct memcached_result_st
{
};



inline memcached_result_st* memcached_fetch_result(memcached_st* ptr, memcached_result_st* result, memcached_return_t* error)
{
	return NULL;
}

inline const char* memcached_result_value(memcached_result_st* ptr)
{
	return NULL;
}

inline void memcached_result_free(memcached_result_st* ptr)
{
	return;
}

enum memcached_behavior
{
	MEMCACHED_BEHAVIOR_BINARY_PROTOCOL,
};

inline memcached_return_t memcached_behavior_set(memcached_st* ptr, memcached_behavior flag, UInt64 data)
{
	return MEMCACHED_SUCCESS;
}

#endif

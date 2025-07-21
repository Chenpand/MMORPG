#ifndef __COMMON_BITMAP_H__
#define __COMMON_BITMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common_namespace_definition.h"

SERVER_BEGIN

enum
{
	bytesize = (int32_t)(8*sizeof(unsigned char)),
};

//bitm container
template<int bitmap_size>
class CBitmap
{
public:	
	enum 
	{
		__MAX_BITMAP_SIZE = bitmap_size,
	};
	//copy constructor
	 CBitmap<bitmap_size>& operator =(const CBitmap<bitmap_size>& origianl);

	//clear all bits
	int32_t clear();
	
	//is the bits[idx] setted ?
    int32_t is_bit_setted(int32_t idx);

	//set the bit of idx
    int32_t set_bit(int32_t idx);

	//clear the bit of idx
    int32_t unset_bit(int32_t idx);
    
	//just set all bits
    int32_t set_all_bits();
    //int GetSettedBits(int *piBitSeqs);

//     //判断从0 ~ bits-1位是否都未设置
//     //return value : 1 -- true; 0 -- otherwise
//     int IsSomeEmpty(int bits);
// 
//     int GetMapMem(int& riSize, unsigned char * pbyMap);
//     void Show(FILE *fpOut);

private:
    unsigned char m_array[__MAX_BITMAP_SIZE];
};

template<int bitmap_size>
CBitmap<bitmap_size>& CBitmap<bitmap_size>::operator =(const CBitmap<bitmap_size>& origianl)
{
	if (this == &origianl)
	{
		return (*this);
	}

    memcpy((void *)m_array, (const void *)origianl.m_array, sizeof(m_array));
    return (*this);
}

template<int bitmap_size>
int32_t CBitmap<bitmap_size>::clear()
{
	memset((void *)m_array, 0, sizeof(m_array));
	return 0;
}

template<int bitmap_size>
int32_t CBitmap<bitmap_size>::is_bit_setted(int32_t idx)
{
    int32_t setcion_idx = 0;
    int32_t bit_idx = 0;
    unsigned char magic = 0x80;

    if( (idx > (bitmap_size * bytesize)) || (idx < 0))
    {
        return 0;
    }

    setcion_idx = idx / bytesize;
    bit_idx = idx % bytesize;

    if( m_array[setcion_idx] & (magic >> bit_idx) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// template<int bitmap_size>
// int CBitmap<bitmap_size>::IsSomeEmpty(int bits)
// {
//     int i = 0;
//     if (bits <= 0 || bits > bitmap_size*bytesize )
//     {
//         return 0;
//     }
// 
//     for(i = 0; i < bits; i++)
//     {
//         if (is_bit_setted(i) != 0)
//         {
//             return 0;
//         }
//     }
// 
//     return 1;
// }


template<int bitmap_size>
int32_t CBitmap<bitmap_size>::set_bit(int32_t idx)
{
    int setcion_idx = 0;
    int bit_idx = 0;
    unsigned char magic = 0x80;
    
	if( (idx > (bitmap_size * bytesize)) || (idx < 0))
	{
		return 0;
	}

    setcion_idx = idx / bytesize;
    bit_idx = idx % bytesize;

    m_array[setcion_idx] |= (magic >> bit_idx);

    return 0;
}

template<int bitmap_size>
int32_t CBitmap<bitmap_size>::unset_bit(int32_t idx)
{
	int setcion_idx = 0;
	int bit_idx = 0;
	unsigned char magic = 0x80;

	if( (idx > (bitmap_size * bytesize)) || (idx < 0))
	{
		return 0;
	}

    setcion_idx = idx / bytesize;
    bit_idx = idx % bytesize;

    m_array[setcion_idx] &= (0xff ^ (magic >> bit_idx));

    return 0;
}



template<int bitmap_size>
int32_t CBitmap<bitmap_size>::set_all_bits()
{
    memset(m_array, 0xff, sizeof(m_array));
    return 0;
}

// template<int bitmap_size>
// int CBitmap<bitmap_size>::GetSettedBits(int *piBitSeqs)
// {
//     int iBitCount;
//     int i;
// 
//     if( !piBitSeqs )
//     {
//         return -1;
//     }
//     iBitCount = 0;
// 
//     for( i = 0; i < bitmap_size*bytesize; i++ )
//     {
//         if( is_bit_setted(i) )
//         {
//             piBitSeqs[iBitCount] = i;
//             iBitCount++;
//         }
//     }
// 
//     return iBitCount;
// }

// template<int bitmap_size>
// int CBitmap<bitmap_size>::GetMapMem(int& riSize, unsigned char *pbyMap)
// {
//     riSize = bitmap_size;
//     memcpy((void *)pbyMap, (const void *)m_array, riSize);
//     return 0;
// }
// 
// template<int bitmap_size>
// void CBitmap<bitmap_size>::Show(FILE *fpOut)
// {
//     int i,j;
// 
//     if( !fpOut )
//     {
//         return;
//     }
//     fprintf(fpOut, "BITMAP:\n");
// 
//     for( i = 0; i < bitmap_size*bytesize; i++ )
//     {
//         fprintf(fpOut, "%2d", is_bit_setted(i));
//     }
// 
//     fprintf(fpOut, "\n");
//     fflush(fpOut);
// }

SERVER_END


#endif /* __COMMON_BITMAP_H__ */

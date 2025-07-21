#ifndef __COMMON_SET_H__
#define __COMMON_SET_H__

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
//#include <time.h>

#include "Common/common_base.h"
#include "common_namespace_definition.h"

SERVER_BEGIN


template< class T, int32_t Count>
class CSet
{
public:
	typedef T value_type;
	typedef T * iterator;
	typedef T * const const_iterator;
	typedef T & reference;
	typedef const T &  const_reference;

	enum
	{
		__COUNT__ = Count,
	};

public:
	//
	CSet();
	//
	~CSet();
	
	
	int32_t add_item( T& stItem);
	int32_t del_item(int32_t idx);

	int32_t last();
	int32_t clear();
	int32_t capacity();
	
	CSet& operator = ( CSet& original);
	reference operator []( int32_t idx );
	
	int32_t begin (void)
	{
		return 0;
	}

	int32_t end (void)
	{
		return m_iLastItem;
	}

	bool full (void)
	{
		return (Count == m_iLastItem);
	}

	bool empty (void)
	{
		return (0 >= m_iLastItem);
	}

private:
	value_type m_astItems[__COUNT__];
	int32_t m_iLastItem;
	int32_t m_iMaxCount;
	
};

template<class T, int32_t Count>
CSet<T, Count>::CSet()
{
	m_iMaxCount = Count;
}

template<class T, int32_t Count>
CSet <T, Count>::~CSet()
{
	//nothing
}

template<class T, int32_t Count>
CSet<T, Count>& CSet<T, Count>::operator = ( CSet<T, Count>& original)
{
	if (this == &original)
	{
		return *this;
	}	

	m_iLastItem = original.m_iLastItem;
	if (m_iLastItem > Count)
	{
		m_iLastItem = Count;
	}
	for (int16_t i = 0; i < m_iLastItem; i++)
	{
		m_astItems[i] = original.m_astItems[i];
	}

	return (*this);
}

template<class T, int32_t Count>
int32_t CSet<T, Count>::clear()
{
	m_iLastItem = 0;
	return 0;
}

template<class T, int32_t Count>
T& CSet <T, Count>::operator [](int32_t idx)
{
	if( idx < 0 )
	{
		return m_astItems[0];
	}
	if( idx >= m_iLastItem )
	{
			
		if (m_iLastItem > 0)
		{
			return m_astItems[m_iLastItem-1];
		}
		else
		{
			return m_astItems[0];
		}	
		
	}
	
	return m_astItems[idx];
}

template<class T, int32_t Count>
int32_t CSet<T, Count>::add_item( T& stItem )
{
	int32_t result = -1;
	
	result = m_iLastItem;
	
	if( m_iLastItem < Count )
	{
		m_astItems[m_iLastItem] = stItem;
		++m_iLastItem;
	}
	
	return result;
}

template<class T, int32_t Count>
int32_t CSet<T, Count>::del_item( int32_t idx )
{
	if( idx < 0 || idx >= m_iLastItem )
	{
		return idx;
	}
		
	m_iLastItem--;
	if( m_iLastItem < 0 )
	{
		m_iLastItem = 0;
		return m_iLastItem;
	}
	
	m_astItems[idx] = m_astItems[m_iLastItem];
		
	return idx;
}

template<class T, int32_t Count>
int32_t CSet<T, Count>::last()
{
	return m_iLastItem;
}

template<class T, int32_t Count>
int32_t CSet<T, Count>::capacity()
{
	return Count;
}

SERVER_END

#endif //__COMMON_SET_H__



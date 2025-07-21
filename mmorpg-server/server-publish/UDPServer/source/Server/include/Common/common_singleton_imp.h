/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __COMMON_SINGLETONIMPL_H__
#define __COMMON_SINGLETONIMPL_H__

#include "common_singleton.h"
#include "common_namespace_definition.h"

SERVER_BEGIN

// avoid the using namespace here cuz
// its a .h file afterall

template<typename T,class CreatePolicy>
T& Singleton<T, CreatePolicy>::Instance()
{
    if(NULL == si_instance)
    {
       si_instance = CreatePolicy::Create();
        
    }
	si_destroyed = false;
    return *si_instance;
}

template<typename T,class CreatePolicy>
void Singleton<T,CreatePolicy>::DestroySingleton()
{
    CreatePolicy::Destroy(si_instance);
    si_instance = NULL;
    si_destroyed = true;
}

#define INSTANTIATE_SINGLETON_1(TYPE) \
    template class Singleton<TYPE,OperatorNew<TYPE> >; \
    template<> TYPE* Singleton<TYPE, OperatorNew<TYPE> >::si_instance = NULL; \
    template<> bool Singleton<TYPE, OperatorNew<TYPE> >::si_destroyed = false


#define INSTANTIATE_SINGLETON_2(TYPE, CREATIONPOLICY) \
    template class Singleton<TYPE, CREATIONPOLICY >; \
    template<> TYPE* Singleton<TYPE, CREATIONPOLICY>::si_instance = NULL; \
    template<> bool Singleton<TYPE,CREATIONPOLICY >::si_destroyed = false

SERVER_END
#endif

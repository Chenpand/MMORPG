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

#ifndef __COMMON_SINGLETON_H__
#define __COMMON_SINGLETON_H__

//#include "common_creation_policy.h"
#include "common_namespace_definition.h"
#include "Common/common_base.h"
/**
 * @brief class Singleton
 */

/*
 * @defgroup common_singleton
 * @author  51GameServer
 * @version 
 * @date    2008/02/22
 * @purpose 
 * @{
 */
SERVER_BEGIN



 template<typename T> //, class CreatePolicy = OperatorNew<T> > 
	class  Singleton
	{
        public:
            static T& Instance()
			{
				if (NULL == si_instance)
				{
					si_instance = new T;
				}

				return *si_instance;
			}

        protected:
            Singleton() {};

        private:

            // Prohibited actions...this does not prevent hijacking.
            Singleton(const Singleton &);
            Singleton& operator=(const Singleton &);

            // Singleton Helpers
            //static void DestroySingleton();

            
            static T *si_instance;
            //static bool si_destroyed;
	};

	template<typename T>
	T* Singleton<T>::si_instance = NULL;


SERVER_END

/**//** @}*/ 

#endif

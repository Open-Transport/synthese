
/// RegistryWithAutocreationEnabled class header.
///	@file RegistryWithAutocreationEnabled.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_util_RegistryWithAutocreationEnabled_h__
#define SYNTHESE_util_RegistryWithAutocreationEnabled_h__

#include "Registry.h"

namespace synthese
{
	namespace util
	{
		/** Registry class permitting auto creation of objects by get method if not found.
			@warning A RegistryWithAutocreationEnabled is incompatible with abstract classes.
			For abstract classes use classic Registry instead.
			@ingroup m01Registry
		*/
		template<class T>
		class RegistryWithAutocreationEnabled
		:	public Registry<T>
		{
		public:
			RegistryWithAutocreationEnabled()
				: Registry<T>()
			{

			}


			boost::shared_ptr<const T> getWithAutoCreation(
				const RegistryKeyType& key
			){
				try
				{
					return Registry<T>::get(key);
				}
				catch(ObjectNotFoundException<T>)
				{
					boost::shared_ptr<T> object(new T(key));
					this->add(object);
					return boost::const_pointer_cast<const T, T>(object);
				}
			}


			boost::shared_ptr<T> getEditableWithAutoCreation(
				const RegistryKeyType& key
			){
				try
				{
					return Registry<T>::	getEditable(key);
				}
				catch(ObjectNotFoundException<T>)
				{
					boost::shared_ptr<T> object(new T(key));
					this->add(object);
					return object;
				}
			}
		};
	}
}

#endif // SYNTHESE_util_RegistryWithAutocreationEnabled_h__

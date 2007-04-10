
/** Registrable class header.
	@file Registrable.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_UTIL_REGISTRABLE_H
#define SYNTHESE_UTIL_REGISTRABLE_H

#include <map>

#include <string>
#include <iostream>

#include "01_util/Registry.h"

namespace synthese
{
	namespace util
	{

		/** Generic registry class for common environment
			operations (get, add, remove...).
			@warning The registry only has the responsability of destroying registered objects. So do not create shared pointers on Registrable object, use getRegisteredSharedPointer method instead.
			@note A Registrable object must be registered in only one registry.

			@ingroup m01
		*/
		template<class K, class T>
		class Registrable
		{
		public:
			typedef typename K								KeyType;
			typedef typename Registry<K, T>					Registry;
			typedef typename RegistryKeyException<K, T>		RegistryKeyException;

		private:
			K					_key;		//!< The key of the object in the registry.
			Registry*			_registry;	//!< The official registry of the object (updated at the first insertion)


		public:
			Registrable();
			Registrable (const K& key);
			virtual ~Registrable ();


			//! @name Getters/Setters
			//@{
				const K&					getKey () const;
				void						setKey(const K& key);
//				const K& getId () const;
			//@}

			//! @name Calculators
			//@{
				/** Gets the registered shared pointer if exists.
					@return boost::shared_ptr<const T> The unique shared pointer to use, a null one if the object is not registered.
					@author Hugues Romain
					@date 2007

					This method gives the registered shared pointer to this object, in 
					order to avoid to create multiple shared pointers to the object, with 
					the risk of an auto deletion.
					
				*/
				boost::shared_ptr<const T>	getRegisteredSharedPointer() const;
			//@}

			friend class util::Registry<K, T>;

		};

		template<class K, class T>
		boost::shared_ptr<const T> Registrable<K, T>::getRegisteredSharedPointer() const
		{
			return _registry ? _registry->get(_key) : shared_ptr<const T>();
		}

		template<class K, class T>
			synthese::util::Registrable<K, T>::Registrable()
			: _key(0), _registry(NULL)
		{

		}



		template<class K,class T>
		Registrable<K,T>::Registrable (const K& key)
			: _key (key), _registry(NULL)
		{
		}


		template<class K, class T>
		Registrable<K,T>::~Registrable ()
		{
		}


		template<class K, class T>
		const K&
		Registrable<K,T>::getKey () const
		{
			return _key; 
		}


		template<class K, class T>
		void
		Registrable<K,T>::setKey (const K& key)
		{
			_key = key;
		}

		/*
		template<class K, class T>
		const K&
		Registrable<K,T>::getId () const
		{
			return _key; 
		}
		*/

	}
}
#endif

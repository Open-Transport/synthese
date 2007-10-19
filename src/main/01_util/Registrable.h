
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
#include <set>

#include <string>
#include <iostream>

#include "01_util/Registry.h"
#include "01_util/Constants.h"

namespace synthese
{
	namespace util
	{

		/** Generic registry class for common environment
			operations (get, add, remove...).
			@warning The registry only has the responsibility of destroying registered objects. So do not create shared pointers on Registrable object, use getRegisteredSharedPointer method instead.
			@note A Registrable object must be registered in only one registry.

			@ingroup m01
		*/
		template<class K, class T>
		class Registrable
		{
		public:
			typedef K										KeyType;
			typedef Registry<K, T>							Registry;
			typedef RegistryKeyException<K, T>				RegistryKeyException;
			typedef ObjectNotFoundException<K,T>			ObjectNotFoundException;
			typedef ObjectNotFoundInRegistryException<K,T>	ObjectNotFoundInRegistryException;
			typedef typename Registry::const_iterator		ConstIterator;
			typedef	std::set<const void*>					ChildTemporaryObjects;

		private:
			K						_key;		//!< The key of the object in the registry.
			
			static Registry			_registry;	//!< The official registry of the object (updated at the first insertion)

			bool					_linked;
			ChildTemporaryObjects	_childTemporaryObjects;


		public:
			Registrable();
			Registrable (const K& key);
			virtual ~Registrable ();

			void	store();
			void	remove();

			static boost::shared_ptr<const T>	Get(const K& key);
			static boost::shared_ptr<T>			GetUpdateable(const K& key);
			static bool Contains(const K& key);
			static ConstIterator Begin();
			static ConstIterator End();
			static void Remove(const K& key);

			
			//! @name Getters/Setters
			//@{
				const K&	getKey () const;
				void		setKey(const K& key);
				bool		getLinked()	const		{ return _linked; }
				void		setLinked(bool value)	{ _linked = value; }
			//@}

				template<class C>
				void addChildTemporaryObject(C* object)
				{
					_childTemporaryObjects.insert(static_cast<const void*>(object));
				}

				void clearChildTemporaryObjects()
				{
					for (ChildTemporaryObjects::iterator it(_childTemporaryObjects.begin()); it != _childTemporaryObjects.end(); ++it)
						delete *it;
					_childTemporaryObjects.clear();
					_linked = false;
				}

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

//			friend class util::Registry<K, T>;

		};

		
		template<class K, class T> 
		    typename Registrable<K, T>::Registry Registrable<K, T>::_registry;


		template<class K, class T>
		typename Registrable<K, T>::ConstIterator Registrable<K, T>::Begin()
		{
			return _registry.begin();
		}



		template<class K, class T>
		typename Registrable<K, T>::ConstIterator Registrable<K, T>::End()
		{
			return _registry.end();
		}


		template<class K, class T>
		boost::shared_ptr<const T> Registrable<K, T>::Get(const K& key)
		{
			return _registry.get(key);
		}



		template<class K, class T>
		boost::shared_ptr<T> Registrable<K, T>::GetUpdateable(const K& key)
		{
			return _registry.getUpdateable(key);
		}



		template<class K, class T>
		bool Registrable<K, T>::Contains(const K& key)
		{
			return _registry.contains(key);
		}



		template<class K, class T>
		void Registrable<K, T>::Remove(const K& key)
		{
			return _registry.remove(key);
		}



		template<class K, class T>
		void Registrable<K, T>::remove()
		{
			_registry.remove(_key);
		}
		
		
		template<class K, class T>
		void Registrable<K, T>::store()
		{
			_registry.add(boost::shared_ptr<T>(static_cast<T*>(this)));
		}
		
		
		template<class K, class T>
		boost::shared_ptr<const T> Registrable<K, T>::getRegisteredSharedPointer() const
		{
		    return _registry.get(_key);
		}

		template<class K, class T>
			synthese::util::Registrable<K, T>::Registrable()
			: _key(UNKNOWN_VALUE)
			, _linked(false)
		{

		}



		template<class K,class T>
		Registrable<K,T>::Registrable (const K& key)
			: _key (key)
			, _linked(false)
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

	}
}

#endif

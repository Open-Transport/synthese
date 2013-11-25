////////////////////////////////////////////////////////////////////////////////
/// Registry class header.
///	@file Registry.h
///	@author Hugues Romain
///	@date 2008-12-26 19:29
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_UTIL_REGISTRY_H
#define SYNTHESE_UTIL_REGISTRY_H

#include "RegistryBase.h"

#include "UtilTypes.h"
#include "UtilConstants.h"
#include "RegistryKeyException.h"
#include "ObjectNotFoundInRegistryException.h"
#include "ObjectNotFoundException.h"

#include <map>
#include <boost/foreach.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace util
	{
		/** @defgroup m01Registry Registry and environments.
			@ingroup m01
			@{
		*/

		/** Generic registry class for common operations (get, add, remove...).
			Note that the registry has the responsibility of destroying	registered objects due to the use of shared pointers.
			This class should not be used directly : use instead the "template" typedef in the classes derived from Registrable.

			A registry stores a collection of objects making an environment.

			It is possible to erase a single element in the ordered way of use : the pop_back method removes
			the least element of the vector and the corresponding record in the map.

			The order of the elements is the order of insertions.

			@section registryDeclaration Declaration

			Each registry template instantiation must be declared :
				- call of Env::template Integrate<T>() in the .cpp.gen file of the module of T
				- inclusion of T.h in the .cpp.inc file of the module of T
				- instantiation of the static variable KEY filled by a unique identifier of the T class (usually the class name itself)

			Example of codes :

			@code
			#include "T.h"
			@endcode

			@code
			synthese::util::Env::template Integrate<synthese::module::T>();
			@endcode

			@code
			namespace util
			{
				template<> const std::string Registry<module::T>::KEY("T");
			}
			@endcode
		*/
		template<class T>
		class Registry:
			public RegistryBase
		{
			//lint --e{1704}

		public:
			typedef std::map<RegistryKeyType, boost::shared_ptr<T> > Map;
			typedef T ObjectsClass;
			typedef typename Map::iterator iterator;
			typedef typename Map::const_iterator const_iterator;
			typedef typename Map::value_type value_type;
			typedef typename Map::reverse_iterator reverse_iterator;
			typedef typename Map::const_reverse_iterator const_reverse_iterator;

		private:

			 Map	_registry;
			 mutable boost::recursive_mutex _mutex;

		 public:

			Registry(): RegistryBase() {}

			/** Static registry key.
				Used by Env class to select the whole registry from a string designing the class.
			*/
			static const std::string KEY;

			//! @name Query methods
			//@{
				boost::recursive_mutex& getMutex() const { return _mutex; }

				virtual bool contains (RegistryKeyType key) const
				{
					boost::recursive_mutex::scoped_lock lock(_mutex);
					return _registry.find(key) != _registry.end();
				}

				/** Gets a shared pointer to a registered object with read permission.
					@param key key of the object to find
					@return boost::shared_ptr<T> the object found
					@throws ObjectNotFoundInRegistryException<T> if the key does not exists
					in the registry and autoCreate is false
				*/
				boost::shared_ptr<const T> get(
					RegistryKeyType key
				) const;



				/** Gets a shared pointer to a registered object with write permission.
					@param key key of the object to find
					@return boost::shared_ptr<T> the object found
					@throws ObjectNotFoundInRegistryException<T> if the key does not exists
					in the registry and autoCreate is false
				*/
				const boost::shared_ptr<T>& getEditable(
					RegistryKeyType key
				) const;



				virtual boost::shared_ptr<Registrable> getEditableObject(
					RegistryKeyType key
				) const {
					return boost::static_pointer_cast<Registrable, T>(getEditable(key));
				}



				virtual size_t size() const { return _registry.size(); }
				bool empty() const { return _registry.empty(); }
				iterator begin() { return _registry.begin(); }
				const_iterator begin() const { return _registry.begin(); }
				iterator end() { return _registry.end(); }
				const_iterator end() const { return _registry.end(); }
				reverse_iterator rbegin() { return _registry.rbegin(); }
				const_reverse_iterator rbegin() const { return _registry.rbegin(); }
				reverse_iterator rend() { return _registry.rend(); }
				const_reverse_iterator rend() const { return _registry.rend(); }
			//@}


			//! @name Update methods
			//@{
				/** Removes and destroy all the registered objects.
				*/
				void clear ()
				{
					boost::recursive_mutex::scoped_lock lock(_mutex);
					_registry.clear();
				}



				/** Adds an object to the registry.
					@param ptr Shared pointer to the object to add
					@throws RegistryKeyException if the key of the object is 0 or if the keys is already used in the registry
				*/
				void add(
					const boost::shared_ptr<T>& ptr
				);


				/** Adds an object to the registry.
					@param ptr Shared pointer to the object to add
					@throws RegistryKeyException if the key of the object is 0 or if the keys is already used in the registry
				*/
				virtual void addRegistrable(
					const boost::shared_ptr<Registrable>& ptr
				);


				/** Replaces an object in the registry.
					@param ptr Shared pointer to the new object to add instead of the old one
					@throws RegistryKeyException if the key of the object is 0 (0 is reserved for a const neutral element)
					@throws ObjectNotFoundInRegistryException if no object with the same key was existing in the registry
					This method cleans the ordered vector.
				*/
				void replace(
					const boost::shared_ptr<T>& ptr
				);



				/** Removes an object from the registry.
					@param key key of the object to remove
					@throws RegistryKeyException if the key of the object is 0 (0 is reserved for a const neutral element)
					@throws ObjectNotFoundInRegistryException if the key is not found in the registry
					This method cleans the ordered vector.
				*/
				virtual void remove(RegistryKeyType key);
			//@}


			virtual RegistrablesVector getRegistrablesVector() const ;

		 private:

			Registry ( const Registry& ref );
			Registry& operator= ( const Registry& rhs );
		};



		template<class T>
		void Registry<T>::addRegistrable(
			const boost::shared_ptr<Registrable>& ptr
		){
			if(!dynamic_cast<T*>(ptr.get()))
			{
				throw synthese::Exception("Bad registry");
			}
			add(boost::dynamic_pointer_cast<T, Registrable>(ptr));
		}



		template<class T>
		RegistryBase::RegistrablesVector Registry<T>::getRegistrablesVector() const
		{
			RegistrablesVector r;
			boost::recursive_mutex::scoped_lock lock(_mutex);
			BOOST_FOREACH(const typename Registry<T>::Map::value_type& item, _registry)
			{
				r.push_back(boost::static_pointer_cast<Registrable, T>(item.second));
			}
			return r;
		}

		/** @} */



		template<class T>
		const boost::shared_ptr<T>& Registry<T>::getEditable(
			RegistryKeyType key
		) const {
			boost::recursive_mutex::scoped_lock lock(_mutex);
			typename Map::const_iterator it(_registry.find(key));

			if(it == _registry.end())
			{
				throw util::ObjectNotFoundInRegistryException<T>(key);
			}

			return it->second;
		}



		template<class T>
		boost::shared_ptr<const T> Registry<T>::get(
			RegistryKeyType key
		) const	{
			boost::recursive_mutex::scoped_lock lock(_mutex);
			typename Map::const_iterator it(_registry.find(key));

			if(it == _registry.end())
			{
				throw util::ObjectNotFoundInRegistryException<T>(key);
			}

			return boost::const_pointer_cast<const T, T>(it->second);
		}



		template<class T>
		void Registry<T>::add(
			const boost::shared_ptr<T>& ptr
		){
			boost::recursive_mutex::scoped_lock lock(_mutex);

			if (ptr->getKey() == 0)
			{
				throw RegistryKeyException<T>("Object with unknown key cannot be registered.", 0);
			}

			if (contains (ptr->getKey ()))
			{
				throw RegistryKeyException<T>("Duplicate key in registry", ptr->getKey ());
			}

			_registry.insert (std::make_pair (ptr->getKey (), ptr));
		}



		template<class T>
		void Registry<T>::replace(
			const boost::shared_ptr<T>& ptr
		){
			boost::recursive_mutex::scoped_lock lock(_mutex);
			remove (ptr->getKey ());
			_registry.insert (std::make_pair (ptr->getKey (), ptr));
		}



		template<class T>
		void Registry<T>::remove (RegistryKeyType key)
		{
			if (key == 0)
			{
				throw typename util::RegistryKeyException<T>("Neutral object cannot be removed at execution time", 0);
			}

			boost::recursive_mutex::scoped_lock lock(_mutex);
			if (contains (key) == false)
			{
				throw typename util::ObjectNotFoundInRegistryException<T>(key);
			}

			_registry.erase (key);
		}
}	}

#endif

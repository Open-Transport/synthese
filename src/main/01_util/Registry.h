
/** Registry class header.
	@file Registry.h

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

#ifndef SYNTHESE_UTIL_REGISTRY_H
#define SYNTHESE_UTIL_REGISTRY_H

#include <map>

#include <boost/shared_ptr.hpp>

#include "01_util/RegistryKeyException.h"
#include "01_util/Constants.h"

namespace synthese
{
	namespace util
	{

		class Exception;

		/** Generic registry class for common operations (get, add, remove...).
			Note that the registry has the responsability of destroying	registered objects due to the use of shared pointers.
			This class should not be used directly : use instead the "template" typedef in the classes derived from Registrable.

			@ingroup m01
		*/
		template<class K, class T>
		class Registry
		{
		 private:
		    
			 std::map<K, boost::shared_ptr<T> > _registry;

		 public:

			/** Constructor.
				@param createNullObject if true, an element indexed at 0 position is automatically created, and is considered as neutral element
			*/
			Registry ();
			~Registry ();

			typedef typename std::map<K, boost::shared_ptr<T> >::const_iterator const_iterator;

			//! @name Getters/Setters
			//@{
			//@}


			//! @name Query methods
			//@{
			bool contains (const K& key) const;
			boost::shared_ptr<const T> get (const K& key) const;
			boost::shared_ptr<T> getUpdateable (const K& key);
			
			size_t size () const;
			const_iterator begin() const;
			const_iterator end() const;

			//@}

		    
			//! @name Update methods
			//@{

			/** Removes and destroy all the registered objects.
			 */
			void clear ();

			void add (boost::shared_ptr<T> ptr);
			void replace (boost::shared_ptr<T> ptr);
			void remove (const K& key);
			//@}
		    

		 private:

			Registry ( const Registry& ref );
			Registry& operator= ( const Registry& rhs );

		};

		template<class K, class T>
		boost::shared_ptr<T> synthese::util::Registry<K, T>::getUpdateable( const K& key )
		{
			if (contains (key) == false) 
				throw RegistryKeyException<K,T> ("No such key in registry", key);

			return _registry.find (key)->second;
		}

		template<class K, class T>
		typename Registry<K,T>::const_iterator Registry<K,T>::begin() const
		{
			return _registry.begin();
		}

		template<class K, class T>
		typename Registry<K,T>::const_iterator Registry<K,T>::end() const
		{
			return _registry.end();
		}



		template<class K, class T>
		Registry<K,T>::Registry ()
		{
		}



		template<class K, class T>
		Registry<K,T>::~Registry ()
		{
		}



		template<class K, class T>
		bool 
		Registry<K,T>::contains (const K& key) const
		{
			return _registry.find (key) != _registry.end ();
		}



		template<class K, class T>
		boost::shared_ptr<const T> Registry<K,T>::get (const K& key) const
		{
			if (contains (key) == false) 
				throw RegistryKeyException<K,T> ("No such key in registry", key);

			return _registry.find (key)->second;
		}



		template<class K, class T>
		size_t 
		Registry<K,T>::size () const
		{
			return _registry.size ();
		}



		template<class K, class T>
		void 
		Registry<K,T>::clear ()
		{
			_registry.clear ();
		}



		template<class K, class T>
		void Registry<K,T>::add (boost::shared_ptr<T> ptr)
		{
			if (ptr->getKey() == UNKNOWN_VALUE)
				throw RegistryKeyException<K,T>("Object with unknown key cannot be registered.", UNKNOWN_VALUE);

			if (contains (ptr->getKey ())) 
				throw RegistryKeyException<K,T> ("Duplicate key in registry", ptr->getKey ());
		    
			_registry.insert (std::make_pair (ptr->getKey (), ptr));

			ptr->_registry = this;
		}


		template<class K, class T>
		void Registry<K,T>::replace (boost::shared_ptr<T> ptr)
		{
			remove (ptr->getKey ());
			_registry.insert (std::make_pair (ptr->getKey (), ptr));
		}



		template<class K, class T>
		void 
		Registry<K,T>::remove (const K& key)
		{
			if (key == 0)
				throw RegistryKeyException<K,T>("Neutral object cannot be removed at execution time", 0);

			if (contains (key) == false) 
				throw RegistryKeyException<K,T> ("No such key in registry", key);

			_registry.erase (key);
		}

	}
}

#endif

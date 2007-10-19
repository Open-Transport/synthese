
/** Factory class header.
	@file Factory.h

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

#ifndef SYNTHESE_util_Factory_h__
#define SYNTHESE_util_Factory_h__

#include <map>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "01_util/FactoryException.h"
#include "01_util/Log.h"



namespace synthese
{
	namespace util
	{
		/** Generic class factory.

			The features of the generic class factory are :
				- Auto instantiation of the factory at the fist class registration
				- Iterator for accessing to the registered subclasses list
				- Creation of objects from the iterator or directly from a string key

			Possible uses :
				- Factory<FactoryClass>::create("class_key") -> Creates an object from the string key of the class
				- Factory<FactoryClass>::create<Subclass>() -> equivalent as new Subclass but the created object knows the key of its class as if it comes from a from string creation.
				- object->getFactoryKey() -> Answers the key corresponding to the class of the object, if the object has been created by the factory
				- Factory<FactoryClass>::getKey<SubClass>() -> returns the key corresponding to the sub class
				- Factory<FactoryClass>::contains("class_key") -> Answers if a class is registered with the specified key

			@todo See if the getKey() method could be optimized by use of static variable (my first attempts have failed)
			@ingroup m01
		*/
		template <class RootObject>
		class Factory
		{
		public:
			class Iterator;

		private:

			/** Interface for auto-generated creators. */
			class CreatorInterface
			{
			private:
				virtual RootObject* create() = 0;
				virtual RootObject* create(const typename RootObject::Args& args) = 0;
				friend class Factory;
				friend class Factory::Iterator;
			};

			/** Auto-generated creator for each registered subclass. */
			template <class T>
			class Creator : public CreatorInterface
			{
			private:

				friend class Factory;


				RootObject* create ()
				{
					typename T::Args defaultArgs;
				    return create (defaultArgs);
				}


				RootObject* create (const typename T::Args& args)
				{
				    return static_cast<RootObject*>(createTyped(args));
				}
				

				T* createTyped (const typename T::Args& args)
				{
					T* obj(new T);
//					obj->setFactoryKey(getKey<T>());
					return obj;
				}
			};

			/** Registered subclasses map type. */
			typedef std::map<std::string, CreatorInterface*> Map;

			/** The registered subclasses map.
			*/
			static Map _registeredCreator;




		public:

			static size_t size()
			{
				return _registeredCreator.size();
			}

			/** Subclass registration.
				@param key Text key of the subclass
				@return the key if ok, empty string if the subclass is already registered
			*/
/*			template <class T>
				static void integrate(const typename Map::key_type& key)
			{
				Log::GetInstance ().debug ("Registering compound... " + key);

				// If the key is already used then return false (it would be better to use exceptions)
				if(_registeredCreator.find(key) != _registeredCreator.end())
					throw FactoryException<RootObject>("Attempted to integrate a class twice");

				// Saving of the auto generated builder
				CreatorInterface* creator = new Creator<T>;
				_registeredCreator.insert(std::pair<typename Map::key_type, 
							       CreatorInterface*>(key, creator));
			}
*/
			/** Subclass automatic registration.
				@return the key if ok, empty string if the subclass is already registered
			*/
			template <class T>
				static void integrate()
			{
				Log::GetInstance ().debug ("Registering compound... " + T::FACTORY_KEY);

				// If the key is already used then return false (it would be better to use exceptions)
				if(_registeredCreator.find(T::FACTORY_KEY) != _registeredCreator.end())
					throw FactoryException<RootObject>("Attempted to integrate a class twice");

				// Saving of the auto generated builder
				CreatorInterface* creator = new Creator<T>;
				_registeredCreator.insert(std::pair<typename Map::key_type, 
					CreatorInterface*>(T::FACTORY_KEY, creator));
			}

			template <class T>
				static typename Map::key_type getKey()
			{
				// If no registered classes
				if (size () == 0)
					throw FactoryException<RootObject>("Factorable class not found (empty factory)");

				// Search for a creator for the T class
				typename Map::const_iterator it;
				for (it = _registeredCreator.begin(); 
				     it != _registeredCreator.end(); ++it)
				{
				    if (dynamic_cast<Creator<T>*>(it->second) != NULL)
					return it->first;
				}

				// No such creator was founded
				throw FactoryException<RootObject>("Factorable class not found");

			}

			static bool contains( const typename Map::key_type& key )
			{
				// Search of the key of the wished class in the map
				typename Map::iterator it = _registeredCreator.find(key);

				// The key is not found
				return it != _registeredCreator.end();
			}


			template<class T>
			    static T* create()
			{
				typename T::Args defaultArgs;
				return create<T> (defaultArgs);
			}


			template<class T>
			    static T* create(const typename T::Args& args)
			{
				Creator<T> creator;
				return creator.createTyped(args);
			}


			template<class T>
			static boost::shared_ptr<T> createSharedPtr()
			{
				typename T::Args defaultArgs;
                return createSharedPtr<T> (defaultArgs);
			}


			template<class T>
			static boost::shared_ptr<T> createSharedPtr(const typename T::Args& args)
			{
				return boost::shared_ptr<T>(create<T>(args));
			}


			static RootObject* create(const typename Map::key_type& key)
			{
				typename RootObject::Args defaultArgs;
				return create (key, defaultArgs);
			}


			static RootObject* create(const typename Map::key_type& key, 
						  const typename RootObject::Args& args)
			{
				// The factory "single object" was never filled
				if (size () == 0)
					throw FactoryException<RootObject>("Unable to factor "+ key +" object (empty factory)");

				// Search of the key of the wished class in the map
				typename Map::iterator it = _registeredCreator.find(key);

				// The key is not found
				if(it == _registeredCreator.end())
				    throw FactoryException<RootObject>("Unable to factor "+ key +" object (class not found)");

				// The key is found : return of an instance of the object
				return it->second->create(args);
			}


			static boost::shared_ptr<RootObject> createSharedPtr(const typename Map::key_type& key)
			{
				typename RootObject::Args defaultArgs;
				return createSharedPtr (key, defaultArgs);
			}

			static boost::shared_ptr<RootObject> createSharedPtr(const typename Map::key_type& key,
									     const typename RootObject::Args& args)
			{
			    return boost::shared_ptr<RootObject>(create(key, args));
			}
			
			static void destroy()
			{
			    // MJ : never called; review memory management...
                            //delete _registeredCreator;
			}

			class Iterator
			{
			private:
				typename Factory::Map::const_iterator _it;
				
			public:
				Iterator(const typename Factory::Map::const_iterator& it)
					: _it(it)
				{	}

				/** Object creator.
					@return Shared Pointer to a temporary object of the current subclass, which will be deleted when unused.
				*/
				boost::shared_ptr<RootObject> operator*()
				{
					return boost::shared_ptr<RootObject>(_it->second->create());
				}

				boost::shared_ptr<RootObject> operator->()
				{
					return operator*();
				}

				/** Iteration.
				*/
				void operator++()
				{
					++_it;
				}

				bool operator==(const Iterator& obj)
				{
					return obj._it == _it;
				}

				bool operator!=(const Iterator& obj)
				{
					return obj._it != _it;
				}

				/** Key getter.
					@return The key of the current subclass.
				*/
				const typename Factory::Map::key_type& getKey()
				{
					return _it->first;
				}
			};

			/** First iterator on the subclasses.
			@return First iterator on the subclasses.
			*/
			static Iterator begin()
			{		
				return Iterator( _registeredCreator.begin() );
			}

			/** Last iterator on the subclasses.
			@return Last iterator on the subclasses.
			*/
			static Iterator end()
			{
				return Iterator( _registeredCreator.end() );
			}
		};

		template <class T>
			typename Factory<T>::Map Factory<T>::_registeredCreator;

	}
}

#endif // Factory_h__

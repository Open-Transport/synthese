////////////////////////////////////////////////////////////////////////////////
/// Factory class header.
///	@file Factory.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_util_Factory_h__
#define SYNTHESE_util_Factory_h__

#include "FactoryException.h"
#include "Log.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace util
	{
		/// @defgroup m01Factory Factory
		///	@ingroup m01
		//@{

		////////////////////////////////////////////////////////////////////////
		/// Generic class factory.
		///
		///	The features of the generic class factory are :
		///		- Auto instantiation of the factory at the fist class registration
		///		- Creation of objects from the iterator or directly from a string key
		///
		///	Possible uses :
		///		- Factory<FactoryClass>::create("class_key") -> Creates an object from the string key of the class
		///		- Factory<FactoryClass>::contains("class_key") -> Answers if a class is registered with the specified key
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
				friend class Factory;
				friend class Factory::Iterator;
			};

			/** Auto-generated creator for each registered subclass. */
			template <class T>
			class Creator : public CreatorInterface
			{
			private:

				friend class Factory;


				//////////////////////////////////////////////////////////////////////////
				/// Creation of a child object without arguments.
				/// @return a RootObject pointer to the created object.
				//////////////////////////////////////////////////////////////////////////
				RootObject* create ()
				{
					return static_cast<RootObject*>(new T);
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



			/** Tests if the factory contains a class registered with the specified key.
				@param key Key to search
				@return bool true if a class is already registered with the specified key
				@author Hugues Romain
			*/
			static bool contains( const typename Map::key_type& key )
			{
				// Search of the key of the wished class in the map
				typename Map::iterator it = _registeredCreator.find(key);

				// The key is not found
				return it != _registeredCreator.end();
			}



			/** Creation of an object from the key of its class, with arguments, returned as a pointer to an instantiation of the factory root class.
				@param key Key of the class to instantiate
				@param args Arguments to transmit to the constructor
				@return RootObject* Pointer to the instantiated object
				@author Hugues Romain
			*/
			static RootObject* create(const typename Map::key_type& key)
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
				return it->second->create();
			}


			
			static void destroy()
			{
			    // MJ : never called; review memory management...
                            //delete _registeredCreator;
			}

			
			
			////////////////////////////////////////////////////////////////////
			///	Builds a collection of new elements of each factorable subclass.
			///	@return a collection of new elements of each factorable subclass.
			///	@author Hugues Romain
			///	@date 2008
			static std::vector<boost::shared_ptr<RootObject> > GetNewCollection()
			{
				std::vector<boost::shared_ptr<RootObject> > result;
				for(typename Map::iterator it = _registeredCreator.begin(); it != _registeredCreator.end(); ++it)
				{
					result.push_back(boost::shared_ptr<RootObject>(it->second->create()));
				}
				return result;
			}
		};

		//@}

		template <class T>
			typename Factory<T>::Map Factory<T>::_registeredCreator;

	}
}

#endif // Factory_h__

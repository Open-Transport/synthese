//////////////////////////////////////////////////////////////////////////
/// Env class header.
///	@file Env.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_util_Env_h__
#define SYNTHESE_util_Env_h__

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#include "Registry.h"
#include "EnvException.h"
#include "Log.h"

namespace synthese
{
	namespace util
	{
		//////////////////////////////////////////////////////////////////////////
		///	Environment class.
		///
		///	An environment is a collection of registries (see Registry) that is able to
		/// store objects physical memory loaded object whole database compilation.
		///	It contains a registry per Registry template instantiation : some subclasses
		/// of the Registrable class.
		///
		///	The Environment class handles an <i>official</i> static instance that contains each
		///	object that must be loaded in the physical memory according to the table sync rules.
		///
		///	@ingroup m01Registry
		//////////////////////////////////////////////////////////////////////////
		class Env
		{
			friend class UtilModule;

		public:
			//////////////////////////////////////////////////////////////////////////
			///	Type of the map containing all data of the environment.
			//////////////////////////////////////////////////////////////////////////
			typedef std::map<std::string, boost::shared_ptr<RegistryBase> > RegistryMap;



		private:
			//////////////////////////////////////////////////////////////////////////
			///	Map containing all data of the environment.
			//////////////////////////////////////////////////////////////////////////
			mutable RegistryMap _map;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for auto-generated creators.
			///	@ingroup m01Registry
			//////////////////////////////////////////////////////////////////////////
			class RegistryCreatorInterface
			{
			private:
				virtual boost::shared_ptr<RegistryBase> create() const = 0;
				virtual size_t getObjectSize() const = 0;
				friend class Env;

			public:
				virtual ~RegistryCreatorInterface() {}
			};



			//////////////////////////////////////////////////////////////////////////
			///	Auto-generated creators.
			///	Implements the creator interface for each Registrable class.
			///	@ingroup m01Registry
			//////////////////////////////////////////////////////////////////////////
			template<class R>
			class RegistryCreator:
				public RegistryCreatorInterface
			{
			private:

				friend class Env;

				virtual boost::shared_ptr<RegistryBase> create() const
				{
					return boost::shared_ptr<RegistryBase>(new typename R::Registry);
				}

				virtual size_t getObjectSize() const
				{
					return sizeof(typename R::Registry::ObjectsClass);
				}

			public:
				virtual ~RegistryCreator() {}
			};

			typedef std::map<std::string, boost::shared_ptr<RegistryCreatorInterface> > RegistryCreatorMap;

			static RegistryCreatorMap	_registryCreators;
			static boost::shared_ptr<Env>	_officialRegistries;

		public:
			const RegistryMap& getMap() const { return _map; }



			//////////////////////////////////////////////////////////////////////////
			/// Returns the size of the objects stored in a given registry.
			/// The template version is not implemented because it is simpler to use
			/// directly sizeof(T)
			//////////////////////////////////////////////////////////////////////////
			/// @param registryKey the key of the registry
			/// @return the size of the objects stored in the specified registry
			/// @author Hugues Romain
			/// @version 3.4.1
			/// @date 2012
			static size_t GetObjectSize(
				const std::string& registryKey
			){
				RegistryCreatorMap::const_iterator itc(_registryCreators.find(registryKey));
				if (itc == _registryCreators.end())
				{
					throw util::EnvException(registryKey);
				}

				return itc->second->getObjectSize();
			}



			//////////////////////////////////////////////////////////////////////////
			/// Subclass automatic registration.
			///	@throw EnvException if a registry class is already integrated with the same key
			//////////////////////////////////////////////////////////////////////////
			// TODO: rename to Register for consistency?
			template <class R>
			static void Integrate()
			{
				Log::GetInstance().debug("Registering registry... " + Registry<R>::KEY);

				if(_registryCreators.find(Registry<R>::KEY) != _registryCreators.end())
					throw util::EnvException("Attempted to integrate a registry class twice " + Registry<R>::KEY);

				// Saving of the auto generated builder
				boost::shared_ptr<RegistryCreatorInterface> creator(new RegistryCreator<R>);
				_registryCreators.insert(make_pair(Registry<R>::KEY, creator));
			}



			//////////////////////////////////////////////////////////////////////////
			/// Subclass automatic registration.
			///	@throw EnvException if no registry class is already integrated with the given key
			//////////////////////////////////////////////////////////////////////////
			template <class R>
			static void Unregister()
			{
				Log::GetInstance().debug("Unregistering registry... " + Registry<R>::KEY);

				if(_registryCreators.find(Registry<R>::KEY) == _registryCreators.end())
					throw util::EnvException("Attempted to unregister a registry class not registered " + Registry<R>::KEY);

				_registryCreators.erase(Registry<R>::KEY);
			}



			//////////////////////////////////////////////////////////////////////////
			/// Read only registry getter.
			/// The template parameter class must be a subclass of Registrable and must
			///	have been integrated at the launch of the program (run Integrate method
			/// in *.gen.cpp files)
			///	@throw EnvException if no registry corresponds to the template parameter
			///	class.
			//////////////////////////////////////////////////////////////////////////
			template<class R>
			const Registry<R>& getRegistry() const
			{
				return const_cast<const Registry<R>& >(this->getEditableRegistry<R>());
			}



			//////////////////////////////////////////////////////////////////////////
			/// Read-write registry getter.
			/// The template parameter class must be a subclass of Registrable and must
			///	have been integrated at the launch of the program (run Integrate method
			/// in *.gen.cpp files)
			///	@throw EnvException if no registry corresponds to the template parameter
			///	class.
			//////////////////////////////////////////////////////////////////////////
			template<class R>
			Registry<R>& getEditableRegistry() const
			{
				RegistryMap::const_iterator it(_map.find(Registry<R>::KEY));
				if (it != _map.end())
				{
					return * boost::static_pointer_cast<Registry<R>, RegistryBase>(it->second);
				}

				RegistryCreatorMap::const_iterator itc(_registryCreators.find(Registry<R>::KEY));
				if (itc == _registryCreators.end())
				{
					throw util::EnvException(Registry<R>::KEY);
				}

				_map.insert(make_pair(Registry<R>::KEY, itc->second->create()));

				it = _map.find(Registry<R>::KEY);
				return * boost::static_pointer_cast<Registry<R>, RegistryBase>(it->second);
			}



			template<class R>
			void add(
				boost::shared_ptr<R> object
			);



			void addRegistrable(boost::shared_ptr<util::Registrable> object);



			template<class R>
			boost::shared_ptr<const R> get(
				util::RegistryKeyType id
			) const {
				return this->getEditableRegistry<R>().get(id);
			}



			template<class R>
			const boost::shared_ptr<R>& getEditable(
				util::RegistryKeyType id
			) const {
				return this->getEditableRegistry<R>().getEditable(id);
			}



			template<class C, class R>
			const boost::shared_ptr<C> getCastEditable(
				util::RegistryKeyType id
			) const {
				try
				{
					boost::shared_ptr<C> c(
						boost::dynamic_pointer_cast<C, R>(
							this->getEditableRegistry<R>().getEditable(id)
					)	);
					if(!c.get())
					{
						throw ObjectNotFoundException<C>(id, "Bad type");
					}
					return c;
				}
				catch(ObjectNotFoundException<R>& e)
				{
					throw ObjectNotFoundException<C>(id, e.getMessage());
				}
			}



			template<class C, class R>
			boost::shared_ptr<const C> getCast(
				util::RegistryKeyType id
			) const {
				return boost::const_pointer_cast<const C>(getCastEditable<C, R>(id));
			}




			template<class R>
			bool contains(
				const R& object
			) const {
				return
					this->getRegistry<R>().contains(object.getKey()) &&
					&object == this->getEditableRegistry<R>().get(object.getKey()).get()
				;
			}



			template<class R>
			boost::shared_ptr<const R> getSPtr(
				const R* object
			) const {
				if(object == NULL)
				{
					return boost::shared_ptr<const R>();
				}
				assert(this->contains(*object));
				return this->getEditableRegistry<R>().get(object->getKey());
			}



			template<class R>
			boost::shared_ptr<R> getEditableSPtr(
				R* object
			) const {
				if(object == NULL)
				{
					return boost::shared_ptr<R>();
				}
				assert(this->contains(*object));
				return this->getEditableRegistry<R>().getEditable(object->getKey());
			}



			//////////////////////////////////////////////////////////////////////////
			/// Clears the environment : each registry is destroyed.
			/// Thanks to the shared pointer, this method should destroy linked objects
			/// too, except if they are linked by extern shared pointer instances.
			void clear()
			{
				_map.clear();
			}



			//////////////////////////////////////////////////////////////////////////
			/// Official environment static getter.
			/// @return Pointer to the main environment containing the physical memory loaded
			/// of each table that use this feature (see DBDirectTableSyncTemplate).
			//////////////////////////////////////////////////////////////////////////
			static Env& GetOfficialEnv() { return *_officialRegistries; }



			//////////////////////////////////////////////////////////////////////////
			/// Official environment static getter.
			/// @return Pointer to the main environment containing the physical memory loaded
			/// of each table that use this feature (see DBDirectTableSyncTemplate).
			//////////////////////////////////////////////////////////////////////////
			static const boost::shared_ptr<Env>& GetOfficialEnvSPtr() { return _officialRegistries; }
		};



		template<class R>
		void Env::add(
			boost::shared_ptr<R> object
		){
			if(!object.get())
			{
				return;
			}
			this->getEditableRegistry<R>().add(object);
		}
}	}

#endif // SYNTHESE_util_Env_h__

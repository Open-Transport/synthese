
/// Env class header.
///	@file Env.h
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

		private:
			//////////////////////////////////////////////////////////////////////////
			///	Type of the map containing all data of the environment.
			//////////////////////////////////////////////////////////////////////////
			typedef std::map<std::string, boost::shared_ptr<RegistryBase> > RegistryMap;



			//////////////////////////////////////////////////////////////////////////
			///	Map containing all data of the environment.
			//////////////////////////////////////////////////////////////////////////
			RegistryMap _map;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for auto-generated creators.
			///	@ingroup m01Registry
			//////////////////////////////////////////////////////////////////////////
			class RegistryCreatorInterface
			{
			private:
				virtual boost::shared_ptr<RegistryBase> create() = 0;
				friend class Env;
			};



			//////////////////////////////////////////////////////////////////////////
			///	Auto-generated creators.
			///	Implements the creator interface for each Registrable class.
			///	@ingroup m01Registry
			//////////////////////////////////////////////////////////////////////////
			template<class R>
			class RegistryCreator : public RegistryCreatorInterface
			{
			private:

				friend class Env;

				boost::shared_ptr<RegistryBase> create ()
				{
					return boost::shared_ptr<RegistryBase>(new typename R::Registry);
				}
			};

			typedef std::map<std::string, RegistryCreatorInterface*> RegistryCreatorMap;

			static RegistryCreatorMap		_registryCreators;
			static boost::shared_ptr<Env>	_officialRegistries;

		public:


			//////////////////////////////////////////////////////////////////////////
			///	Constructor.
			///	Builds a map containing an empty registry by integrated Registrable class
			//////////////////////////////////////////////////////////////////////////
			Env();



			//////////////////////////////////////////////////////////////////////////
			///	Destructor.
			/// No operation.
			//////////////////////////////////////////////////////////////////////////
			~Env();



			//////////////////////////////////////////////////////////////////////////
			/// Subclass automatic registration.
			///	@throw EnvException if a registry class is already integrated with the same key
			//////////////////////////////////////////////////////////////////////////
			template <class R>
			static void Integrate()
			{
				Log::GetInstance ().debug ("Registering registry... " + Registry<R>::KEY);

				// If the key is already used then throw exception
				if(_registryCreators.find(Registry<R>::KEY) != _registryCreators.end())
					throw util::EnvException(Registry<R>::KEY);

				// Saving of the auto generated builder
				RegistryCreatorInterface* creator = new RegistryCreator<R>;
				_registryCreators.insert(make_pair(Registry<R>::KEY, creator));
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
				RegistryMap::const_iterator it(_map.find(Registry<R>::KEY));
				if (it == _map.end())
				{
					throw util::EnvException(Registry<R>::KEY);
				}
				return * boost::static_pointer_cast<const Registry<R>, RegistryBase>(it->second);
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
			Registry<R>& getEditableRegistry()
			{
				RegistryMap::iterator it(_map.find(Registry<R>::KEY));
				if (it == _map.end())
				{
					throw util::EnvException(Registry<R>::KEY);
				}
				return * boost::static_pointer_cast<Registry<R>, RegistryBase>(it->second);
			}



			//////////////////////////////////////////////////////////////////////////
			/// Official environment static getter.
			/// @return Pointer to the main environment containing the physical memory loaded
			/// of each table that use this feature (see SQLiteDirectTableSyncTemplate).
			//////////////////////////////////////////////////////////////////////////
			static Env* GetOfficialEnv();
		};
	}
}

#endif // SYNTHESE_util_Env_h__

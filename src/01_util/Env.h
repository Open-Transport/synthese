
/** Env class header.
	@file Env.h

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

#ifndef SYNTHESE_util_Env_h__
#define SYNTHESE_util_Env_h__

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

#include "Registry.h"

#include "01_util/Log.h"

namespace synthese
{
	namespace util
	{
		/** Environment class.

			An environment is a collection of registries (see Registry).
			It contains a registry per Registry template instantiation.

			The Environment class handles an <i>official</i> static instance that contains each
			object that must be loaded in the physical memory according to the table sync rules.

			@ingroup m01Registry
		*/
		class Env
		{
			friend class UtilModule;

		private:
			typedef std::map<std::string, boost::shared_ptr<RegistryBase> > RegistryMap;

			RegistryMap _map;

			/** Interface for auto-generated creators. */
			class RegistryCreatorInterface
			{
			private:
				virtual boost::shared_ptr<RegistryBase> create() = 0;
				friend class Env;
			};

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

			Env();
			~Env();

			/** Subclass automatic registration.
			@return the key if ok, empty string if the subclass is already registered
			*/
			template <class R>
			static void Integrate()
			{
				Log::GetInstance ().debug ("Registering registry... " + Registry<R>::KEY);

				// If the key is already used then return false (it would be better to use exceptions)
				//if(_registeredCreator.find(T::FACTORY_KEY) != _registeredCreator.end())
				//	throw FactoryException<RootObject>("Attempted to integrate a class twice");

				// Saving of the auto generated builder
				RegistryCreatorInterface* creator = new RegistryCreator<R>;
				_registryCreators.insert(make_pair(Registry<R>::KEY, creator));
			}

			template<class R>
			const Registry<R>& getRegistry() const
			{
				RegistryMap::const_iterator it(_map.find(Registry<R>::KEY));
				return * boost::static_pointer_cast<const Registry<R>, RegistryBase>(it->second);
			}

			template<class R>
			Registry<R>& getEditableRegistry()
			{
				RegistryMap::iterator it(_map.find(Registry<R>::KEY));
				return * boost::static_pointer_cast<Registry<R>, RegistryBase>(it->second);
			}

			static Env* GetOfficialEnv();
		};
	}
}

#endif // SYNTHESE_util_Env_h__

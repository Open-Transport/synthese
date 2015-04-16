
/** SecurityRight class implementation.
	@file SecurityRight.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "SecurityRight.h"
#include "SecurityModule.h"
#include "SecurityConstants.hpp"
#include "Profile.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, SecurityRight>::FACTORY_KEY("Security");
	}

	namespace security
	{
		template<> const string RightTemplate<SecurityRight>::NAME("Gestion de la sécurité");
		template<> const bool RightTemplate<SecurityRight>::USE_PRIVATE_RIGHTS(true);

		template<> ParameterLabelsVector RightTemplate<SecurityRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(tous profils)"));
			SecurityRight::addSubProfilesLabel(m, boost::shared_ptr<Profile>(), string());
			return m;
		}


		std::string SecurityRight::displayParameter(
			util::Env& env
		) const	{
			if (_parameter == GLOBAL_PERIMETER)
				return "(tous profils)";

			return env.getRegistry<Profile>().contains(lexical_cast<RegistryKeyType>(_parameter)) ?
				env.getRegistry<Profile>().get(lexical_cast<RegistryKeyType>(_parameter))->getName() :
				"(invalide)";
		}



		bool SecurityRight::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			if (_parameter == GLOBAL_PERIMETER)
				return true;
			if (perimeter.empty())
				return true;

			const Registry<Profile>& registry(env.getRegistry<Profile>());

			RegistryKeyType parameterKey = -1, perimeterKey = -1;
			try
			{
				parameterKey = lexical_cast<RegistryKeyType>(_parameter);
			}
			catch (bad_lexical_cast)
			{
			}
			try
			{
				perimeterKey = lexical_cast<RegistryKeyType>(perimeter);
			}
			catch (bad_lexical_cast)
			{
			}

			if (registry.contains(parameterKey) && registry.contains(perimeterKey)
			){
				const Profile* currentProfile(registry.get(parameterKey).get());
				for(const Profile* includedProfile(registry.get(perimeterKey).get());
					includedProfile;
					includedProfile = includedProfile->getParent()
				)
				{
					if (currentProfile == includedProfile)
					{
						return true;
					}
				}
			}

			return false;
		}

		void SecurityRight::addSubProfilesLabel(
			ParameterLabelsVector& plv,
			boost::shared_ptr<Profile> parent,
			std::string prefix
		){
			BOOST_FOREACH(const boost::shared_ptr<Profile>& profile, SecurityModule::getSubProfiles(parent))
			{
				plv.push_back(make_pair(lexical_cast<string>(profile->getKey()), prefix + profile->getName()));

				addSubProfilesLabel(plv, profile, "&nbsp;&nbsp;" + prefix);
			}
		}
	}
}

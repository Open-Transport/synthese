
/** SecurityRight class implementation.
	@file SecurityRight.cpp

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

#include "01_util/Conversion.h"

#include "12_security/SecurityRight.h"
#include "12_security/SecurityModule.h"
#include "12_security/Constants.h"
#include "12_security/Profile.h"

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

		template<> Right::ParameterLabelsVector RightTemplate<SecurityRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(tous profils)"));
			SecurityRight::addSubProfilesLabel(m, shared_ptr<const Profile>(), string());
			return m;
		}


		std::string SecurityRight::displayParameter() const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "(tous profils)";

			return (SecurityModule::getProfiles().contains(Conversion::ToLongLong(_parameter)))
				? SecurityModule::getProfiles().get(Conversion::ToLongLong(_parameter))->getName()
				: "(invalide)";
		}

		bool SecurityRight::perimeterIncludes( const std::string& perimeter ) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;
			if (perimeter == UNKNOWN_PERIMETER)
				return true;

			if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(_parameter))
				&& SecurityModule::getProfiles().contains(Conversion::ToLongLong(perimeter)))
			{
				shared_ptr<const Profile> includedProfile = SecurityModule::getProfiles().get(Conversion::ToLongLong(perimeter));
				shared_ptr<const Profile> currentProfile = SecurityModule::getProfiles().get(Conversion::ToLongLong(_parameter));

				for (;includedProfile.get(); includedProfile = SecurityModule::getProfiles().get(includedProfile->getParentId()))
					if (currentProfile == includedProfile)
						return true;
			}

			return false;
		}

		void SecurityRight::addSubProfilesLabel( ParameterLabelsVector& plv, shared_ptr<const Profile> parent, std::string prefix)
		{
			vector<shared_ptr<const Profile> > p = SecurityModule::getSubProfiles(parent);

			for (vector<shared_ptr<const Profile> >::const_iterator it = p.begin(); it != p.end(); ++it)
			{
				plv.push_back(make_pair(Conversion::ToString((*it)->getKey()), prefix + (*it)->getName()));

				addSubProfilesLabel(plv, *it, "&nbsp;&nbsp;" + prefix);
			}
		}
	}
}

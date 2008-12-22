
/** MessagesRight class implementation.
	@file MessagesRight.cpp

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

#include "12_security/Constants.h"

#include "MessagesRight.h"
#include "AlarmRecipient.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace messages;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, MessagesRight>::FACTORY_KEY("Messages");
	}

	namespace security
	{
		template<> const string RightTemplate<MessagesRight>::NAME("Gestion des messages");
		template<> const bool RightTemplate<MessagesRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<MessagesRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les messages)"));

			for (Factory<AlarmRecipient>::Iterator it = Factory<AlarmRecipient>::begin(); it != Factory<AlarmRecipient>::end(); ++it)
			{
				m.push_back(make_pair(UNKNOWN_PERIMETER, "=== " + it->getTitle() + " ==="));
				it->getStaticParametersLabelsVirtual(m);
			}

			return m;
		}
	}

	namespace messages
	{
		std::string MessagesRight::displayParameter(
			util::Env& env
		) const	{
			return _parameter;
		}

		bool MessagesRight::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			return true;
		}
	}
}


/** MessagesLibraryRight class implementation.
	@file MessagesLibraryRight.cpp

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

#include "SecurityConstants.hpp"

#include "17_messages/MessagesLibraryRight.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace messages;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, MessagesLibraryRight>::FACTORY_KEY("MessagesLibrary");
	}

	namespace security
	{
		template<> const string RightTemplate<MessagesLibraryRight>::NAME("Bilbliothèque de messages");
		template<> const bool RightTemplate<MessagesLibraryRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<MessagesLibraryRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(tous les modèles)"));
			return m;
		}
	}

	namespace messages
	{
		std::string MessagesLibraryRight::displayParameter(
			util::Env& env
		) const	{
			return _parameter;
		}

		bool MessagesLibraryRight::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			return true;
		}
	}
}

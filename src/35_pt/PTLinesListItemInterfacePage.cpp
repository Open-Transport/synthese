
/** PTLinesListItemInterfacePage class implementation.
	@file PTLinesListItemInterfacePage.cpp
	@author Hugues Romain
	@date 2010

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

#include "PTLinesListItemInterfacePage.hpp"
#include "CommercialLine.h"
#include "TransportNetwork.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, pt::PTLinesListItemInterfacePage>::FACTORY_KEY("lines_list_item");
	}

	namespace pt
	{
		PTLinesListItemInterfacePage::PTLinesListItemInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, PTLinesListItemInterfacePage>(),
			Registrable(0)
		{
		}
		
		

		void PTLinesListItemInterfacePage::display(
			std::ostream& stream,
			const CommercialLine& object,
			size_t rank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(lexical_cast<string>(object.getKey()));//0
			pv.push_back(object.getName()); //1
			pv.push_back(object.getShortName()); //2
			pv.push_back(object.getStyle()); //3
			pv.push_back(object.getColor() ? object.getColor()->toString() : string()); //4
			pv.push_back(lexical_cast<string>(rank)); //5
			pv.push_back(lexical_cast<string>(rank % 2)); //6
			pv.push_back(lexical_cast<string>(object.getNetwork()->getKey()));//7
			pv.push_back(object.getNetwork()->getName());//8

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}
	}
}

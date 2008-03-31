
/** JourneyBoardsInterfaceElement class implementation.
	@file JourneyBoardsInterfaceElement.cpp

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

#include "JourneyBoardsInterfaceElement.h"

#include "33_route_planner/JourneyBoardInterfacePage.h"
#include "33_route_planner/Types.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include <boost/logic/tribool.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::JourneyBoardsInterfaceElement>::FACTORY_KEY("journey_boards");

	namespace routeplanner
	{
		void JourneyBoardsInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 2)
				throw InterfacePageException("Insufficient parameters number");
			_handicappedFilter = vel.front();
			_bikeFilter = vel.front();
			if (vel.isEmpty())
				return;
			_pageCode = vel.front();
		}

		string JourneyBoardsInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			const JourneyBoardJourneys* jv(static_cast<const JourneyBoardJourneys*>(object));
			const JourneyBoardInterfacePage* page(_page->getInterface()->getPage<JourneyBoardInterfacePage>(_pageCode ? _pageCode->getValue(parameters, variables, object, request) : string()));
			logic::tribool hFilter(Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request)));
			logic::tribool bFilter(Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request)));
			

			if ( jv == NULL || jv->empty())  // No solution or type error
				return string();

			int i=1;
			for(JourneyBoardJourneys::const_iterator it(jv->begin());
				it != jv->end();
				++it, ++i
			){
				page->display(stream, variables, i, *it, hFilter, bFilter, request);
			}
			return string();
		}

		JourneyBoardsInterfaceElement::~JourneyBoardsInterfaceElement()
		{
		}
	}
}

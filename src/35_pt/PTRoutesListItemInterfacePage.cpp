
/** PTRoutesListItemInterfacePage class implementation.
	@file PTRoutesListItemInterfacePage.cpp
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

#include "PTRoutesListItemInterfacePage.hpp"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, pt::PTRoutesListItemInterfacePage>::FACTORY_KEY("ptroutes_list_item");
	}

	namespace pt
	{
		PTRoutesListItemInterfacePage::PTRoutesListItemInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, PTRoutesListItemInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void PTRoutesListItemInterfacePage::display(
			std::ostream& stream,
			const Line& object,
			size_t rank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(lexical_cast<string>(object.getKey())); //0
			pv.push_back(object.getName()); //1
			pv.push_back(lexical_cast<string>(object.getLastEdge() ?  object.getLastEdge()->getMetricOffset() : double(0))); //2
			pv.push_back(lexical_cast<string>(object.getEdges().size())); //3
			pv.push_back(object.getDirection()); //4
			pv.push_back(dynamic_cast<const LineStop*>(object.getEdge(0)) ? dynamic_cast<const LineStop*>(object.getEdge(0))->getPhysicalStop()->getConnectionPlace()->getCity()->getName() : string()); //5
			pv.push_back(dynamic_cast<const LineStop*>(object.getEdge(0)) ? dynamic_cast<const LineStop*>(object.getEdge(0))->getPhysicalStop()->getConnectionPlace()->getName() : string()); //6
			pv.push_back(dynamic_cast<const LineStop*>(object.getLastEdge()) ? dynamic_cast<const LineStop*>(object.getLastEdge())->getPhysicalStop()->getConnectionPlace()->getCity()->getName() : string()); //7
			pv.push_back(dynamic_cast<const LineStop*>(object.getLastEdge()) ? dynamic_cast<const LineStop*>(object.getLastEdge())->getPhysicalStop()->getConnectionPlace()->getName() : string()); //8
			pv.push_back(lexical_cast<string>(rank));
			pv.push_back(lexical_cast<string>(rank % 2));

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

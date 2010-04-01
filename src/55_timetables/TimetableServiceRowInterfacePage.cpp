
/** TimetableServiceRowInterfacePage class implementation.
	@file TimetableServiceRowInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "TimetableServiceRowInterfacePage.h"
#include "TimetableColumn.h"
#include "PhysicalStop.h"
#include "Line.h"
#include "CommercialLine.h"
#include "RollingStock.h"
#include "TimetableServiceRowCellInterfacePage.h"
#include "Interface.h"
#include "InterfacePageException.h"
#include "TimetableWarning.h"
#include "Service.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"

#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace timetables;
	using namespace graph;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableServiceRowInterfacePage>::FACTORY_KEY("timetable_service_row");
	}

	namespace timetables
	{
		TimetableServiceRowInterfacePage::TimetableServiceRowInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableServiceRowInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableServiceRowInterfacePage::display(
			std::ostream& stream,
			const TimetableColumn& object,
			const time_duration& lastSchedule,
			size_t rank,
			std::size_t followingServicesWithSameHour,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
		
			pv.push_back(lexical_cast<string>(Service::GetTimeOfDay(object.getContent().begin()->second).hours())); //0
			pv.push_back(lexical_cast<string>(Service::GetTimeOfDay(object.getContent().begin()->second).minutes())); //1
			pv.push_back(lexical_cast<string>(Service::GetTimeOfDay(lastSchedule).hours())); //2
			pv.push_back(object.getWarning() ? lexical_cast<string>(object.getWarning()->getNumber()) : string()); //3
			pv.push_back(object.getWarning() ? object.getWarning()->getText() : string()); //4

			// 5 : Cells content
			stringstream content;
			try
			{
				const TimetableServiceRowCellInterfacePage* cellPage(getInterface()->getPage<TimetableServiceRowCellInterfacePage>());
				size_t rank(0);
				BOOST_FOREACH(const TimetableColumn::Content::value_type& cell, object.getContent())
				{
					cellPage->display(content, cell, object.getLine()->getRollingStock(), rank++, variables, request);
				}
			}
			catch(InterfacePageException)
			{
			}
			pv.push_back(content.str());

			pv.push_back(object.getContent().begin()->first ? object.getContent().begin()->first->getName() : string()); //6
			pv.push_back(object.getLine()->getCommercialLine()->getShortName()); //7
			pv.push_back(object.getLine()->getCommercialLine()->getStyle()); //8
			pv.push_back(object.getLine()->getCommercialLine()->getImage()); //9
			pv.push_back(object.getLine()->getRollingStock() ? lexical_cast<string>(object.getLine()->getRollingStock()->getKey()) : string()); //10
			pv.push_back(lexical_cast<string>(rank));
			pv.push_back(lexical_cast<string>(followingServicesWithSameHour)); //12

			const PublicTransportStopZoneConnectionPlace* lastPlace(object.getLine()->getDestination()->getConnectionPlace());
			pv.push_back(lexical_cast<string>(lastPlace->getCity()->getKey())); //13
			pv.push_back(lastPlace->getCity()->getName()); //14
			pv.push_back(lexical_cast<string>(lastPlace->getKey())); //15
			pv.push_back(lastPlace->getName()); //16

			pv.push_back(object.getLine()->getDirection()); //17

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

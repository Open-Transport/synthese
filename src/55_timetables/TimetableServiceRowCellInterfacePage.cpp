
/** TimetableServiceRowCellInterfacePage class implementation.
	@file TimetableServiceRowCellInterfacePage.cpp
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

#include "TimetableServiceRowCellInterfacePage.h"
#include "Schedule.h"
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
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableServiceRowCellInterfacePage>::FACTORY_KEY("timetable_service_row_cell");
	}

	namespace timetables
	{
		TimetableServiceRowCellInterfacePage::TimetableServiceRowCellInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableServiceRowCellInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableServiceRowCellInterfacePage::display(
			std::ostream& stream,
			const TimetableColumn::Content::value_type& object,
			size_t rank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			if(object.first)
			{
				pv.push_back(lexical_cast<string>(object.second.getHour().getHours())); //0
				pv.push_back(lexical_cast<string>(object.second.getHour().getMinutes())); //1
				pv.push_back(object.first->getConnectionPlace()->getName()); //2
				pv.push_back(object.first->getConnectionPlace()->getCity()->getName()); //3
				pv.push_back(object.first->getConnectionPlace()->getName26()); //4
			}
			else
			{
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
				pv.push_back(string());
			}
			pv.push_back(lexical_cast<string>(rank)); //5

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

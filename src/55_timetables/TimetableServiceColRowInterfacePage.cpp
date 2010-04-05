
/** TimetableServiceColRowInterfacePage class implementation.
	@file TimetableServiceColRowInterfacePage.cpp
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

#include "TimetableServiceColRowInterfacePage.hpp"
#include "TimetableServiceColCellInterfacePage.hpp"
#include "TimetableRow.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "TimetableResult.hpp"
#include "Interface.h"
#include "CommercialLine.h"

#include <boost/date_time/time_duration.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace timetables;
	using namespace pt;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableServiceColRowInterfacePage>::FACTORY_KEY("timetable_row_lines");
	}

	namespace timetables
	{
		TimetableServiceColRowInterfacePage::TimetableServiceColRowInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableServiceColRowInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableServiceColRowInterfacePage::display(
			std::ostream& stream,
			const TimetableResult::RowLinesVector& lines,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(TimetableServiceColCellInterfacePage::TYPE_LINE); //0
		
			const TimetableServiceColCellInterfacePage* page(getInterface()->getPage<TimetableServiceColCellInterfacePage>());
			stringstream content;
			size_t colRank(0);
			BOOST_FOREACH(const CommercialLine* line, lines)
			{
				page->display(content, *line, 0, colRank++, variables, request);
			}
			pv.push_back(content.str()); //1

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}



		void TimetableServiceColRowInterfacePage::display(
			std::ostream& stream,
			const TimetableRow& place,
			const TimetableResult::RowTimesVector& times,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(TimetableServiceColCellInterfacePage::TYPE_TIME); //0
		
			const TimetableServiceColCellInterfacePage* page(getInterface()->getPage<TimetableServiceColCellInterfacePage>());
			stringstream content;
			size_t colRank(0);
			BOOST_FOREACH(time_duration duration, times)
			{
				page->display(content, duration, place.getRank(), colRank++, variables, request);
			}
			pv.push_back(content.str()); //1

			pv.push_back(lexical_cast<string>(place.getRank())); //2

			pv.push_back(lexical_cast<string>(place.getRank() % 2)); //3

			pv.push_back(lexical_cast<string>(place.getPlace()->getCity()->getKey())); //4
			pv.push_back(lexical_cast<string>(place.getPlace()->getKey())); //5
			pv.push_back(place.getPlace()->getCity()->getName()); //6
			pv.push_back(place.getPlace()->getName()); //7

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}



		void TimetableServiceColRowInterfacePage::display(
			std::ostream& stream,
			const TimetableResult::RowNotesVector& notes,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(TimetableServiceColCellInterfacePage::TYPE_NOTE); //0
		
			const TimetableServiceColCellInterfacePage* page(getInterface()->getPage<TimetableServiceColCellInterfacePage>());
			stringstream content;
			size_t colRank(0);
			BOOST_FOREACH(const TimetableWarning* note, notes)
			{
				page->display(content, note, 0, colRank++, variables, request);
			}
			pv.push_back(content.str()); //1

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}



		void TimetableServiceColRowInterfacePage::display(
			std::ostream& stream,
			const TimetableResult::RowRollingStockVector& rollingStock,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(TimetableServiceColCellInterfacePage::TYPE_ROLLING_STOCK); //0

			const TimetableServiceColCellInterfacePage* page(getInterface()->getPage<TimetableServiceColCellInterfacePage>());
			stringstream content;
			size_t colRank(0);
			BOOST_FOREACH(const RollingStock* object, rollingStock)
			{
				page->display(content, object, 0, colRank++, variables, request);
			}
			pv.push_back(content.str()); //1

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}
	}
}

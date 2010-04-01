
/** TimetableServiceColCellInterfacePage class implementation.
	@file TimetableServiceColCellInterfacePage.cpp
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

#include "TimetableServiceColCellInterfacePage.hpp"
#include "CommercialLine.h"
#include "RollingStock.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "TimetableWarning.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableServiceColCellInterfacePage>::FACTORY_KEY("timetable_service_col_cell");
	}

	namespace timetables
	{
		const std::string TimetableServiceColCellInterfacePage::TYPE_LINE("line");
		const std::string TimetableServiceColCellInterfacePage::TYPE_TIME("time");
		const std::string TimetableServiceColCellInterfacePage::TYPE_NOTE("note");
		const std::string TimetableServiceColCellInterfacePage::TYPE_BOOKING("booking");
		const std::string TimetableServiceColCellInterfacePage::TYPE_ROLLING_STOCK("rollingstock");



		TimetableServiceColCellInterfacePage::TimetableServiceColCellInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableServiceColCellInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void TimetableServiceColCellInterfacePage::display(
			std::ostream& stream,
			const CommercialLine& object,
			size_t rowRank,
			size_t colRank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(TYPE_LINE); //0
			pv.push_back(lexical_cast<string>(colRank)); //1
			pv.push_back(lexical_cast<string>(rowRank)); //2
			pv.push_back(lexical_cast<string>(object.getKey())); //3
			pv.push_back(object.getName()); //4
			pv.push_back(object.getColor() ? object.getColor()->toString() : string()); //5
			pv.push_back(object.getStyle()); //6
			pv.push_back(object.getImage()); //7

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}




		void TimetableServiceColCellInterfacePage::display(
			std::ostream& stream,
			boost::posix_time::time_duration object,
			std::size_t rowRank,
			std::size_t colRank,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const {
			ParametersVector pv;
			pv.push_back(TYPE_TIME); //0
			pv.push_back(lexical_cast<string>(colRank)); //1
			pv.push_back(lexical_cast<string>(rowRank)); //2
			pv.push_back(object.is_not_a_date_time() ? string() : lexical_cast<string>(object.hours())); //3
			pv.push_back(object.is_not_a_date_time() ? string() : lexical_cast<string>(object.minutes())); //4

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}



		void TimetableServiceColCellInterfacePage::display(
			std::ostream& stream,
			const TimetableWarning* object,
			std::size_t rowRank,
			std::size_t colRank,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const {
			ParametersVector pv;
			pv.push_back(TYPE_NOTE); //0
			pv.push_back(lexical_cast<string>(colRank)); //1
			pv.push_back(lexical_cast<string>(rowRank)); //2
			pv.push_back(object ? lexical_cast<string>(object->getNumber()) : string()); //3
			pv.push_back(object ? lexical_cast<string>(object->getText()) : string()); //4

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}



		void TimetableServiceColCellInterfacePage::display(
			std::ostream& stream,
			const env::RollingStock* object,
			std::size_t rowRank,
			std::size_t colRank,
			interfaces::VariablesMap& variables,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(TYPE_ROLLING_STOCK); //0
			pv.push_back(lexical_cast<string>(colRank)); //1
			pv.push_back(lexical_cast<string>(rowRank)); //2
			pv.push_back(object ? lexical_cast<string>(object->getKey()) : string()); //3
			pv.push_back(object ? object->getIndicator() : string()); //4
			pv.push_back(object ? object->getName() : string()); //5

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
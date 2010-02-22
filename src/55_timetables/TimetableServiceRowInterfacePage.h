
/** TimetableServiceRowInterfacePage class header.
	@file TimetableServiceRowInterfacePage.h
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

#ifndef SYNTHESE_TimetableServiceRowInterfacePage_H__
#define SYNTHESE_TimetableServiceRowInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace timetables
	{
		class TimetableColumn;

		/** TimetableServiceRowInterfacePage Interface Page Class.
			@ingroup m55Pages refPages
			@author Hugues
			@date 2009

			@code timetable_service_row @endcode

			Parameters :
				- 0 : Departure hour
				- 1 : Departure minutes
				- 2 : Last departure hour
				- 3 : Note number
				- 4 : Note text
				- 5 : Cells content
				- 6 : Departure quay
				- 7 : Line short name
				- 8 : Line style
				- 9 : Line picture
				- 10 : Rolling stock id
				- 11 : Rank
				- 12 : Following services with same departure hour
				
			Object : TimetableColumn
		*/
		class TimetableServiceRowInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, TimetableServiceRowInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const timetables::TimetableColumn& object,
				const boost::posix_time::time_duration& lastSchedule,
				std::size_t rank,
				std::size_t followingServicesWithSameHour,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			TimetableServiceRowInterfacePage();
		};
	}
}

#endif // SYNTHESE_TimetableServiceRowInterfacePage_H__

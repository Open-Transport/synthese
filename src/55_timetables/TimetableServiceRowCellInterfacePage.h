
/** TimetableServiceRowCellInterfacePage class header.
	@file TimetableServiceRowCellInterfacePage.h
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

#ifndef SYNTHESE_TimetableServiceRowCellInterfacePage_H__
#define SYNTHESE_TimetableServiceRowCellInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "TimetableColumn.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace pt
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace pt
	{
		class RollingStock;
	}

	namespace timetables
	{
		/** TimetableServiceRowCellInterfacePage Interface Page Class.
			@ingroup m55Pages refPages
			@author Hugues
			@date 2009

			@code timetable_service_row_cell @endcode

			Parameters :
				- 0 : ptime
				- 1 : Minutes
				- 2 : Stop name
				- 3 : City name
				- 4 : Stop alias
				- 5 : Rank
				- 6 : Rolling stock ID

			Object : Cell
		*/
		class TimetableServiceRowCellInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, TimetableServiceRowCellInterfacePage>
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
				const TimetableColumn::Content::value_type& object,
				const pt::RollingStock* rollingStock,
				std::size_t rank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			TimetableServiceRowCellInterfacePage();
		};
	}
}

#endif // SYNTHESE_TimetableServiceRowCellInterfacePage_H__

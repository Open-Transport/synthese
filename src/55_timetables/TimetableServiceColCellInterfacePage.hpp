
/** TimetableServiceColCellInterfacePage class header.
	@file TimetableServiceColCellInterfacePage.hpp
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

#ifndef SYNTHESE_TimetableRowCellInterfacePage_H__
#define SYNTHESE_TimetableRowCellInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/date_time/time_duration.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace env
	{
		class CommercialLine;
	}

	namespace pt
	{
		class RollingStock;
	}

	namespace timetables
	{
		class TimetableWarning;

		/** 55.11 Interface page : Cell for a timetable with services by column.
			@ingroup m55Pages refPages
			@author Hugues Romain
			@date 2010

			@code timetable_service_col_cell @endcode

			Common parameters :
				- 0 : type of cell (line, time, note, booking, rollingstock)
				- 1 : column number
				- 2 : row number

			If cell type is line, parameters :
				- 3 : line id
				- 4 : line name
				- 5 : line color
				- 6 : line css
				- 7 : line image url

			If cell type is time, parameters :
				- 3 : hours (empty = stop is not served)
				- 4 : minutes (empty = stop is not served)
			
			If cell type is note, parameters :
				- 3 : note number (empty = no note)
				- 4 : note text

			If cell type is booking, parameters :
				- 3 : 
				@todo implement booking cell


			If cell type is rolling stock :
				- 3 : rolling stock id
				- 4 : rolling stock short name
				- 5 : rolling stock long name
		*/
		class TimetableServiceColCellInterfacePage:
			public util::FactorableTemplate<interfaces::InterfacePage, TimetableServiceColCellInterfacePage>
		{
		public:
			static const std::string TYPE_LINE;
			static const std::string TYPE_TIME;
			static const std::string TYPE_NOTE;
			static const std::string TYPE_BOOKING;
			static const std::string TYPE_ROLLING_STOCK;

			/** Display of a cell of the lines row of the timetable.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object Line to display
				@param rowRank Row number
				@param colRank Column number
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const env::CommercialLine& object,
				std::size_t rowRank,
				std::size_t colRank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			
			/** Display of a cell of a times row of the timetable.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object Time to display (not_a_date_time = stop not served)
				@param rowRank Row number
				@param colRank Column number
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				boost::posix_time::time_duration object,
				std::size_t rowRank,
				std::size_t colRank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;



			/** Display of a cell of the notes row of the timetable.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object Note to display (NULL = no note)
				@param rowRank Row number
				@param colRank Column number
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const TimetableWarning* object,
				std::size_t rowRank,
				std::size_t colRank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;


			
			/** Display of a cell of the rolling stock row of the timetable.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object Rolling stock to display (NULL = no information)
				@param rowRank Row number
				@param colRank Column number
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const pt::RollingStock* object,
				std::size_t rowRank,
				std::size_t colRank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			TimetableServiceColCellInterfacePage();
		};
	}
}

#endif // SYNTHESE_TimetableRowCellInterfacePage_H__

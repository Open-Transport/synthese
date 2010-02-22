
/** RoutePlannerSheetColumnInterfacePage class header.
	@file RoutePlannerSheetColumnInterfacePage.h

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

#include "InterfacePage.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace routeplanner
	{
		/** Schedule sheet cell.
			@code schedule_sheet_column @endcode
			@ingroup m53Pages refPages

			Parameters :
				- 0 : isItFirst row
				- 1 : isItLast row
				- 2 : columnNumber
				- 3 : isItFoot row
				- 4 : firstArrivalTime
				- 5 : lastArrivalTime
				- 6 : isItContinuousService
				- 7 : isFirstWriting
				- 8 : isLastWriting
				- 9 : is it first foot

		*/
		class RoutePlannerSheetColumnInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,RoutePlannerSheetColumnInterfacePage>
		{
		public:
			static const std::string DATA_IS_FIRST_ROW;
			static const std::string DATA_IS_LAST_ROW;
			static const std::string DATA_COLUMN_NUMBER;
			static const std::string DATA_IS_FOOT;
			static const std::string DATA_FIRST_TIME;
			static const std::string DATA_LAST_TIME;
			static const std::string DATA_IS_CONTINUOUS_SERVICE;
			static const std::string DATA_IS_FIRST_WRITING;
			static const std::string DATA_IS_LAST_WRITING;
			static const std::string DATA_IS_FIRST_FOOT;

			RoutePlannerSheetColumnInterfacePage();

			/** Display of schedule sheet cell.
				@param stream Stream to write on
				@param isItFirstLine (0) Is the cell the first departure or arrival ?
				@param isItLastLine (1) Is the cell the last departure or arrival ?
				@param columnNumber (2) Rank of the column from left to right
				@param isItFootLine (3) Is the cell on a pedestrian junction ?
				@param firstTime (4) Start of continuous service, Time else
				@param lastTime (5) End of continuous service, Time else
				@param isItContinuousService (6) Is the cell on a continuous service ?
				@param isFirstWriting (7) Is it the first time that we write on the column ?
				@param isLastWriting (8) Is it the last time that we write on the column ?
				@param session 
			*/
			void display(
				std::ostream& stream
				, bool isItFirstLine
				, bool isItLastLine
				, size_t columnNumber
				, bool isItFootLine
				, const boost::posix_time::time_duration& firstTime
				, const boost::posix_time::time_duration& lastTime
				, bool isItContinuousService
				, bool isFirstWriting
				, bool isLastWriting
				, bool isFirstFoot
				, const server::Request* request = NULL
			) const;
		};
	}
}

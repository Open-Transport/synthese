
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

#include "11_interfaces/InterfacePage.h"
#include "04_time/Hour.h"

namespace synthese
{
	namespace routeplanner
	{
		/** Schedule sheet cell.
			@code schedule_sheet_column @endcode
			@ingroup m33Pages refPages
		*/
		class RoutePlannerSheetColumnInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display of schedule sheet cell.
				@param stream Stream to write on
				@param isItFirstLine (0) Is the cell the first departure or arrival ?
				@param isItLastLine (1) Is the cell the last departure or arrival ?
				@param columnNumber (2) Rank of the column from left to right
				@param isItFootLine (3) Is the cell on a pedestrian junction ?
				@param firstTime (4) Start of continuous service, Time else
				@param lastTime (5) End of continuous service, Time else
				@param isItContinuousService (6) Is the cell on a continuous service ?
				@param session 
			*/
			void display( std::ostream& stream, bool isItFirstLine, bool isItLastLine, size_t columnNumber, bool isItFootLine
				, const synthese::time::Hour& firstTime, const synthese::time::Hour& lastTime
				, bool isItContinuousService, const server::Request* request = NULL ) const;
		};
	}
}

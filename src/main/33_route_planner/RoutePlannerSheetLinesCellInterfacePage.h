
/** RoutePlannerSheetLinesCellInterfacePage class header.
	@file RoutePlannerSheetLinesCellInterfacePage.h

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

namespace synthese
{
	namespace routeplanner
	{
		class Journey;

		/** Schedule sheet lines list cell.
			@code schedule_sheet_lines_cell @endcode
			@ingroup m33Pages refPages
		*/
		class RoutePlannerSheetLinesCellInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display of schedule sheet lines list cell.
				@param stream Stream to write on
				@param columnNumber (0) Column rank from left to right
				@param journey () The journey to describe
				@param site Displayed site
			*/
			void display(
				std::ostream& stream
				, size_t columnNumber
				, interfaces::VariablesMap& variables
				, const Journey* object
				, const server::Request* request= NULL ) const;
		};
	}
}




/** RoutePlannerSheetLineInterfacePage class header.
	@file RoutePlannerSheetLineInterfacePage.h

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

#ifndef SYNTHESE_RoutePlannerSheetLineInterfacePage_H__
#define SYNTHESE_RoutePlannerSheetLineInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace server
	{
		class Request;
	}



	namespace routeplanner
	{
		/** Line of schedule sheet.
			@code schedule_sheet_row @endcode
			@ingroup m53Pages refPages

			Parameters :
				- 0 : text conaining cells
				- 1 : alternateColor (1) indicates if the line rank is odd or even. Values :
					- false/0 = odd
					- true/1 = even
				- 2 : 0|1 is origin row
				- 3 : 0|1 is destination row
		*/
		class RoutePlannerSheetLineInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,RoutePlannerSheetLineInterfacePage>
		{
		public:
			RoutePlannerSheetLineInterfacePage();

			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param text (0) Text containing cells
				@param alternateColor (1) indicates if the line rank is odd or even. Values :
					- false/0 = odd
					- true/1 = even
				@param place (Place* object) : Corresponding place of the line
				@param site Displayed site
			*/
			void display(
				std::ostream& stream
				, const std::string& text
				, bool alternateColor
				, bool isOrigin
				, bool isDestination
				, interfaces::VariablesMap& variables
				, const geography::Place* place
				, const server::Request* request = NULL
			) const;


		};
	}
}
#endif // SYNTHESE_RoutePlannerSheetLineInterfacePage_H__

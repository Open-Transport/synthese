
/** ResultLinesListInterfaceelement class header.
	@file schedulesheetlineslistlineinterfaceelement.h

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

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace routeplanner
	{
		//////////////////////////////////////////////////////////////////////////
		/// Displays the list of the lines used by at least one solution of the
		/// result of the route planning.
		///
		///	@author Hugues Romain
		///	@date 2009
		///	@ingroup m53Library refLibrary
		///
		///	Parameters :
		///		- 0 : Page code to use to render the lines (optional)
		///
		/// Object : route planner result (
		///
		///	@code result_lines_list <page_code> @endcode
		///
		class ResultLinesListInterfaceElement :
			public util::FactorableTemplate<interfaces::LibraryInterfaceElement, ResultLinesListInterfaceElement>
		{
		private:
			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _pageCode;

		public:
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;
	
			/** Parser.
				@param vel list of parameters :
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};

	}
}



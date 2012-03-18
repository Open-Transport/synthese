
/** DeparturesTableInterfaceElement class header.
	@file DeparturesTableInterfaceElement.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_DeparturesTableInterfaceElement_H__
#define SYNTHESE_DeparturesTableInterfaceElement_H__

#include "FactorableTemplate.h"
#include "LibraryInterfaceElement.h"

#include <string>

namespace synthese
{
    namespace interfaces
    {
	class ValueInterfaceElement;

    }


	namespace departure_boards
	{

		/** Departure table based on a display screen definition.
			@ingroup m54Library refLibrary
			@todo Verify the documentation

			Displays : The defined departure table.
			Parameters :
				- 0 : Step to use for going to the next page (if you don't know what to do, put 1)
				- 1 : Max page number :
					- 0 : Don't handle page
					- -1 : Unlimited pages number (one per displayed stop)
					- n>0 : max of n pages (one per displayed stop)
				- 2 : Separator between pages
				- 3 : Number of departures to hide (default = 0)
				- 4 : Message to display
				- 5 : Display services number (default : false)
				- 6 : Display track number
				- 7 : Number of intermediates stops
				- 8 : Display team
				- 9 : Blinking delay
		*/
		class DeparturesTableInterfaceElement : public util::FactorableTemplate<interfaces::LibraryInterfaceElement, DeparturesTableInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _multiplicateurRangeeVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _pagesVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _pageSeparator;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _departuresToHide;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _message;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayServiceNumber;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayQuai;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _numberOfIntermediatesStops;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayTeam;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _blinkingDelay;

		public:
			void storeParameters(interfaces::ValueElementList& vel);
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;
			const std::string getLabel(const interfaces::ParametersVector& parameters) const;
		};

	}
}

#endif // SYNTHESE_DeparturesTableInterfaceElement_H__

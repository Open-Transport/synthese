
/** DeparturesTableRoutePlanningInterfaceElement class header.
	@file DeparturesTableRoutePlanningInterfaceElement.h

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

#ifndef SYNTHESE_DeparturesTableRoutePlanningInterfaceElement_H__
#define SYNTHESE_DeparturesTableRoutePlanningInterfaceElement_H__

#include <string>

#include "LibraryInterfaceElement.h"

namespace synthese
{
	namespace departure_boards
	{

		/** Route planning departure table based on a display screen definition.
			@ingroup m54Library refLibrary

			Displays : The defined departure table.
			Parameters :
				- 0 : Origin id
				- 1 : Number of departures to hide (default = 0)
				- 2 : Display services number (default : false)
				- 3 : Display track number
				- 4 : With transfer
				- 5 : Blinking delay
				- 6 :
		*/
		class DeparturesTableRoutePlanningInterfaceElement:
			public util::FactorableTemplate<interfaces::LibraryInterfaceElement, DeparturesTableRoutePlanningInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _originId;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _departuresToHide;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayServiceNumber;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayQuai;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _withTransfer;
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

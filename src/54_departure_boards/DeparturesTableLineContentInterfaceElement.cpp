
/** DeparturesTableLineContentInterfaceElement class implementation.
	@file DeparturesTableLineContentInterfaceElement.cpp

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

#include "DeparturesTableLineContentInterfaceElement.h"

#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "Interface.h"
#include "LineMarkerInterfacePage.h"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "Request.h"
#include "DeparturesTableTypes.h"
#include "DeparturesTableInterfaceElement.h"
#include "Service.h"

#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace pt;
	using namespace util;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement, departure_boards::DeparturesTableLineContentInterfaceElement>::FACTORY_KEY("departurestablelinecell");

	namespace departure_boards
	{
		void DeparturesTableLineContentInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() < 4)
				throw interfaces::InterfacePageException("Malformed departure table line declaration");

			_htmlStartLine = vel.front();
			_htmlEndLine = vel.front();
			_cellWidth = vel.front();
			_cellHeight = vel.front();
		}

		string DeparturesTableLineContentInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const JourneyPattern* line(static_cast<const JourneyPattern*>(static_cast<const ArrivalDepartureRow*>(object)->first.getService()->getPath()));
			const LineMarkerInterfacePage* page(_page->getInterface()->getPage<LineMarkerInterfacePage>());

			page->display(
				stream
				, variables
				, _htmlStartLine->getValue(parameters, variables, object, request)
				, _htmlEndLine->getValue(parameters, variables, object, request)
				, lexical_cast<int>(_cellWidth->getValue(parameters, variables, object, request))
				, lexical_cast<int>(_cellHeight->getValue(parameters, variables, object,request))
				, *line->getCommercialLine()
				, request
				);
			return string();
		}
	}
}

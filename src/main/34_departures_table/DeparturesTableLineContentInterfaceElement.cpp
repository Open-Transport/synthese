
/** DeparturesTableLineContentInterfaceElement class implementation.
	@file DeparturesTableLineContentInterfaceElement.cpp

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

#include <vector>

#include "01_util/Conversion.h"

#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/Interface.h"

#include "15_env/LineMarkerInterfacePage.h"
#include "15_env/LineStop.h"

#include "30_server/Request.h"
#include "30_server/Site.h"

#include "34_departures_table/Types.h"
#include "34_departures_table/DeparturesTableInterfaceElement.h"
#include "34_departures_table/DeparturesTableLineContentInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;

	namespace departurestable
	{
		DeparturesTableLineContentInterfaceElement::DeparturesTableLineContentInterfaceElement()
			:			_htmlStartLine(NULL)
			, _htmlEndLine(NULL)
			, _cellWidth(NULL)
			, _cellHeight(NULL)
		{		}

		DeparturesTableLineContentInterfaceElement::~DeparturesTableLineContentInterfaceElement()
		{
			delete _htmlStartLine;
			delete _htmlEndLine;
			delete _cellWidth;
			delete _cellHeight;
		}

		void DeparturesTableLineContentInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() < 4)
				throw interfaces::InterfacePageException("Malformed departure table line declaration");

			_htmlStartLine = vel.front();
			_htmlEndLine = vel.front();
			_cellWidth = vel.front();
			_cellHeight = vel.front();
		}

		void DeparturesTableLineContentInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const Line* line = ((const ArrivalDepartureRow*) object)->first.linestop->getLine();

			const LineMarkerInterfacePage* page = request->getSite()->getInterface()->getPage<LineMarkerInterfacePage>();
			page->display(stream
				, _htmlStartLine->getValue(parameters, object, request)
				, _htmlEndLine->getValue(parameters, object, request)
				, Conversion::ToInt(_cellWidth->getValue(parameters, object, request))
				, Conversion::ToInt(_cellHeight->getValue(parameters, object,request))
				, line, request
				);
			delete page;
		}
	}
}

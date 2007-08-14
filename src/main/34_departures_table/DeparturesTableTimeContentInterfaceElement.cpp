
/** DeparturesTableTimeContentInterfaceElement class implementation.
	@file DeparturesTableTimeContentInterfaceElement.cpp

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

#include "04_time/DateTime.h"


#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "15_env/LineMarkerInterfacePage.h"

#include "34_departures_table/Types.h"
#include "34_departures_table/DeparturesTableTimeContentInterfaceElement.h"
#include "34_departures_table/DepartureTableRowInterfacePage.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace time;

	namespace departurestable
	{
		void DeparturesTableTimeContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("Malformed departure table time cell declaration");

			_zeroVIE = vel.front();
			_beforeIfNext = vel.front();
			_afterIfNext = vel.front();
		}

		string DeparturesTableTimeContentInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureRow*	row = (const ArrivalDepartureRow*) object;
			const DateTime&				__Moment = row->first.servicePointer.getActualDateTime();
			bool						blinking = row->first.blinking;
			string						__Zero = _zeroVIE->getValue(parameters, variables, object, request);
			string						__AvantSiImminent = _beforeIfNext->getValue(parameters, variables, object, request);
			string						__ApresSiImminent = _afterIfNext->getValue(parameters, variables, object, request);

			if (blinking)
				stream << __AvantSiImminent;
			if ( __Moment.getHour ().getHours() < 10 )
				stream << __Zero;
			stream << __Moment.getHour ().toString();
			if (blinking)
				stream << __ApresSiImminent;
			return "";
		}
	}
}

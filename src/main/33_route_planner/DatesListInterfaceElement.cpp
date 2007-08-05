
/** DatesListInterfaceElement class implementation.
	@file DatesListInterfaceElement.cpp

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

#include "DatesListInterfaceElement.h"

#include "33_route_planner/RoutePlannerFunction.h"
#include "33_route_planner/Site.h"

#include "30_server/Request.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/DateTimeInterfacePage.h"
#include "11_interfaces/Interface.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace time;

	namespace routeplanner
	{
		void DatesListInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_value = vel.front();
		}

		string DatesListInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			// Collecte des param�tres sp�cifiques
			Date dateDefaut(Date::FromInternalString(_value->getValue(parameters, variables, object, request)));
			shared_ptr<const RoutePlannerFunction> function(request->getFunction<RoutePlannerFunction>());

			assert(function.get());
			
			Date minDate(function->getSite()->getStartDate());
			Date maxDate(function->getSite()->getEndDate());
				
			//time::Date __DateJour;
			//if ( !__Site->getSolutionsPassees() && DateMin < __DateJour )
			//	DateMin = __DateJour;
			//synthese::time::Date DateMax = dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLListeDatesMax ] ->Texte( __Parametres ) );

			shared_ptr<const DateTimeInterfacePage> datePage(_page->getInterface()->getPage<DateTimeInterfacePage>());

			// Construction de l'objet HTML
			stream << "<select name=\"" << RoutePlannerFunction::PARAMETER_DAY << "\">";
			for (Date iDate(minDate); iDate <= maxDate; iDate++)
			{
				stream << "<option ";
				if ( iDate == dateDefaut )
					stream << "selected=\"1\" ";
				stream << "value=\"" << iDate.toInternalString() << "\">";
				datePage->display(stream, variables, iDate, request);
				stream << "</option>";
			}
			stream << "</select>";

			return string();
		}

		DatesListInterfaceElement::~DatesListInterfaceElement()
		{
		}
	}
}

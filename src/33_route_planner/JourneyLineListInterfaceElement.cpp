
/** JourneyLineListInterfaceElement class implementation.
	@file JourneyLineListInterfaceElement.cpp

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

#include "JourneyLineListInterfaceElement.h"
#include "Request.h"
#include "Road.h"
#include "Line.h"
#include "LineMarkerInterfacePage.h"
#include "ServiceUse.h"
#include "Journey.h"
#include "Service.h"
#include "Interface.h"
#include "ValueElementList.h"
#include "Conversion.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace pt;
	using namespace graph;
	using namespace road;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::JourneyLineListInterfaceElement>::FACTORY_KEY(
		"journey_line_list"
	);

	namespace routeplanner
	{
		string JourneyLineListInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			// Parameters
			const Journey* journey = static_cast<const Journey*>(object);
			bool __AfficherLignesPied = Conversion::ToBool(
				_displayPedestrianLines->getValue(parameters, variables, object, request)
			);
			const LineMarkerInterfacePage* lineMarkerInterfacePage(
				_page->getInterface()->getPage<LineMarkerInterfacePage>()
			);

			// Fabrication de l'affichage
			BOOST_FOREACH(const ServiceUse& leg, journey->getServiceUses())
			{
				if ( __AfficherLignesPied || !dynamic_cast<const Road*> (leg.getService()->getPath ()) )
				{
					lineMarkerInterfacePage->display(
						stream
						, variables
						, _rowStartHtml->getValue(parameters, variables, object, request)
						, _rowEndHtml->getValue(parameters, variables, object, request)
						, Conversion::ToInt(_pixelWidth->getValue(parameters, variables, object, request))
						, Conversion::ToInt(_pixelHeight->getValue(parameters, variables, object, request))
						, *static_cast<const Line*>(leg.getService()->getPath ())->getCommercialLine()
						, request
					);
				}
			}

			return string();
		}

		void JourneyLineListInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			_displayPedestrianLines = vel.front();
			_rowStartHtml = vel.front();
			_rowEndHtml = vel.front();
			_pixelWidth = vel.front();
			_pixelHeight = vel.front();
		}
	}
}

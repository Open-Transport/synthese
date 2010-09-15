
//////////////////////////////////////////////////////////////////////////////////////////
/// PTRouteDetailFunction class header.
///	@file PTRouteDetailFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTRouteDetailFunction.hpp"
#include "Request.h"
#include "Edge.h"
#include "StopPoint.hpp"
#include "JourneyPattern.hpp"
#include "RequestException.h"
#include "StopArea.hpp"
#include "City.h"
#include "CommercialLine.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace graph;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,pt::PTRouteDetailFunction>::FACTORY_KEY(
			"PTRouteDetailFunction"
		);

	namespace pt
	{
		ParametersMap PTRouteDetailFunction::_getParametersMap() const
		{
			ParametersMap result;
			if(_journeyPattern.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _journeyPattern->getKey());
			}
			return result;
		}



		void PTRouteDetailFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_journeyPattern = Env::GetOfficialEnv().getRegistry<JourneyPattern>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<JourneyPattern>&)
			{
				throw RequestException("No such Journey Pattern");
			}
		}


		void PTRouteDetailFunction::run(ostream& stream, const Request& request) const
		{
			const CommercialLine * commercialLine(_journeyPattern->getCommercialLine());
			// XML header
			stream <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<route xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/PTRouteDetailFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance" <<
				"\" id=\""            << _journeyPattern->getKey() <<
				"\" name=\""          << _journeyPattern->getName()<<
				"\" lineId=\""        << commercialLine->getKey() <<
				"\" lineName=\""      << commercialLine->getName() <<
				"\" lineShortName=\"" << commercialLine->getShortName() <<
				"\" lineLongName=\""  << commercialLine->getLongName() <<
				"\" lineImage=\""     << commercialLine->getImage() <<
				"\" lineStyle=\""     << commercialLine->getStyle() <<
				"\" lineColor=\""     << commercialLine->getColor() <<
				"\">";

			BOOST_FOREACH(const Edge* edge,_journeyPattern->getEdges())
			{
				const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));

				const StopArea * connPlace(stopPoint->getConnectionPlace());

				stream << "<stop id=\""   << stopPoint->getKey() <<
					"\" name=\""          << stopPoint->getName() <<
					"\" operatorCode=\""  << stopPoint->getCodeBySource() <<
					"\" cityId=\""        << connPlace->getCity()->getKey() <<
					"\" cityName=\""      << connPlace->getCity()->getName() <<
					"\" directionAlias=\""<< connPlace->getName26() <<
					"\" />";
			}
			// XML footer
			stream <<
				"</route>";
		}



		bool PTRouteDetailFunction::isAuthorized(const Session* session) const
		{
			return true;
		}



		string PTRouteDetailFunction::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}

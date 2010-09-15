////////////////////////////////////////////////////////////////////////////////
/// StopAreasListFunction class implementation.
///	@file StopAreasListFunction.cpp
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "StopAreasListFunction.hpp"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "LineMarkerInterfacePage.h"
#include "Request.h"
#include "JourneyPattern.hpp"
#include "Path.h"
#include "Edge.h"
#include "City.h"

#include <map>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace graph;
	using namespace std;
	
	template<> const string util::FactorableTemplate<server::Function,pt::StopAreasListFunction>::FACTORY_KEY(
		"StopAreasListFunction"
	);
	
	namespace pt
	{
		ParametersMap StopAreasListFunction::_getParametersMap() const
		{
			ParametersMap result;
			return result;
		}

		void StopAreasListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_commercialLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such Commercial Line");
			}
		}


		void StopAreasListFunction::run( std::ostream& stream, const Request& request ) const
		{
			//Key Is stop Area Name
			typedef map<string,const StopArea *> stopMap;
			stopMap stopAreaMap;

			//Populate stopAreaMap
			BOOST_FOREACH(const Path* path, _commercialLine->getPaths())
			{
				const JourneyPattern* journey = dynamic_cast<const JourneyPattern*>(path);
				BOOST_FOREACH(const Edge* edge,journey->getEdges())
				{
					const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));
					const StopArea * connPlace(stopPoint->getConnectionPlace());

					stopAreaMap[connPlace->getName()] = connPlace;
				}
			}

			// XML header
			stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<stopAreas xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/StopAreasListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					;

			BOOST_FOREACH(stopMap::value_type& it, stopAreaMap)
			{
				stream << "<stopArea id=\""   << it.second->getKey() <<
						"\" name=\""          << it.second->getName() <<
						"\" cityId=\""        << it.second->getCity()->getKey() <<
						"\" cityName=\""      << it.second->getCity()->getName() <<
						"\" directionAlias=\""<< it.second->getName26() <<
						"\" />";
			}

			// XML footer
			stream << "</stopAreas>";
		}

		bool StopAreasListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}

		std::string StopAreasListFunction::getOutputMimeType() const
		{
			return "text/xml";
		}
	}
}

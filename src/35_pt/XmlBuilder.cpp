
/** XmlBuilder class implementation.
	@file 35_pt/XmlBuilder.cpp

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


#include "XmlBuilder.h"

#include <assert.h>

#include "StopArea.hpp"
#include "XmlToolkit.h"
#include "DesignatedLinePhysicalStop.hpp"

using namespace synthese::util::XmlToolkit;
using namespace boost;
using namespace geos::geom;


namespace synthese
{
	using namespace geography;
	using namespace util;
	using namespace pt;



	namespace pt
	{


		boost::shared_ptr<City>
		XmlBuilder::CreateCity (XMLNode& node)
		{
			util::RegistryKeyType id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));

			return boost::shared_ptr<City>(new City (id, name));
		}




		boost::shared_ptr<StopArea>
		XmlBuilder::CreateConnectionPlace (XMLNode& node,
						   const Registry<City>& cities)
		{
			// assert ("connectionPlace" == node.getName ());

			util::RegistryKeyType id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
			util::RegistryKeyType cityId (GetLongLongAttr (node, "cityId"));

			std::string typeStr (GetStringAttr (node, "connectionType"));

			bool type(lexical_cast<bool>(typeStr));

			posix_time::time_duration defaultTransferDelay(posix_time::minutes(GetIntAttr(
					node,
					"defaultTransferDelay"
			)	)	);

			boost::shared_ptr<const City> city = cities.get (cityId);

			boost::shared_ptr<StopArea> p(
				new StopArea(id, type, defaultTransferDelay)
			);
			p->setCity(const_cast<City*>(city.get()));
			p->setName(name);

			return p;
		}


		boost::shared_ptr<CommercialLine>
		XmlBuilder::CreateCommercialLine (XMLNode& node)
		{
			util::RegistryKeyType id (GetLongLongAttr (node, "id"));

			boost::shared_ptr<CommercialLine> commercialLine (new CommercialLine ());

			commercialLine->setKey (id);

			std::string shortName (GetStringAttr (node, "shortName"));
			commercialLine->setShortName (shortName);

			std::string longName (GetStringAttr (node, "longName"));
			commercialLine->setLongName (longName);

			std::string color (GetStringAttr (node, "color"));
			commercialLine->setColor (synthese::util::RGBColor (color));

			std::string style (GetStringAttr (node, "style"));
			commercialLine->setStyle (style);

			return commercialLine;
		}




		boost::shared_ptr<JourneyPattern>
		XmlBuilder::CreateLine (XMLNode& node,
					const Registry<CommercialLine>& commercialLines)
		{
			util::RegistryKeyType id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));

			boost::shared_ptr<JourneyPattern> line(new JourneyPattern(id, name));

			util::RegistryKeyType commercialLineId (GetLongLongAttr (node, "commercialLineId"));
			line->setCommercialLine (const_cast<CommercialLine*>(commercialLines.get(commercialLineId).get()));

			std::string direction (GetStringAttr (node, "direction"));
			line->setDirection (direction);

			return line;
		}




		boost::shared_ptr<DesignatedLinePhysicalStop>
		XmlBuilder::CreateLineStop (XMLNode& node,
						Registry<JourneyPattern>& lines,
						const Registry<StopPoint>& physicalStops)
		{
			// assert ("lineStop" == node.getName ());

			util::RegistryKeyType id (GetLongLongAttr (node, "id"));

			util::RegistryKeyType physicalStopId (GetLongLongAttr (node, "physicalStopId"));
			util::RegistryKeyType lineId (GetLongLongAttr (node, "lineId"));
			int rankInPath (GetIntAttr (node, "rankInPath"));
			bool isDeparture (GetBoolAttr (node, "isDeparture"));
			bool isArrival (GetBoolAttr (node, "isArrival"));
			double metricOffset (GetDoubleAttr (node, "metricOffset"));

			boost::shared_ptr<JourneyPattern> line = lines.getEditable (lineId);

			boost::shared_ptr<DesignatedLinePhysicalStop> lineStop (
			new DesignatedLinePhysicalStop(
				id,
				line.get(),
				rankInPath,
				isDeparture, isArrival,
				metricOffset,
				const_cast<StopPoint*>(physicalStops.get(physicalStopId).get())))
			;

			// Add via points
//			int nbPoints = GetChildNodeCount(node, "point");
//			for (int i=0; i<nbPoints; ++i)
//			{
//				XMLNode pointNode = GetChildNode (node, "point", i);
//				lineStop->addViaPoint (CreatePoint (pointNode));
//			}

			line->addEdge(*lineStop);

			return lineStop;
		}



		boost::shared_ptr<StopPoint>
		XmlBuilder::CreatePhysicalStop(
			XMLNode& node,
			const Registry<StopArea>& connectionPlaces)
		{
			// assert ("physicalStop" == node.getName ());
			util::RegistryKeyType id (GetLongLongAttr (node, "id"));
			std::string name (GetStringAttr (node, "name"));
			util::RegistryKeyType placeId (GetLongLongAttr (node, "placeId"));
			double x (GetDoubleAttr (node, "x"));
			double y (GetDoubleAttr (node, "y"));

			return boost::shared_ptr<StopPoint>(
				new synthese::pt::StopPoint(
					id,
					name,
					connectionPlaces.get (placeId).get(),
					CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(x, y)
			)	);

		}



		Coordinate XmlBuilder::CreatePoint(
			XMLNode& node
		){
			// assert ("point" == node.getName ());

			return Coordinate(
				GetDoubleAttr (node, "x"),
				GetDoubleAttr (node, "y")
			);
		}
	}
}

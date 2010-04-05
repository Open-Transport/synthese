
/** XmlBuilder class implementation.
	@file XmlBuilder.cpp

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


#include "XmlBuilder.h"

#include <assert.h>

#include "PublicTransportStopZoneConnectionPlace.h"

#include "Conversion.h"
#include "XmlToolkit.h"
#include "UId.h"

#include "Point2D.h"

using namespace synthese::util::XmlToolkit;
using namespace boost;


namespace synthese
{
	using namespace geography;	
	using namespace geometry;
	using namespace util;
	using namespace pt;
	
	

	namespace pt
	{


		shared_ptr<City> 
		XmlBuilder::CreateCity (XMLNode& node)
		{
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
		    
			return shared_ptr<City>(new City (id, name));
		}




		shared_ptr<PublicTransportStopZoneConnectionPlace>
		XmlBuilder::CreateConnectionPlace (XMLNode& node, 
						   const Registry<City>& cities)
		{
			// assert ("connectionPlace" == node.getName ());
		    
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
			uid cityId (GetLongLongAttr (node, "cityId"));
		    
			std::string typeStr (GetStringAttr (node, "connectionType"));

			bool type(Conversion::ToBool(typeStr));

			posix_time::time_duration defaultTransferDelay(posix_time::minutes(GetIntAttr(
					node, 
					"defaultTransferDelay"
			)	)	);

			shared_ptr<const City> city = cities.get (cityId);

			shared_ptr<PublicTransportStopZoneConnectionPlace> p(
				new PublicTransportStopZoneConnectionPlace(id, type, defaultTransferDelay)
			);
			p->setCity(city.get());
			p->setName(name);

			return p;
		}


		shared_ptr<CommercialLine> 
		XmlBuilder::CreateCommercialLine (XMLNode& node)
		{
			uid id (GetLongLongAttr (node, "id"));

			shared_ptr<CommercialLine> commercialLine (new CommercialLine ());

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




		shared_ptr<Line> 
		XmlBuilder::CreateLine (XMLNode& node, 
					const Registry<CommercialLine>& commercialLines)
		{
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));

			shared_ptr<Line> line(new Line(id, name));

			uid commercialLineId (GetLongLongAttr (node, "commercialLineId"));
			line->setCommercialLine (const_cast<CommercialLine*>(commercialLines.get(commercialLineId).get()));
		 
			std::string direction (GetStringAttr (node, "direction"));
			line->setDirection (direction);
		   
			return line;
		}




		shared_ptr<LineStop> 
		XmlBuilder::CreateLineStop (XMLNode& node, 
						Registry<Line>& lines,
						const Registry<PhysicalStop>& physicalStops)
		{
			// assert ("lineStop" == node.getName ());

			uid id (GetLongLongAttr (node, "id"));

			uid physicalStopId (GetLongLongAttr (node, "physicalStopId"));
			uid lineId (GetLongLongAttr (node, "lineId"));
			int rankInPath (GetIntAttr (node, "rankInPath"));
			bool isDeparture (GetBoolAttr (node, "isDeparture"));
			bool isArrival (GetBoolAttr (node, "isArrival"));
			double metricOffset (GetDoubleAttr (node, "metricOffset"));
		   
			shared_ptr<Line> line = lines.getEditable (lineId);

			shared_ptr<LineStop> lineStop (
			new LineStop (
				id,
				line.get(), 
				rankInPath, 
				isDeparture, isArrival,
				metricOffset, 
				const_cast<PhysicalStop*>(physicalStops.get(physicalStopId).get())))
			;

			// Add via points
			int nbPoints = GetChildNodeCount(node, "point");
			for (int i=0; i<nbPoints; ++i) 
			{
				XMLNode pointNode = GetChildNode (node, "point", i);
				lineStop->addViaPoint (CreatePoint (pointNode));
			}

			line->addEdge (lineStop.get());


			return lineStop;

		}





		shared_ptr<PhysicalStop> 
		XmlBuilder::CreatePhysicalStop(
			XMLNode& node,
			const Registry<PublicTransportStopZoneConnectionPlace>& connectionPlaces)
		{
			// assert ("physicalStop" == node.getName ());
			uid id (GetLongLongAttr (node, "id"));
			std::string name (GetStringAttr (node, "name"));
			uid placeId (GetLongLongAttr (node, "placeId"));
			double x (GetDoubleAttr (node, "x"));
			double y (GetDoubleAttr (node, "y"));

			return shared_ptr<PhysicalStop>(
			new synthese::pt::PhysicalStop (
				id,
				name, 
				connectionPlaces.get (placeId).get(), 
				x, y))
			;

		}



		Point2D
		XmlBuilder::CreatePoint (XMLNode& node)
		{
			// assert ("point" == node.getName ());

			double x (GetDoubleAttr (node, "x"));
			double y (GetDoubleAttr (node, "y"));

			return Point2D (x, y);
		}

	}
}

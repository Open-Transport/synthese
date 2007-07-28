
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

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "06_geometry/Point2D.h"

using namespace synthese::util::XmlToolkit;
using namespace boost;


namespace synthese
{
	using namespace geometry;

	namespace env
	{


		shared_ptr<City> 
		XmlBuilder::CreateCity (XMLNode& node)
		{
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
		    
			return shared_ptr<City>(new City (id, name));
		}




		 shared_ptr<Axis>
		 XmlBuilder::CreateAxis (XMLNode& node)
		 {
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));

			bool free (GetBoolAttr (node, "free"));
			bool authorized (GetBoolAttr (node, "authorized"));
		    
			return shared_ptr<Axis>(new synthese::env::Axis (id, name, free, authorized));
		 }



		    
		shared_ptr<ConnectionPlace>
		XmlBuilder::CreateConnectionPlace (XMLNode& node, 
						   const City::Registry& cities)
		{
			// assert ("connectionPlace" == node.getName ());
		    
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
			uid cityId (GetLongLongAttr (node, "cityId"));
		    
			std::string typeStr (GetStringAttr (node, "connectionType"));

			ConnectionPlace::ConnectionType type = ConnectionPlace::CONNECTION_TYPE_FORBIDDEN;
			if (typeStr == "road-road") 
			type = ConnectionPlace::CONNECTION_TYPE_ROADROAD;
			else if (typeStr == "road-line") 
			type = ConnectionPlace::CONNECTION_TYPE_ROADLINE;
			else if (typeStr == "line-line") 
			type = ConnectionPlace::CONNECTION_TYPE_LINELINE;
			else if (typeStr == "recommendedShort") 
			type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED_SHORT;
			else if (typeStr == "recommended") 
			type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED;


			int defaultTransferDelay (GetIntAttr (node, 
							  "defaultTransferDelay", 
							  ConnectionPlace::FORBIDDEN_TRANSFER_DELAY));

			shared_ptr<const City> city = cities.get (cityId);

			return shared_ptr<ConnectionPlace>(
						new ConnectionPlace (id, name, city.get(),
										type, defaultTransferDelay))
						;
		    

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
					const Axis::Registry& axes,
					const CommercialLine::Registry& commercialLines)
		{
			uid id (GetLongLongAttr (node, "id"));

			std::string name (GetStringAttr (node, "name"));
			uid axisId (GetLongLongAttr (node, "axisId"));

			shared_ptr<Line> line (
									 new synthese::env::Line (id, name, 
							 									 axes.get (axisId).get()))
									 ;

			uid commercialLineId (GetLongLongAttr (node, "commercialLineId"));
			line->setCommercialLine (commercialLines.get (commercialLineId).get());
		 
			std::string direction (GetStringAttr (node, "direction"));
			line->setDirection (direction);
		   
			return line;
		}




		shared_ptr<LineStop> 
		XmlBuilder::CreateLineStop (XMLNode& node, 
						Line::Registry& lines,
						const PhysicalStop::Registry& physicalStops)
		{
			// assert ("lineStop" == node.getName ());

			uid id (GetLongLongAttr (node, "id"));

			uid physicalStopId (GetLongLongAttr (node, "physicalStopId"));
			uid lineId (GetLongLongAttr (node, "lineId"));
			int rankInPath (GetIntAttr (node, "rankInPath"));
			bool isDeparture (GetBoolAttr (node, "isDeparture"));
			bool isArrival (GetBoolAttr (node, "isArrival"));
			double metricOffset (GetDoubleAttr (node, "metricOffset"));
		   
			shared_ptr<Line> line = lines.getUpdateable (lineId);

			shared_ptr<LineStop> lineStop (
			new LineStop (
				id,
				line.get(), 
				rankInPath, 
				isDeparture, isArrival,
				metricOffset, 
				physicalStops.get(physicalStopId).get()))
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
		XmlBuilder::CreatePhysicalStop (XMLNode& node, const ConnectionPlace::Registry& connectionPlaces)
		{
			// assert ("physicalStop" == node.getName ());
			uid id (GetLongLongAttr (node, "id"));
			std::string name (GetStringAttr (node, "name"));
			uid placeId (GetLongLongAttr (node, "placeId"));
			double x (GetDoubleAttr (node, "x"));
			double y (GetDoubleAttr (node, "y"));

			return shared_ptr<PhysicalStop>(
			new synthese::env::PhysicalStop (
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

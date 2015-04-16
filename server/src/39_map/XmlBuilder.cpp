
/** XmlBuilder class implementation.
	@file 39_map/XmlBuilder.cpp
	@author Hugues Romain
	@date 2008

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

#include "XmlToolkit.h"
#include "UtilTypes.h"
#include "Log.h"
#include "DrawableLine.h"
#include "Map.h"
#include "MapBackgroundManager.h"
#include "StopArea.hpp"
#include "LineStop.h"

#include <assert.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace util::XmlToolkit;
	using namespace graph;
	using namespace geography;

	namespace map
	{
		DrawableLine* XmlBuilder::CreateDrawableLine(
			XMLNode& node,
			const Registry<JourneyPattern>& lines
		){
			// assert ("drawableLine" == node.getName ());

			util::RegistryKeyType lineId (GetLongLongAttr (node, "lineId"));

			boost::shared_ptr<const JourneyPattern> line = lines.get (lineId);

			const vector<Edge*>& lineStops = line->getEdges ();

			assert (lineStops.size () >= 2);

			int fromLineStopIndex (GetIntAttr (node, "fromLineStopId", 0));
			int toLineStopIndex (GetIntAttr (node, "toLineStopId", (int) lineStops.size () - 1));

			bool withPhysicalStops (GetBoolAttr (node, "withPhysicalStops", false));

			return new DrawableLine(
				line.get(),
				fromLineStopIndex,
				toLineStopIndex,
				withPhysicalStops
			);
		}




		Map*
		XmlBuilder::CreateMap (XMLNode& node, const Registry<JourneyPattern>& lines)
		{
			// assert ("map" == node.getName ());


			int outputWidth (GetIntAttr (node, "outputWidth", -1));
			int outputHeight (GetIntAttr (node, "outputHeight", -1));

			// Drawable lines
			std::set<DrawableLine*> selectedLines;
			int nbDrawableLines = node.nChildNode ("drawableLine");
			for (int i=0; i<nbDrawableLines; ++i)
			{
			XMLNode drawableLineNode = node.getChildNode ("drawableLine", i);
			selectedLines.insert (CreateDrawableLine (drawableLineNode, lines));
			}

			const MapBackgroundManager* mbm = 0;

			std::string backgroundId (GetStringAttr (node, "backgroundId", ""));
			if (backgroundId != "")
			{
			try
			{
				mbm = MapBackgroundManager::GetMapBackgroundManager (backgroundId);
			}
			catch (synthese::Exception& ex)
			{
				Log::GetInstance ().warn ("Cannot find background", ex);
			}
			}

			std::string urlPattern (GetStringAttr (node, "urlPattern", ""));

			Map* map = 0;

			bool preserveRatio (GetBoolAttr (node, "preserveRatio", true));

			double neighborhood (GetDoubleAttr (node, "neighborhood", 0.0));

			// If one of the 4 coordinates is missing, let the autofit
			// feature process the right rectangle
			if (
			(HasAttr (node, "lowerLeftLatitude") == false) ||
			(HasAttr (node, "lowerLeftLongitude") == false) ||
			(HasAttr (node, "upperRightLatitude") == false) ||
			(HasAttr (node, "upperRightLongitude") == false)
			)
			{
			map = new Map (selectedLines,
					outputWidth,
					outputHeight,
					neighborhood,
					preserveRatio,
					mbm, urlPattern);

			}
			else
			{
			double lowerLeftLatitude (GetDoubleAttr (node, "lowerLeftLatitude"));
			double lowerLeftLongitude (GetDoubleAttr (node, "lowerLeftLongitude"));
			double upperRightLatitude (GetDoubleAttr (node, "upperRightLatitude"));
			double upperRightLongitude (GetDoubleAttr (node, "upperRightLongitude"));

				map = new Map (selectedLines,
					Rectangle (lowerLeftLatitude,
						lowerLeftLongitude,
						upperRightLatitude - lowerLeftLatitude,
						upperRightLongitude - lowerLeftLongitude),
					outputWidth,
					outputHeight,
					preserveRatio,
					mbm, urlPattern);


			}


			bool lineGrouping (GetBoolAttr (node, "lineGrouping", true));
			if (lineGrouping) map->setLineGrouping (lineGrouping);

			return map;
		}

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




		boost::shared_ptr<LineStop>
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

			boost::shared_ptr<LineStop> lineStop (
				new LineStop(
					id,
					line.get(),
					rankInPath,
					isDeparture, isArrival,
					metricOffset,
					const_cast<StopPoint&>(*physicalStops.get(physicalStopId))
			)	);

			// Add via points
//			int nbPoints = GetChildNodeCount(node, "point");
//			for (int i=0; i<nbPoints; ++i)
//			{
//				XMLNode pointNode = GetChildNode (node, "point", i);
//				lineStop->addViaPoint (CreatePoint (pointNode));
//			}

//			line->addEdge(*lineStop);

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

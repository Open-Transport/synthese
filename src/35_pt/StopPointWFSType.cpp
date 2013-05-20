
/** StopPointWFSType class implementation.
	@file StopPointWFSType.cpp

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

#include "StopPointWFSType.hpp"
#include "Env.h"
#include "StopPoint.hpp"
#include "CoordinatesSystem.hpp"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "City.h"

#include <geos/geom/Envelope.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace map;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<map::WFSType, StopPointWFSType>::FACTORY_KEY("StopPoint");
	}

	namespace map
	{
		template<>
		void WFSTypeTemplate<StopPointWFSType>::GetSchema(
			std::ostream& stream
		){
			stream <<
				"<element name=\"" << FACTORY_KEY << "\" type=\"synthese:" << FACTORY_KEY << "Type\" substitutionGroup=\"gml:_Feature\"/>" <<
				"<complexType name=\"" << FACTORY_KEY << "Type\">" <<
				"<complexContent>" <<
				"<extension base=\"gml:AbstractFeatureType\">" <<
				"<sequence>" <<
				"<element name=\"msGeometry\" type=\"gml:GeometryPropertyType\" minOccurs=\"0\" maxOccurs=\"1\"/>" <<
				"<element name=\"" << TABLE_COL_ID << "\" type=\"string\"/>" <<
				"<element name=\"" << StopPointTableSync::COL_OPERATOR_CODE << "\" type=\"string\"/>" <<
				"<element name=\"CITY_NAME\" type=\"string\"/>" <<
				"<element name=\"STOP_AREA_NAME\" type=\"string\"/>" <<
				"<element name=\"" << StopPointTableSync::COL_NAME << "\" type=\"string\"/>" <<
				"</sequence>" <<
				"</extension>" <<
				"</complexContent>" <<
				"</complexType>";
		}


		template<>
		void WFSTypeTemplate<StopPointWFSType>::GetFeatures(
			std::ostream& stream,
			const geos::geom::Envelope& envelope,
			const CoordinatesSystem& sr
		){
			stream << fixed;

			StopPointTableSync::SearchResult stops(
				StopPointTableSync::SearchByEnvelope(
					envelope,
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
			)	);

			BOOST_FOREACH(const boost::shared_ptr<StopPoint>& ps, stops)
			{
				boost::shared_ptr<geos::geom::Point> point(
					sr.convertPoint(*ps->getGeometry())
				);

				stream <<
					"<gml:featureMember><synthese:" << FACTORY_KEY << ">" <<
					"<gml:boundedBy><gml:Box srsName=\"EPSG:" << sr.getSRID() << "\">" <<
					"<gml:coordinates>" << point->getX() << "," << point->getY() << " " << point->getX() << "," << point->getY() << "</gml:coordinates>" <<
					"</gml:Box></gml:boundedBy>" <<

					"<synthese:msGeometry>" <<
					"<gml:Point srsName=\"EPSG:" << sr.getSRID() << "\">" <<
					"<gml:coordinates>" << point->getX() << "," << point->getY() <<
					"</gml:coordinates></gml:Point></synthese:msGeometry>"

					"<synthese:" << TABLE_COL_ID << ">" << ps->getKey() << "</synthese:" << TABLE_COL_ID << ">" <<
					"<synthese:" << StopPointTableSync::COL_OPERATOR_CODE << ">" << ps->getCodeBySources() << "</synthese:" << StopPointTableSync::COL_OPERATOR_CODE << ">" <<
					"<synthese:CITY_NAME>" << ps->getConnectionPlace()->getCity()->getName() << "</synthese:CITY_NAME>" <<
					"<synthese:STOP_AREA_NAME>" << ps->getConnectionPlace()->getName() << "</synthese:STOP_AREA_NAME>" <<
					"<synthese:" << StopPointTableSync::COL_NAME << ">" << ps->getName() << "</synthese:" << StopPointTableSync::COL_NAME << ">" <<
					"</synthese:" << FACTORY_KEY << "></gml:featureMember>" <<
					endl
				;
			}
		}
}	}

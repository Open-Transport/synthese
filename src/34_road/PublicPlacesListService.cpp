
/** PublicPlacesListService class implementation.
	@file PublicPlacesListService.cpp
	@author Hugues Romain
	@date 2010

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

#include "PublicPlacesListService.hpp"

#include "MimeTypes.hpp"
#include "PublicPlace.h"
#include "PublicPlaceEntrance.hpp"
#include "PublicPlaceTableSync.h"
#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace geos::geom;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function, road::PublicPlacesListService>::FACTORY_KEY("PublicPlacesListService");

	namespace road
	{
		const string PublicPlacesListService::PARAMETER_PAGE_ID = "page_id";
		const string PublicPlacesListService::PARAMETER_BBOX = "bbox";
		const string PublicPlacesListService::PARAMETER_SRID = "srid";		

		const string PublicPlacesListService::TAG_PLACES = "places";
		const string PublicPlacesListService::TAG_PLACE = "place";

		const string PublicPlacesListService::DATA_ID = "id";
		const string PublicPlacesListService::DATA_NAME = "name";
		const string PublicPlacesListService::DATA_CATEGORY = "category";
		const string PublicPlacesListService::DATA_DETAILS = "details";
		const string PublicPlacesListService::DATA_X = "x";
		const string PublicPlacesListService::DATA_Y = "y";

		const string PublicPlacesListService::DATA_DISTANCE_TO_BBOX_CENTER = "distanceToBboxCenter";
		


		ParametersMap PublicPlacesListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else
			if(_bbox)
			{
				stringstream s;
				s << _bbox->getMinX() << "," << _bbox->getMinY() << "," <<
					_bbox->getMaxX() << "," << _bbox->getMaxY();
				map.insert(PARAMETER_BBOX, s.str());
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}

			return map;
		}



		void PublicPlacesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Coordinate system
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(
					PARAMETER_SRID,
					CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID()
			)	);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// BBox selection request
			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				vector< string > parsed_bbox;
				split(parsed_bbox, bbox, is_any_of(",; ") );

				if(parsed_bbox.size() != 4)
				{
					throw RequestException("Malformed bbox.");
				}

				boost::shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1]))
				);
				boost::shared_ptr<Point> pt2(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[2]), lexical_cast<double>(parsed_bbox[3]))
				);
				pt1 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt1);
				pt2 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt2);

				_bbox = Envelope(
					pt1->getX(),
					pt2->getX(),
					pt1->getY(),
					pt2->getY()
				);
			}

			// CMS output
			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID))
			{
				try
				{
					_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such page");
				}
			}
			else
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}
			
		}



		util::ParametersMap PublicPlacesListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap pm;
			
			BOOST_FOREACH(const Registry<PublicPlace>::value_type& publicPlace, Env::GetOfficialEnv().getRegistry<PublicPlace>())
			{
				boost::shared_ptr<ParametersMap> publicPlacePm(new ParametersMap);
				publicPlace.second->toParametersMap(*publicPlacePm, _coordinatesSystem);

				pm.insert(TAG_PLACE, publicPlacePm);
			}

			// Output
			if(_page.get()) // CMS output
			{
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type placeMap, pm.getSubMaps(TAG_PLACE))
				{
					placeMap->merge(getTemplateParameters());

					// Display of the stop by the template
					_page->display(stream, request, *placeMap);
				}
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					TAG_PLACES,
					""
				);
			}

			return pm;
		}



		bool PublicPlacesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		string PublicPlacesListService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
}	}

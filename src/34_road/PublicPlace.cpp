
/** PublicPlace class implementation.
	@file PublicPlace.cpp

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

#include "PublicPlace.h"

#include "City.h"
#include "Crossing.h"
#include "ParametersMap.h"
#include "PublicPlaceEntrance.hpp"
#include "ReverseRoadChunk.hpp"
#include "RoadModule.h"
#include "VertexAccessMap.h"

#include <geos/geom/Point.h>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace road;
	using namespace impex;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace, PublicPlace>::FACTORY_KEY("PublicPlace");
	}

	CLASS_DEFINITION(PublicPlace, "t013_public_places", 13)
	FIELD_DEFINITION_OF_OBJECT(PublicPlace, "public_place_id", "public_place_ids")

	namespace road
	{
		const string PublicPlace::DATA_ID = "id";
		const string PublicPlace::DATA_NAME = "name";
		const string PublicPlace::DATA_X = "x";
		const string PublicPlace::DATA_Y = "y";



		PublicPlace::PublicPlace (
			util::RegistryKeyType id
		):	Registrable(id),
			Object<PublicPlace, PublicPlaceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(NamedPlaceField),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)	),
		  NamedPlaceTemplate<PublicPlace>()
		{}



		PublicPlace::~PublicPlace ()
		{}



		std::string PublicPlace::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? NamedPlace::getName() : text);
		}



		void PublicPlace::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const geography::Place::GraphTypes& whatToSearch
		) const	{

			// Return results only if road graph is searched
			if(whatToSearch.find(RoadModule::GRAPH_ID) == whatToSearch.end())
			{
				return;
			}

			// Loop en entrances
			BOOST_FOREACH(
				const Entrances::value_type& it,
				_entrances
			){
				if(!it->getRoadChunk())
				{
					continue;
				}
				it->getVertexAccessMap(result, accessParameters, whatToSearch);
			}
		}



		const boost::shared_ptr<Point>& PublicPlace::getPoint() const
		{
			return get<PointGeometry>();
		}



		void PublicPlace::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			const std::string& prefix
		) const {

			// ID
			pm.insert(prefix + DATA_ID, get<Key>());

			// Name
			pm.insert(prefix + DATA_NAME, NamedPlace::getName());

			// City
			if(getCity())
			{
				getCity()->toParametersMap(pm, NULL, prefix);
			}

			// Coordinates
			if(coordinatesSystem && getPoint())
			{
				boost::shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*getPoint())
				);
				{
					stringstream s;
					s << std::fixed << pg->getX();
					pm.insert(prefix + DATA_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << pg->getY();
					pm.insert(prefix + DATA_Y, s.str());
				}
			}
		}



		void PublicPlace::toParametersMap( util::ParametersMap& pm, const std::string& prefix ) const
		{
			toParametersMap(pm,&CoordinatesSystem::GetInstanceCoordinatesSystem(),prefix);
		}



		void PublicPlace::toParametersMap( util::ParametersMap& pm ) const
		{
			string emptyPrefix;
			toParametersMap(pm, emptyPrefix);
		}



		void PublicPlace::addEntrance( PublicPlaceEntrance& entrance )
		{
			_entrances.insert(&entrance);
		}



		void PublicPlace::removeEntrance( PublicPlaceEntrance& entrance )
		{
			_entrances.erase(&entrance);
		}



		std::string PublicPlace::getName() const
		{
			return NamedPlace::getName();
		}



		void PublicPlace::link(
			util::Env& env,
			bool withAlgorithmOptimizations /*= false*/
		){
			// Registration to city matcher
			if(getCity())
			{
				const_cast<City*>(getCity())->addPlaceToMatcher(env.getEditableSPtr(this));
			}

			// Registration to all places matcher
			if(	&env == &Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				GeographyModule::GetGeneralAllPlacesMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}

			// Registration to public places matcher
			if(	&env == &Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				RoadModule::GetGeneralPublicPlacesMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}
		}



		void PublicPlace::unlink()
		{
			// City matcher
			City* city(const_cast<City*>(getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher(*this);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General all places
				GeographyModule::GetGeneralAllPlacesMatcher().remove(
					getFullName()
				);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General public places
				RoadModule::GetGeneralPublicPlacesMatcher().remove(
					getFullName()
				);
			}
		}
}	}

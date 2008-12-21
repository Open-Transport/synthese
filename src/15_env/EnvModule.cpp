
/** EnvModule class implementation.
    @file EnvModule.cpp

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

#include "EnvModule.h"

#include "01_util/Constants.h"
#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/T9Filter.h"

#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "Crossing.h"
#include "CrossingTableSync.h"
#include "Line.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "Road.h"
#include "RoadTableSync.h"
#include "PublicPlace.h"
#include "PublicPlaceTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "PlaceAlias.h"
#include "PlaceAliasTableSync.h"
#include "Address.h"
#include "AddressTableSync.h"
#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"

#include "12_security/Constants.h"
#include "Right.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace lexmatcher;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,env::EnvModule>::FACTORY_KEY("35_env");
	}

    namespace env
    {
		LexicalMatcher<uid>				EnvModule::_citiesMatcher; //!< @todo To be moved in transportwebsite::Site
		LexicalMatcher<uid>				EnvModule::_citiesT9Matcher; //!< @todo To be moved in transportwebsite::Site

		void EnvModule::initialize()
		{
		}

	

		std::vector<pair<uid, std::string> > EnvModule::getCommercialLineLabels(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, bool withAll
		){
			vector<pair<uid,string> > m;
			if (withAll)
			m.push_back(make_pair(UNKNOWN_VALUE, "(toutes)"));

			Env env;
			CommercialLineTableSync::Search(env, rights, totalControl, neededLevel);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
				m.push_back(make_pair(line->getKey(), line->getShortName()));
			return m;
		}



		shared_ptr<NonPermanentService> EnvModule::FetchEditableService(
			const RegistryKeyType& id,
			Env& env
		){
			int tableId(decodeTableId(id));
			if(tableId == ScheduledServiceTableSync::TABLE.ID)
				return static_pointer_cast<NonPermanentService, ScheduledService>(env.getEditableRegistry<ScheduledService>().getEditable(id));
			if (tableId == ContinuousServiceTableSync::TABLE.ID)
				return static_pointer_cast<NonPermanentService, ContinuousService>(env.getEditableRegistry<ContinuousService>().getEditable(id));
			
			return shared_ptr<NonPermanentService>();
		}


		
		shared_ptr<const Place> EnvModule::FetchPlace(
			const util::RegistryKeyType& id,
			Env& env
		){
			shared_ptr<const Place> place = static_pointer_cast<const Place, const AddressablePlace>(FetchAddressablePlace (id, env));
			if (!place.get())
				place = static_pointer_cast<const Place, const IncludingPlace>(FetchIncludingPlace (id, env));
			return place;
		}




		shared_ptr<const AddressablePlace> EnvModule::FetchAddressablePlace(
			const util::RegistryKeyType& id,
			Env& env
		){
			int tableId(decodeTableId(id));
			if(tableId == ConnectionPlaceTableSync::TABLE.ID)
				return static_pointer_cast<const AddressablePlace, const PublicTransportStopZoneConnectionPlace>(ConnectionPlaceTableSync::Get(id, &env));
			if (tableId == PublicPlaceTableSync::TABLE.ID)
				return static_pointer_cast<const AddressablePlace, const PublicPlace>(PublicPlaceTableSync::Get(id, &env));
			if (tableId == RoadTableSync::TABLE.ID)
				return static_pointer_cast<const AddressablePlace, const Road>(RoadTableSync::Get(id, &env));
			if (tableId == CrossingTableSync::TABLE.ID)
				return static_pointer_cast<const AddressablePlace, const Crossing>(CrossingTableSync::Get(id, &env));
			return shared_ptr<const AddressablePlace>();
		}


		shared_ptr<AddressablePlace> EnvModule::FetchEditableAddressablePlace(
			const util::RegistryKeyType& id,
			util::Env& env
		){
			int tableId(decodeTableId(id));
			if (tableId == ConnectionPlaceTableSync::TABLE.ID)
				return static_pointer_cast<AddressablePlace, PublicTransportStopZoneConnectionPlace>(env.getEditableRegistry<PublicTransportStopZoneConnectionPlace>().getEditable(id));
			if (tableId == PublicPlaceTableSync::TABLE.ID)
				return static_pointer_cast<AddressablePlace, PublicPlace>(env.getEditableRegistry<PublicPlace>().getEditable(id));
			if (tableId == RoadTableSync::TABLE.ID)
				return static_pointer_cast<AddressablePlace, Road>(env.getEditableRegistry<Road>().getEditable(id));
			if (tableId == CrossingTableSync::TABLE.ID)
				return static_pointer_cast<AddressablePlace, Crossing>(env.getEditableRegistry<Crossing>().getEditable(id));
			return shared_ptr<AddressablePlace>();
		}



		shared_ptr<const IncludingPlace> EnvModule::FetchIncludingPlace(
			const util::RegistryKeyType& id,
			Env& env
		){
			int tableId(decodeTableId(id));
			if (tableId == PlaceAliasTableSync::TABLE.ID)
				return static_pointer_cast<const IncludingPlace, const PlaceAlias>(env.getRegistry<PlaceAlias>().get(id));
			if (tableId == CityTableSync::TABLE.ID)
				return static_pointer_cast<const IncludingPlace, const City>(env.getRegistry<City>().get(id));
			return shared_ptr<const IncludingPlace>();
		}



		shared_ptr<const Vertex> EnvModule::FetchVertex(
			const util::RegistryKeyType& id,
			Env& env
		){
			int tableId(decodeTableId(id));
			if (tableId == PhysicalStopTableSync::TABLE.ID)
				return static_pointer_cast<const Vertex, const PhysicalStop>(env.getRegistry<PhysicalStop>().get(id));
			if (tableId == AddressTableSync::TABLE.ID)
				return static_pointer_cast<const Vertex, const Address>(env.getRegistry<Address>().get(id));
			return shared_ptr<const Vertex>();
		}



		CityList EnvModule::guessCity (const std::string& fuzzyName, int nbMatches, bool t9)
		{
			Env& env(*Env::GetOfficialEnv());
			const Registry<City>& cities(env.getRegistry<City>());
			CityList result;
			LexicalMatcher<uid>::MatchResult matches = (t9 ? _citiesT9Matcher : _citiesMatcher).bestMatches (fuzzyName, nbMatches);
			for (LexicalMatcher<uid>::MatchResult::iterator it = matches.begin ();
			 it != matches.end (); ++it)
			{
				uid id = it->value;
				result.push_back (cities.get(id));
			}
			return result;
		}



		void EnvModule::getNetworkLinePlaceRightParameterList(ParameterLabelsVector& m)
		{
			Env env;
			TransportNetworkTableSync::Search(env);
			CommercialLineTableSync::Search(env);

			m.push_back(make_pair(string(), "--- Réseaux ---"));
			BOOST_FOREACH(shared_ptr<TransportNetwork> network, env.getRegistry<TransportNetwork>())
				m.push_back(make_pair(Conversion::ToString(network->getKey()), network->getName() ));

			m.push_back(make_pair(string(), "--- Lignes ---"));
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
				m.push_back(make_pair(Conversion::ToString(line->getKey()), line->getName() ));

		}

		std::string EnvModule::getName() const
		{
			return  "Réseaux de transport public";
		}

		void EnvModule::AddToCitiesMatchers( City* city )
		{
			_citiesMatcher.add(city->getName (), city->getKey ());
			
			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;
			
			_citiesT9Matcher.add(ss.str(), city->getKey());
		}

		void EnvModule::RemoveFromCitiesMatchers( City* city )
		{
			_citiesMatcher.remove(city->getName());

			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;

			_citiesT9Matcher.remove(ss.str());
		}

		const Place* EnvModule::FetchPlace( const std::string& cityName, const std::string& placeName )
		{
				const Place* place(NULL);

				if (cityName.empty())
					throw Exception("Empty city name");

				shared_ptr<const City> city;
				CityList cityList = guessCity(cityName, 1);
				if (cityName.empty())
					throw Exception("An error has occured in city name search");
				city = cityList.front();
				place = city.get();
				assert(place != NULL);

				if (!placeName.empty())
				{
					LexicalMatcher<const Place*>::MatchResult places = city->getAllPlacesMatcher().bestMatches(placeName, 1);
					if (!places.empty())
					{
						place = places.front().value;
					}
				}

				return place;		
		

		}
    }
}

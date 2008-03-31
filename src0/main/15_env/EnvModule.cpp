
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

#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CommercialLine.h"
#include "15_env/Crossing.h"
#include "15_env/Line.h"
#include "15_env/ScheduledService.h"
#include "15_env/ContinuousService.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/Road.h"
#include "15_env/PublicPlace.h"
#include "15_env/City.h"
#include "15_env/PlaceAlias.h"
#include "15_env/Address.h"
#include "15_env/PhysicalStop.h"

#include "12_security/Constants.h"
#include "12_security/Right.h"

#include <boost/iostreams/filtering_stream.hpp>

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

		shared_ptr<Path> EnvModule::fetchPath (const uid& id)
		{
			if (Line::Contains (id))
				return Line::GetUpdateable (id);
			//		if (_roads.contains (id)) return _roads.get (id);
			return shared_ptr<Path>();
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
			
			vector<shared_ptr<CommercialLine> > vl(CommercialLineTableSync::search(rights, totalControl, neededLevel));
			for(vector<shared_ptr<CommercialLine> >::const_iterator it = vl.begin(); it != vl.end(); ++it)
			m.push_back(make_pair((*it)->getKey(), (*it)->getShortName()));
			return m;
		}



		shared_ptr<NonPermanentService> EnvModule::fetchService (const uid& id)
		{
			if (ScheduledService::Contains (id))
				return ScheduledService::GetUpdateable (id);
			if (ContinuousService::Contains (id))
				return ContinuousService::GetUpdateable (id);
			return shared_ptr<NonPermanentService>();
		}


		
		shared_ptr<const Place> 
		EnvModule::fetchPlace (const uid& id)
		{
			shared_ptr<const Place> place = static_pointer_cast<const Place, const AddressablePlace>(fetchAddressablePlace (id));
			if (!place.get())
			place = static_pointer_cast<const Place, const IncludingPlace>(fetchIncludingPlace (id));
			return place;
		}




		shared_ptr<const AddressablePlace> 
		EnvModule::fetchAddressablePlace (const uid& id)
		{
			if (PublicTransportStopZoneConnectionPlace::Contains (id))
				return static_pointer_cast<const AddressablePlace, const PublicTransportStopZoneConnectionPlace>(PublicTransportStopZoneConnectionPlace::Get (id));
			if (PublicPlace::Contains (id))
				return static_pointer_cast<const AddressablePlace, const PublicPlace>(PublicPlace::Get (id));
			if (Road::Contains (id))
				return static_pointer_cast<const AddressablePlace, const Road>(Road::Get (id));
			if (Crossing::Contains(id))
				return static_pointer_cast<const AddressablePlace, const Crossing>(Crossing::Get(id));

			return shared_ptr<const AddressablePlace>();
		}


		shared_ptr<AddressablePlace> 
			EnvModule::fetchUpdateableAddressablePlace (const uid& id)
		{
			if (PublicTransportStopZoneConnectionPlace::Contains (id))
				return static_pointer_cast<AddressablePlace, PublicTransportStopZoneConnectionPlace>(PublicTransportStopZoneConnectionPlace::GetUpdateable(id));
			if (PublicPlace::Contains (id))
				return static_pointer_cast<AddressablePlace, PublicPlace>(PublicPlace::GetUpdateable(id));
			if (Road::Contains (id))
				return static_pointer_cast<AddressablePlace, Road>(Road::GetUpdateable(id));
			if (Crossing::Contains (id))
				return static_pointer_cast<AddressablePlace, Crossing>(Crossing::GetUpdateable(id));

			return shared_ptr<AddressablePlace>();
		}



		shared_ptr<const IncludingPlace> 
		EnvModule::fetchIncludingPlace (const uid& id)
		{
			if (PlaceAlias::Contains (id))
				return static_pointer_cast<const IncludingPlace, const PlaceAlias>(PlaceAlias::Get (id));
			if (City::Contains (id))
				return static_pointer_cast<const IncludingPlace, const City>(City::Get (id));

			return shared_ptr<const IncludingPlace>();
		}



		shared_ptr<const Vertex> 
		EnvModule::fetchVertex (const uid& id)
		{
			if (PhysicalStop::Contains (id))
				return static_pointer_cast<const Vertex, const PhysicalStop>(PhysicalStop::Get (id));
			if (Address::Contains (id))
				return static_pointer_cast<const Vertex, const Address>(Address::Get (id));
			return shared_ptr<const Vertex>();
		}



		LineSet
		EnvModule::fetchLines (const uid& commercialLineId) 
		{
			LineSet result;
			for (Line::ConstIterator it = Line::Begin ();
				it != Line::End (); ++it)
			{
			if (it->second->getCommercialLine ()->getKey () == commercialLineId) 
				result.insert (it->second);
			}
			return result;
		}




		CityList EnvModule::guessCity (const std::string& fuzzyName, int nbMatches, bool t9)
		{
			CityList result;
			LexicalMatcher<uid>::MatchResult matches = (t9 ? _citiesT9Matcher : _citiesMatcher).bestMatches (fuzzyName, nbMatches);
			for (LexicalMatcher<uid>::MatchResult::iterator it = matches.begin ();
			 it != matches.end (); ++it)
			{
				uid id = it->value;
				result.push_back (City::Get (id));
			}
			return result;
		}



		void EnvModule::getNetworkLinePlaceRightParameterList(ParameterLabelsVector& m)
		{


			m.push_back(make_pair(string(), "--- Réseaux ---"));
			vector<shared_ptr<TransportNetwork> > networks(TransportNetworkTableSync::search());
			for (vector<shared_ptr<TransportNetwork> >::const_iterator it = networks.begin(); it != networks.end(); ++it)
			m.push_back(make_pair(Conversion::ToString((*it)->getKey()), (*it)->getName() ));

			m.push_back(make_pair(string(), "--- Lignes ---"));
			vector<shared_ptr<CommercialLine> > lines(CommercialLineTableSync::search());
			for (vector<shared_ptr<CommercialLine> >::const_iterator itl = lines.begin(); itl != lines.end(); ++itl)
			m.push_back(make_pair(Conversion::ToString((*itl)->getKey()), (*itl)->getName() ));

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

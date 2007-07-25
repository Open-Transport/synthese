
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

#include "01_util/Constants.h"
#include "01_util/UId.h"

#include "15_env/EnvModule.h"
#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/CommercialLine.h"

#include "12_security/Constants.h"
#include "12_security/Right.h"

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace lexmatcher;
    using namespace security;

    namespace env
    {
	Address::Registry				EnvModule::_addresses;
	City::Registry					EnvModule::_cities;
	ConnectionPlace::Registry		EnvModule::_connectionPlaces;
	PhysicalStop::Registry			EnvModule::_physicalStops;
	CommercialLine::Registry		EnvModule::_commercialLines;
	Line::Registry					EnvModule::_lines;
	Axis::Registry					EnvModule::_axes;
	Fare::Registry					EnvModule::_fares;
	BikeCompliance::Registry		EnvModule::_bikeCompliances;
	HandicappedCompliance::Registry	EnvModule::_handicappedCompliances;
	PedestrianCompliance::Registry	EnvModule::_pedestrianCompliances;
	ReservationRule::Registry		EnvModule::_reservationRules;
	LineStop::Registry				EnvModule::_lineStops;
	ScheduledService::Registry		EnvModule::_scheduledServices;
	ContinuousService::Registry		EnvModule::_continuousServices;
	TransportNetwork::Registry		EnvModule::_networks;
	PlaceAlias::Registry			EnvModule::_placeAliases;
	PublicPlace::Registry			EnvModule::_publicPlaces;
	RoadChunk::Registry				EnvModule::_roadChunks;
	Road::Registry					EnvModule::_roads;

	LexicalMatcher<uid>				EnvModule::_citiesMatcher; //!< @todo To be moved in RoutePlanner

	void EnvModule::initialize()
	{
	}



	City::Registry& EnvModule::getCities()
	{
	    return _cities;
	}

	ConnectionPlace::Registry& EnvModule::getConnectionPlaces()
	{
	    return _connectionPlaces;
	}

	PhysicalStop::Registry& EnvModule::getPhysicalStops()
	{
	    return _physicalStops;
	}

	CommercialLine::Registry& EnvModule::getCommercialLines()
	{
	    return _commercialLines;
	}

	Line::Registry& EnvModule::getLines()
	{
	    return _lines;
	}

	Axis::Registry& EnvModule::getAxes()
	{
	    return _axes;
	}

	Fare::Registry& EnvModule::getFares()
	{
	    return _fares;
	}

	BikeCompliance::Registry& EnvModule::getBikeCompliances()
	{
	    return _bikeCompliances;
	}

	HandicappedCompliance::Registry& EnvModule::getHandicappedCompliances()
	{
	    return _handicappedCompliances;
	}

	PedestrianCompliance::Registry& EnvModule::getPedestrianCompliances()
	{
	    return _pedestrianCompliances;
	}

	ReservationRule::Registry& EnvModule::getReservationRules()
	{
	    return _reservationRules;
	}


	shared_ptr<Path> EnvModule::fetchPath (const uid& id)
	{
	    if (_lines.contains (id)) return _lines.getUpdateable (id);
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

	LineStop::Registry& EnvModule::getLineStops()
	{
	    return _lineStops;
	}

	ScheduledService::Registry& EnvModule::getScheduledServices()
	{
	    return _scheduledServices;
	}


	shared_ptr<NonPermanentService> EnvModule::fetchService (const uid& id)
	{
	    if (_scheduledServices.contains (id))
			return _scheduledServices.getUpdateable (id);
	    if (_continuousServices.contains (id))
			return _continuousServices.getUpdateable (id);
	    return shared_ptr<NonPermanentService>();
	}

	ContinuousService::Registry& EnvModule::getContinuousServices()
	{
	    return _continuousServices;
	}

	TransportNetwork::Registry& EnvModule::getTransportNetworks()
	{
	    return _networks;
	}

	Address::Registry& EnvModule::getAddresses()
	{
	    return _addresses;
	}

	PlaceAlias::Registry& EnvModule::getPlaceAliases()
	{
	    return _placeAliases;
	}

	PublicPlace::Registry& EnvModule::getPublicPlaces()
	{
	    return _publicPlaces;
	}

	RoadChunk::Registry& EnvModule::getRoadChunks()
	{
	    return _roadChunks;
	}

	Road::Registry& EnvModule::getRoads()
	{
	    return _roads;
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
	    if (_connectionPlaces.contains (id))
		return static_pointer_cast<const AddressablePlace, const ConnectionPlace>(_connectionPlaces.get (id));
	    if (_publicPlaces.contains (id))
		return static_pointer_cast<const AddressablePlace, const PublicPlace>(_publicPlaces.get (id));
	    if (_roads.contains (id))
		return static_pointer_cast<const AddressablePlace, const Road>(_roads.get (id));

	    return shared_ptr<const AddressablePlace>();
	}


	shared_ptr<AddressablePlace> 
		EnvModule::fetchUpdateableAddressablePlace (const uid& id)
	{
		if (_connectionPlaces.contains (id))
			return static_pointer_cast<AddressablePlace, ConnectionPlace>(_connectionPlaces.getUpdateable(id));
		if (_publicPlaces.contains (id))
			return static_pointer_cast<AddressablePlace, PublicPlace>(_publicPlaces.getUpdateable(id));
		if (_roads.contains (id))
			return static_pointer_cast<AddressablePlace, Road>(_roads.getUpdateable(id));

		return shared_ptr<AddressablePlace>();
	}



	shared_ptr<const IncludingPlace> 
	EnvModule::fetchIncludingPlace (const uid& id)
	{
	    if (_placeAliases.contains (id))
		return static_pointer_cast<const IncludingPlace, const PlaceAlias>(_placeAliases.get (id));
	    if (_cities.contains (id))
		return static_pointer_cast<const IncludingPlace, const City>(_cities.get (id));

	    return shared_ptr<const IncludingPlace>();
	}







	shared_ptr<const Vertex> 
	EnvModule::fetchVertex (const uid& id)
	{
	    if (_physicalStops.contains (id))
		return static_pointer_cast<const Vertex, const PhysicalStop>(_physicalStops.get (id));
	    if (_addresses.contains (id))
		return static_pointer_cast<const Vertex, const Address>(_addresses.get (id));
	    return shared_ptr<const Vertex>();
	}


/*
  Vertex* 
  Environment::fetchVertex (const uid& id)
  {
  if (EnvModule::getPhysicalStops().contains (id)) return EnvModule::getPhysicalStops().getUpdateable (id).get();
  if (_addresses.contains (id)) return _addresses.getUpdateable(id).get();
  return 0;
  }
*/


	LineSet
	EnvModule::fetchLines (const uid& commercialLineId) 
	{
	    LineSet result;
	    for (Line::Registry::const_iterator it = _lines.begin ();
		 it != _lines.end (); ++it)
	    {
		if (it->second->getCommercialLine ()->getKey () == commercialLineId) 
		    result.insert (it->second);
	    }
	    return result;
	}




	CityList EnvModule::guessCity (const std::string& fuzzyName, int nbMatches)
	{
	    CityList result;
	    LexicalMatcher<uid>::MatchResult matches =  _citiesMatcher.bestMatches (fuzzyName, nbMatches);
	    for (LexicalMatcher<uid>::MatchResult::iterator it = matches.begin ();
		 it != matches.end (); ++it)
	    {
		uid id = it->value;
		result.push_back (getCities ().get (id));
	    }
	    return result;
	}

	synthese::lexmatcher::LexicalMatcher<uid>& 
	EnvModule::getCitiesMatcher ()
	{
	    return _citiesMatcher;
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

    }
}

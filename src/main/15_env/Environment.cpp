
/** Environment class implementation.
	@file Environment.cpp

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

#include "15_env/Environment.h"
#include "15_env/EnvModule.h"

using synthese::lexmatcher::LexicalMatcher;


namespace synthese
{
	using namespace time;
namespace env
{


Environment::Environment (const uid& id)
    : synthese::util::Registrable<uid,Environment> (id)
    , _citiesMatcher ()
{
}



Environment::~Environment ()
{
    // Delete all objects registered in this environment.
    _addresses.clear ();
    _documents.clear ();
    _lines.clear ();
    _roadChunks.clear ();
    _roads.clear ();
}




Address::Registry& 
Environment::getAddresses ()
{
    return _addresses;
}



const Address::Registry& 
Environment::getAddresses () const
{
    return _addresses;
}



Date Environment::interpretDate( const std::string& text ) const
{
	Date tempDate;
	if ( !text.size () )
		tempDate.updateDate( TIME_CURRENT );

	else if ( text.size () == 1 )
		switch ( text[ 0 ] )
	{
		case TEMPS_MIN_CIRCULATIONS:
			tempDate = getMinDateInUse ();
			break;
		case TEMPS_MAX_CIRCULATIONS:
			tempDate = getMaxDateInUse ();
			break;
		default:
			tempDate.updateDate(text[ 0 ] );
	}
	else
		tempDate.FromString(text);
	return tempDate;
}




City::Registry&
Environment::getCities ()
{
    return _cities;
}



const City::Registry&
Environment::getCities () const
{
    return _cities;
}



Document::Registry&
Environment::getDocuments ()
{
    return _documents;
}


const Document::Registry&
Environment::getDocuments () const
{
    return _documents;
}




ConnectionPlace::Registry&
Environment::getConnectionPlaces ()
{
    return _connectionPlaces;
}



const ConnectionPlace::Registry&
Environment::getConnectionPlaces () const
{
    return _connectionPlaces;
}




PlaceAlias::Registry& 
Environment::getPlaceAliases ()
{
    return _placeAliases;
}



const PlaceAlias::Registry& 
Environment::getPlaceAliases () const
{
    return _placeAliases;
}



PublicPlace::Registry& 
Environment::getPublicPlaces ()
{
    return _publicPlaces;
}



const PublicPlace::Registry& 
Environment::getPublicPlaces () const
{
    return _publicPlaces;
}






RoadChunk::Registry& 
Environment::getRoadChunks ()
{
    return _roadChunks;
}



const RoadChunk::Registry& 
Environment::getRoadChunks () const
{
    return _roadChunks;
}




Road::Registry& 
Environment::getRoads ()
{
    return _roads;
}



const Road::Registry& 
Environment::getRoads () const
{
    return _roads;
}



TransportNetwork::Registry& 
Environment::getTransportNetworks ()
{
    return _transportNetworks;
}



const TransportNetwork::Registry& 
Environment::getTransportNetworks () const
{
    return _transportNetworks;
}



synthese::lexmatcher::LexicalMatcher<uid>& 
Environment::getCitiesMatcher ()
{
    return _citiesMatcher;
}




const synthese::lexmatcher::LexicalMatcher<uid>& 
Environment::getCitiesMatcher () const
{
    return _citiesMatcher;
}





const Place* 
Environment::fetchPlace (const uid& id) const
{
    const Place* place = fetchAddressablePlace (id);
    if (place == 0) place = fetchIncludingPlace (id);
    return place;
}




const AddressablePlace* 
Environment::fetchAddressablePlace (const uid& id) const
{
    if (_connectionPlaces.contains (id)) return _connectionPlaces.get (id);
    if (_publicPlaces.contains (id)) return _publicPlaces.get (id);
    if (_roads.contains (id)) return _roads.get (id);

    return 0;
}




const IncludingPlace* 
Environment::fetchIncludingPlace (const uid& id) const
{
    if (_placeAliases.contains (id)) return _placeAliases.get (id);
    if (_cities.contains (id)) return _cities.get (id);

    return 0;
}







const Vertex* 
Environment::fetchVertex (const uid& id) const
{
	if (EnvModule::getPhysicalStops().contains (id)) return EnvModule::getPhysicalStops().get (id);
    if (_addresses.contains (id)) return _addresses.get (id);
    return 0;
}



Vertex* 
Environment::fetchVertex (const uid& id)
{
    if (EnvModule::getPhysicalStops().contains (id)) return EnvModule::getPhysicalStops().get (id);
    if (_addresses.contains (id)) return _addresses.get (id);
    return 0;
}







Environment::CityList Environment::guessCity (const std::string& fuzzyName, int nbMatches) const
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




const synthese::time::Date& 
Environment::getMinDateInUse () const
{
    return _minDateInUse;
}


const synthese::time::Date& 
Environment::getMaxDateInUse () const
{
    return _maxDateInUse;
}




void 
Environment::updateMinMaxDatesInUse (synthese::time::Date newDate, bool marked)
{
    if (marked)
    {
	if ( (_minDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
	     (newDate < _minDateInUse) ) 
	{
	    _minDateInUse = newDate;
	}

	if ( (_maxDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
	     (newDate > _maxDateInUse) ) 
	{
	    _maxDateInUse = newDate;
	}
    }
    else
    {
	// TODO not written yet...
    }
    
}

const std::string& Environment::getName() const
{
	return _name;
}

void Environment::setName( const std::string& name )
{
	_name = name;
}



}
}



#include "City.h"

#include <assert.h>

#include "ConnectionPlace.h"
#include "PlaceAlias.h"
#include "PublicPlace.h"
#include "Road.h"



using synthese::lexmatcher::LexicalMatcher;


namespace synthese
{
namespace env
{

City::City (const uid& key,
	    const std::string& name)
    : synthese::util::Registrable<uid,City> (key)
    , IncludingPlace (name, 0)  // Note this city's city is null ?
{
}



City::~City ()
{

}




synthese::lexmatcher::LexicalMatcher<const ConnectionPlace*>& 
City::getConnectionPlacesMatcher ()
{
    return _connectionPlacesMatcher;
}




const synthese::lexmatcher::LexicalMatcher<const ConnectionPlace*>& 
City::getConnectionPlacesMatcher () const
{
    return _connectionPlacesMatcher;
}




synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& 
City::getPublicPlacesMatcher ()
{
    return _publicPlacesMatcher;
}




const synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& 
City::getPublicPlacesMatcher () const
{
    return _publicPlacesMatcher;
}






synthese::lexmatcher::LexicalMatcher<const Road*>& 
City::getRoadsMatcher ()
{
    return _roadsMatcher;
}




const synthese::lexmatcher::LexicalMatcher<const Road*>& 
City::getRoadsMatcher () const
{
    return _roadsMatcher;
}




synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& 
City::getPlaceAliasesMatcher ()
{
    return _placeAliasesMatcher;
}




const synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& 
City::getPlaceAliasesMatcher () const
{
    return _placeAliasesMatcher;
}








std::vector<const Road*> 
City::searchRoad (const std::string& fuzzyName, int nbMatches) const
{
    std::vector<const Road*> result;
    LexicalMatcher<const Road*>::MatchResult matches =  _roadsMatcher.bestMatches (fuzzyName, nbMatches);
    for (LexicalMatcher<const Road*>::MatchResult::iterator it = matches.begin ();
	 it != matches.end (); ++it)
    {
	result.push_back (it->value);
    }
    return result;
}



void
City::getImmediateVertices (VertexAccessMap& result, 
			    const AccessDirection& accessDirection,
			    const AccessParameters& accessParameters,
			    const Vertex* origin,
			    bool returnAddresses,
			    bool returnPhysicalStops) const
{

    if (_includedPlaces.empty ())
    {
	if (_connectionPlacesMatcher.size () > 0)
	{
	    _connectionPlacesMatcher.entries ().begin ()->second->
		getImmediateVertices (result, accessDirection, accessParameters, 
				      origin, returnAddresses, returnPhysicalStops);
	}
	else if (_placeAliasesMatcher.size () > 0)
	{
	    _placeAliasesMatcher.entries ().begin ()->second->
		getImmediateVertices (result, accessDirection, accessParameters, 
				      origin, returnAddresses, returnPhysicalStops);
	}
	else if (_publicPlacesMatcher.size () > 0)
	{
	    _publicPlacesMatcher.entries ().begin ()->second->
		getImmediateVertices (result, accessDirection, accessParameters, 
				      origin, returnAddresses, returnPhysicalStops);
	}
	else if (_roadsMatcher.size () > 0)
	{
	    _roadsMatcher.entries ().begin ()->second->
		getImmediateVertices (result, accessDirection, accessParameters, 
				      origin, returnAddresses, returnPhysicalStops);
	}
    }
    else
    {
	IncludingPlace::getImmediateVertices (result, accessDirection, accessParameters, 
					      origin, returnAddresses, returnPhysicalStops);

    }

}









}
}




#include "City.h"

#include <assert.h>


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



std::vector<const ConnectionPlace*> 
City::getMainConnectionPlaces () const
{
    std::vector<const ConnectionPlace*> result;
    const std::vector<const Place*>& includedPlaces = getIncludedPlaces ();
    if (includedPlaces.empty ())
    {
	if (_connectionPlacesMatcher.size () > 0)
	{
	    // Return the first connection place arbitrarily
	    result.push_back (_connectionPlacesMatcher.entries ().begin ()->second);
	}
    }
    else
    {
	for (std::vector<const Place*>::const_iterator it = includedPlaces.begin ();
	     it != includedPlaces.end (); ++it)
	{
	    result.push_back ((const ConnectionPlace*) *it);
	}
    }
    assert (result.size () > 0);
    return result;
    
}







}
}



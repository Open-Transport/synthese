#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H


#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "IncludingPlace.h"

#include <string>
#include <iostream>
#include <vector>


namespace synthese
{
namespace env
{

    class ConnectionPlace;
    class PlaceAlias;
    class PublicPlace;
    class Road;


/** City class.

A city holds in its included places the main connection places
 (those taken by default when no stop is explicitly chosen).

@ingroup m15
*/
class City : public synthese::util::Registrable<uid, City>, 
    public IncludingPlace
{
 private:

    synthese::lexmatcher::LexicalMatcher<const ConnectionPlace*> _connectionPlacesMatcher;
    synthese::lexmatcher::LexicalMatcher<const PublicPlace*> _publicPlacesMatcher;
    synthese::lexmatcher::LexicalMatcher<const Road*> _roadsMatcher;
    synthese::lexmatcher::LexicalMatcher<const PlaceAlias*> _placeAliasesMatcher;

 public:

    City (const uid& key,
	  const std::string& name);
    
    ~City ();


    //! @name Getters/Setters
    //@{
    synthese::lexmatcher::LexicalMatcher<const ConnectionPlace*>& getConnectionPlacesMatcher ();
    const synthese::lexmatcher::LexicalMatcher<const ConnectionPlace*>& getConnectionPlacesMatcher () const;

    synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& getPublicPlacesMatcher ();
    const synthese::lexmatcher::LexicalMatcher<const PublicPlace*>& getPublicPlacesMatcher () const;
    
    synthese::lexmatcher::LexicalMatcher<const Road*>& getRoadsMatcher ();
    const synthese::lexmatcher::LexicalMatcher<const Road*>& getRoadsMatcher () const;

    synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& getPlaceAliasesMatcher ();
    const synthese::lexmatcher::LexicalMatcher<const PlaceAlias*>& getPlaceAliasesMatcher () const;

    //@}

    //! @name Update methods
    //@{

    //@}

    //! @name Query methods
    //@{

    /** Specializes parent method fo handling the case when no main connection
     *  place was defined.
     *
     * @param accessDirection Is this place an origin or a destination.
     * @param result The shortest accesses to included places physical stops, or 
     *               one physical stop accesses of an arbitrarily chosen 
     *		     connection place of this city.
     */
    void reachPhysicalStopAccesses (const AccessDirection& accessDirection,
				    const AccessParameters& accessParameters,
				    PhysicalStopAccessMap& result) const;

    
    std::vector<const Road*> searchRoad (const std::string& fuzzyName, int nbMatches = 10) const;

    //@}


 private:


};





}
}
#endif


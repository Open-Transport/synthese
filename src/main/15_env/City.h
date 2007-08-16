
/** City class header.
	@file City.h

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

    std::string _code; //!< Unique code identifier for city within its country (france => INSEE code)

 public:

    City (const uid& key = UNKNOWN_VALUE,
	  const std::string& name = "",
	  const std::string& code = ""
	);
    
    ~City ();


    //! @name Getters/Setters
    //@{
    const std::string& getCode () const { return _code; }
    void setCode (const std::string& code) { _code = code; }

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

    void getImmediateVertices (VertexAccessMap& result, 
			       const AccessDirection& accessDirection,
			       const AccessParameters& accessParameters,
			       SearchAddresses returnAddresses
			       , SearchPhysicalStops returnPhysicalStops
				   , const Vertex* origin = NULL
	) const;
    
    std::vector<const Road*> searchRoad (const std::string& fuzzyName, int nbMatches = 10) const;

	virtual uid getId() const;
    //@}


 private:


};





}
}
#endif



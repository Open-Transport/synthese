
/** Environment class header.
	@file Environment.h

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

#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "Address.h"
#include "ConnectionPlace.h"
#include "Document.h"
#include "LineStop.h"
#include "PlaceAlias.h"
#include "PublicPlace.h"
#include "RoadChunk.h"
#include "Road.h"
#include "ReservationRule.h"
#include "TransportNetwork.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/Date.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "15_env/City.h"
#include "15_env/CommercialLine.h"


#include <vector>
#include <string>
#include <iostream>

#define TEMPS_MIN_CIRCULATIONS 'r'
#define TEMPS_MAX_CIRCULATIONS 'R'


namespace synthese
{

namespace env
{



/** 
@ingroup m15
*/
class Environment : public synthese::util::Registrable<uid,Environment>
{
 private:

    
    Address::Registry _addresses;
    City::Registry _cities;
    ConnectionPlace::Registry _connectionPlaces;
    Document::Registry _documents;
    CommercialLine::Registry _lines;
    PlaceAlias::Registry _placeAliases; 
    PublicPlace::Registry _publicPlaces;
    RoadChunk::Registry _roadChunks;
    Road::Registry _roads;
    TransportNetwork::Registry _transportNetworks;
    
    synthese::lexmatcher::LexicalMatcher<uid> _citiesMatcher;

    synthese::time::Date _minDateInUse;
    synthese::time::Date _maxDateInUse;

	std::string _name;

    // TODO : 
    // zone

 public:
	 typedef std::vector<const City*> CityList;

    Environment (const uid& id);
    ~Environment ();


    //! @name Getters/Setters
    //@{

    Address::Registry& getAddresses ();
    const Address::Registry& getAddresses () const;

    City::Registry& getCities ();
    const City::Registry& getCities () const;

    ConnectionPlace::Registry& getConnectionPlaces ();
    const ConnectionPlace::Registry& getConnectionPlaces () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    CommercialLine::Registry& getCommercialLines ();
    const CommercialLine::Registry& getCommercialLines () const;


    PlaceAlias::Registry& getPlaceAliases ();
    const PlaceAlias::Registry& getPlaceAliases () const;

    PublicPlace::Registry& getPublicPlaces ();
    const PublicPlace::Registry& getPublicPlaces () const;

    RoadChunk::Registry& getRoadChunks ();
    const RoadChunk::Registry& getRoadChunks () const;

    Road::Registry& getRoads ();
    const Road::Registry& getRoads () const;

    TransportNetwork::Registry& getTransportNetworks ();
    const TransportNetwork::Registry& getTransportNetworks () const;

    synthese::lexmatcher::LexicalMatcher<uid>& getCitiesMatcher ();
    const synthese::lexmatcher::LexicalMatcher<uid>& getCitiesMatcher () const;
    
    const synthese::time::Date& getMinDateInUse () const;
    const synthese::time::Date& getMaxDateInUse () const;

    // ...
    //@}


    //! @name Query methods
    //@{

    /** Fetches a addressable place given its id.
	All the containers storong objects typed (or subtyped) as AddressablePlace
	are inspected.
    */
    const AddressablePlace* fetchAddressablePlace (const uid& id) const;

    const IncludingPlace* fetchIncludingPlace (const uid& id) const;

    const Place* fetchPlace (const uid& id) const;
    

	/** To be moved in EnvModule */
    const Vertex* fetchVertex (const uid& id) const;
    Vertex* fetchVertex (const uid& id);


	/** Interprets date from text and environment data.
		@param text Text to interpret
		@return Interpreted date
		@author Hugues Romain
		@date 2005-2006
		@warning The parameters are not verified

		The returned date depends on the text :
			- date au format texte interne : date transcrite (no control) (ex : 20070201 => 1/2/2007)
			- commande de date classique (synthese::time::TIME_MIN ('m'), synthese::time::TIME_MAX ('M'), synthese::time::TIME_CURRENT ('A'), synthese::time::TIME_UNKNOWN ('?')) : la date correspondante (voir synthese::time::Date::setDate())
			- texte vide : identical to synthese::time::TIME_CURRENT
			- synthese::time::TIME_MIN_CIRCULATIONS ('r') : First date where at least one service runs (see Environment::getMinDateInUse())
			- TEMPS_MAX_CIRCULATIONS ('R') : Last date where at least one service runs (see Environment::getMaxDateInUse())

		The following assertion is always assumed : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
	*/	
	time::Date interpretDate( const std::string& text ) const;

    
    CityList guessCity (const std::string& fuzzyName, int nbMatches = 10) const;
    
    //@}

    //! @name Update methods
    //@{
    void updateMinMaxDatesInUse (synthese::time::Date newDate, bool marked);
    //@}
	
	const std::string& getName() const;
	void setName(const std::string& name);

};





}
}
#endif



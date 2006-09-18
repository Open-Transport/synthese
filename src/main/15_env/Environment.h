#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "Axis.h"
#include "Alarm.h"
#include "Address.h"
#include "City.h"
#include "ConnectionPlace.h"
#include "ContinuousService.h"
#include "Document.h"
#include "Fare.h"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PlaceAlias.h"
#include "PublicPlace.h"
#include "RoadChunk.h"
#include "Road.h"
#include "PedestrianCompliance.h"
#include "HandicappedCompliance.h"
#include "BikeCompliance.h"
#include "ScheduledService.h"
#include "ReservationRule.h"
#include "TransportNetwork.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "04_time/Date.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"


#include <vector>
#include <string>
#include <iostream>



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
    Alarm::Registry _alarms;
    Axis::Registry _axes;
    City::Registry _cities;
    ConnectionPlace::Registry _connectionPlaces;
    ContinuousService::Registry _continuousServices;
    Document::Registry _documents;
    Fare::Registry _fares;
    Line::Registry _lines;
    LineStop::Registry _lineStops;
    PedestrianCompliance::Registry _pedestrianCompliances;
    HandicappedCompliance::Registry _handicappedCompliances;
    BikeCompliance::Registry _bikeCompliances;
    PhysicalStop::Registry _physicalStops;
    PlaceAlias::Registry _placeAliases; 
    PublicPlace::Registry _publicPlaces;
    ReservationRule::Registry _reservationRules;
    RoadChunk::Registry _roadChunks;
    Road::Registry _roads;
    ScheduledService::Registry _scheduledServices;
    TransportNetwork::Registry _transportNetworks;
    
    synthese::lexmatcher::LexicalMatcher<uid> _citiesMatcher;

    synthese::time::Date _minDateInUse;
    synthese::time::Date _maxDateInUse;

    // TODO : 
    // zone

 public:

    Environment (const uid& id);
    ~Environment ();


    //! @name Getters/Setters
    //@{

    Address::Registry& getAddresses ();
    const Address::Registry& getAddresses () const;

    Alarm::Registry& getAlarms ();
    const Alarm::Registry& getAlarms () const;

    Axis::Registry& getAxes ();
    const Axis::Registry& getAxes () const;

    City::Registry& getCities ();
    const City::Registry& getCities () const;

    ConnectionPlace::Registry& getConnectionPlaces ();
    const ConnectionPlace::Registry& getConnectionPlaces () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    Fare::Registry& getFares ();
    const Fare::Registry& getFares () const;

    Line::Registry& getLines ();
    const Line::Registry& getLines () const;

    LineStop::Registry& getLineStops ();
    const LineStop::Registry& getLineStops () const;

    PedestrianCompliance::Registry& getPedestrianCompliances ();
    const PedestrianCompliance::Registry& getPedestrianCompliances () const;

    HandicappedCompliance::Registry& getHandicappedCompliances ();
    const HandicappedCompliance::Registry& getHandicappedCompliances () const;

    BikeCompliance::Registry& getBikeCompliances ();
    const BikeCompliance::Registry& getBikeCompliances () const;

    PlaceAlias::Registry& getPlaceAliases ();
    const PlaceAlias::Registry& getPlaceAliases () const;

    PhysicalStop::Registry& getPhysicalStops ();
    const PhysicalStop::Registry& getPhysicalStops () const;

    PublicPlace::Registry& getPublicPlaces ();
    const PublicPlace::Registry& getPublicPlaces () const;

    RoadChunk::Registry& getRoadChunks ();
    const RoadChunk::Registry& getRoadChunks () const;

    Road::Registry& getRoads ();
    const Road::Registry& getRoads () const;

    ScheduledService::Registry& getScheduledServices ();
    const ScheduledService::Registry& getScheduledServices () const;

    ContinuousService::Registry& getContinuousServices ();
    const ContinuousService::Registry& getContinuousServices () const;

    TransportNetwork::Registry& getTransportNetworks ();
    const TransportNetwork::Registry& getTransportNetworks () const;

    ReservationRule::Registry& getReservationRules ();
    const ReservationRule::Registry& getReservationRules () const;

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
    

    const Path* fetchPath (const uid& id) const;
    Path* fetchPath (const uid& id);

    
    std::vector<const City*> guessCity (const std::string& fuzzyName, int nbMatches = 10) const;
    
    //@}

    //! @name Update methods
    //@{
    void updateMinMaxDatesInUse (synthese::time::Date newDate, bool marked);
    //@}

    
 private:


};





}
}
#endif


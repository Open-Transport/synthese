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
#include "RollingStock.h"
#include "PedestrianCompliance.h"
#include "HandicappedCompliance.h"
#include "BikeCompliance.h"
#include "ScheduledService.h"
#include "TransportNetwork.h"


#include "01_util/Registrable.h"
#include "01_util/UId.h"


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
    RoadChunk::Registry _roadChunks;
    Road::Registry _roads;
    RollingStock::Registry _rollingStocks;
    ScheduledService::Registry _scheduledServices;
    TransportNetwork::Registry _transportNetworks;
    
    

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


    // ...
    //@}



 private:


};





}
}
#endif


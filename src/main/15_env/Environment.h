#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "Axis.h"
#include "Address.h"
#include "City.h"
#include "ConnectionPlace.h"
#include "ContinuousService.h"
#include "Document.h"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PlaceAlias.h"
#include "PublicPlace.h"
#include "RoadChunk.h"
#include "Road.h"
#include "RollingStock.h"
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
    Axis::Registry _axes;
    City::Registry _cities;
    ConnectionPlace::Registry _connectionPlaces;
    ContinuousService::Registry _continuousServices;
    Document::Registry _documents;
    Line::Registry _lines;
    LineStop::Registry _lineStops;
    PhysicalStop::Registry _physicalStops;
    PlaceAlias::Registry _placeAliases; 
    PublicPlace::Registry _publicPlaces;
    RoadChunk::Registry _roadChunks;
    Road::Registry _roads;
    RollingStock::Registry _rollingStocks;
    ScheduledService::Registry _scheduledServices;
    TransportNetwork::Registry _transportNetworks;
    
    

    // TODO : 
    // fare alarm
    // alarm

    // zone

 public:

    Environment (const uid& id);
    ~Environment ();


    //! @name Getters/Setters
    //@{

    Address::Registry& getAddresses ();
    const Address::Registry& getAddresses () const;

    Axis::Registry& getAxes ();
    const Axis::Registry& getAxes () const;

    City::Registry& getCities ();
    const City::Registry& getCities () const;

    ConnectionPlace::Registry& getConnectionPlaces ();
    const ConnectionPlace::Registry& getConnectionPlaces () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    Line::Registry& getLines ();
    const Line::Registry& getLines () const;

    LineStop::Registry& getLineStops ();
    const LineStop::Registry& getLineStops () const;

    PhysicalStop::Registry& getPhysicalStops ();
    const PhysicalStop::Registry& getPhysicalStops () const;

    RoadChunk::Registry& getRoadChunks ();
    const RoadChunk::Registry& getRoadChunks () const;

    Road::Registry& getRoads ();
    const Road::Registry& getRoads () const;

    ScheduledService::Registry& getScheduledServices ();
    const ScheduledService::Registry& getScheduledServices () const;

    ContinuousService::Registry& getContinuousServices ();
    const ContinuousService::Registry& getContinuousServices () const;

    // ...
    //@}



 private:


};





}
}
#endif


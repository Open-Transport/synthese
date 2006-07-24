#include "Environment.h"


namespace synthese
{
namespace env
{


Environment::Environment (const uid& id)
    : synthese::util::Registrable<uid,Environment> (id)
{
}



Environment::~Environment ()
{
    // Delete all objects registered in this environment.
    _addresses.clear ();
    _axes.clear ();
    _cities.clear ();
    _connectionPlaces.clear ();
    _documents.clear ();
    _lines.clear ();
    _lineStops.clear ();
    _physicalStops.clear ();
    _roadChunks.clear ();
    _roads.clear ();
    _continuousServices.clear ();
    _scheduledServices.clear ();
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




Alarm::Registry& 
Environment::getAlarms ()
{
    return _alarms;
}



const Alarm::Registry& 
Environment::getAlarms () const
{
    return _alarms;
}




Axis::Registry& 
Environment::getAxes ()
{
    return _axes;
}



const Axis::Registry& 
Environment::getAxes () const
{
    return _axes;
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



Fare::Registry&
Environment::getFares ()
{
    return _fares;
}


const Fare::Registry&
Environment::getFares () const
{
    return _fares;
}



Line::Registry&
Environment::getLines ()
{
    return _lines;
}


const Line::Registry&
Environment::getLines () const
{
    return _lines;
}


LineStop::Registry&
Environment::getLineStops ()
{
    return _lineStops;
}


const LineStop::Registry&
Environment::getLineStops () const
{
    return _lineStops;
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



PhysicalStop::Registry&
Environment::getPhysicalStops ()
{
    return _physicalStops;
}



PedestrianCompliance::Registry& 
Environment::getPedestrianCompliances ()
{
    return _pedestrianCompliances;
}


const PedestrianCompliance::Registry& 
Environment::getPedestrianCompliances () const
{
    return _pedestrianCompliances;
}



HandicappedCompliance::Registry& 
Environment::getHandicappedCompliances ()
{
    return _handicappedCompliances;
}



const HandicappedCompliance::Registry& 
Environment::getHandicappedCompliances () const
{
    return _handicappedCompliances;
}



BikeCompliance::Registry& 
Environment::getBikeCompliances ()
{
    return _bikeCompliances;
}



const BikeCompliance::Registry& 
Environment::getBikeCompliances () const
{
    return _bikeCompliances;
}





const PhysicalStop::Registry&
Environment::getPhysicalStops () const
{
    return _physicalStops;
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



ScheduledService::Registry& 
Environment::getScheduledServices ()
{
    return _scheduledServices;
}



const ScheduledService::Registry& 
Environment::getScheduledServices () const
{
    return _scheduledServices;
}



ContinuousService::Registry& 
Environment::getContinuousServices ()
{
    return _continuousServices;
}



const ContinuousService::Registry& 
Environment::getContinuousServices () const
{
    return _continuousServices;
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


RollingStock::Registry& 
Environment::getRollingStocks ()
{
    return _rollingStocks;
}



const RollingStock::Registry& 
Environment::getRollingStocks () const
{
    return _rollingStocks;
}



ReservationRule::Registry& 
Environment::getReservationRules ()
{
    return _reservationRules;
}



const ReservationRule::Registry& 
Environment::getReservationRules () const
{
    return _reservationRules;
}





}
}


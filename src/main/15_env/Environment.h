#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "Axis.h"
#include "Address.h"
#include "Calendar.h"
#include "City.h"
#include "Document.h"
#include "Line.h"
#include "LineStop.h"
#include "LogicalStop.h"
#include "PhysicalStop.h"
#include "RoadChunk.h"
#include "Road.h"
#include "ScheduledService.h"
#include "ContinuousService.h"


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
    Calendar::Registry _calendars;
    City::Registry _cities;
    Document::Registry _documents;
    Line::Registry _lines;
    LineStop::Registry _lineStops;
    LogicalStop::Registry _logicalStops;
    PhysicalStop::Registry _physicalStops;
    RoadChunk::Registry _roadChunks;
    Road::Registry _roads;
    ContinuousService::Registry _continuousServices;
    ScheduledService::Registry _scheduledServices;

 public:

    Environment (const uid& id);
    ~Environment ();


    //! @name Getters/Setters
    //@{

    Address::Registry& getAddresses ();
    const Address::Registry& getAddresses () const;

    Axis::Registry& getAxes ();
    const Axis::Registry& getAxes () const;

    Calendar::Registry& getCalendars ();
    const Calendar::Registry& getCalendars () const;

    City::Registry& getCities ();
    const City::Registry& getCities () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    Line::Registry& getLines ();
    const Line::Registry& getLines () const;

    LineStop::Registry& getLineStops ();
    const LineStop::Registry& getLineStops () const;

    LogicalStop::Registry& getLogicalStops ();
    const LogicalStop::Registry& getLogicalStops () const;

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


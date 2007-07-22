#include "15_env/TridentExport.h"

#include "15_env/EnvModule.h"

#include "14_geography/Projection.h"
#include "06_geometry/Point2D.h"

#include "04_time/Date.h"


#include "01_util/Conversion.h"

#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>



using namespace synthese::geography;
using namespace synthese::geometry;
using namespace synthese::time;
using namespace synthese::util;




namespace synthese
{


namespace env
{



std::string ToXsdDuration (int transferDelayMinutes)
{
    return "PT" + Conversion::ToString (transferDelayMinutes) + "M";
}

std::string ToXsdDate (const Date& date)
{
    std::stringstream ss;
    ss << std::setw( 4 ) << std::setfill ( '0' )
       << date.getYear () << "-"
       << std::setw( 2 ) << std::setfill ( '0' )
       << date.getMonth () << "-"
       << std::setw( 2 ) << std::setfill ( '0' )
       << date.getDay ();
    return ss.str ();
}



std::string ToXsdTime (const Hour& time)
{
    std::stringstream ss;
    ss << std::setw( 2 ) << std::setfill ( '0' )
       << time.getHours () << ":"
       << std::setw( 2 ) << std::setfill ( '0' )
       << time.getMinutes () << ":00";
    return ss.str ();
}


void 
TridentExport::Export (std::ostream& os, 
		       const uid& commercialLineId)
{

    os.imbue (std::locale(""));

    // os.imbue (std::locale("en_US.ISO-8859-15"));
    std::cerr << "locale = " << os.getloc ().name () << std::endl;



    os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << std::endl << std::endl;

    boost::shared_ptr<const CommercialLine> commercialLine = EnvModule::getCommercialLines ().get (commercialLineId);
    LineSet lines = EnvModule::fetchLines (commercialLineId);
    const TransportNetwork* tn = commercialLine->getNetwork ();
    

    // Collect all data related to selected commercial line

    std::set<const PhysicalStop*> physicalStops;
    std::set<const ConnectionPlace*> connectionPlaces;
    std::set<const City*> cities;
    std::set<const LineStop*> lineStops;
    ServiceSet services;

    for (LineSet::const_iterator itline = lines.begin ();
	 itline != lines.end (); ++itline)
    {
	const std::vector<Edge*>& edges = (*itline)->getEdges ();
	for (std::vector<Edge*>::const_iterator itedge = edges.begin ();
	     itedge != edges.end (); ++itedge)
	{
	    const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
	    lineStops.insert (lineStop);
	    if (lineStop->getFromVertex ()->isPhysicalStop ()) 
	    {
		const PhysicalStop* physicalStop = dynamic_cast<const PhysicalStop*> (lineStop->getFromVertex ());
		physicalStops.insert (physicalStop);

		const ConnectionPlace* connectionPlace = physicalStop->getConnectionPlace ();
		if (connectionPlace) 
		{
		    connectionPlaces.insert (connectionPlace);
		    const PhysicalStops& cpps(connectionPlace->getPhysicalStops());

		    // add also physical stops of each connection place otherwise we will
		    // lack connection links.
		    for (PhysicalStops::const_iterator itcpps = cpps.begin ();
			 itcpps != cpps.end (); ++itcpps) physicalStops.insert (*itcpps);

		    cities.insert (connectionPlace->getCity ());
		}
		
	    }    
	    
	    const ServiceSet& lservices = (*itline)->getServices ();
	    for (ServiceSet::const_iterator itsrv = lservices.begin ();
		 itsrv != lservices.end (); ++itsrv)
	    {
		services.insert ((*itsrv));
	    }
	}

    }
    
    std::string peerid ("SYNTHESE");
    
    os << "<ChouettePTNetwork xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << std::endl;


    // --------------------------------------------------- PTNetwork 
    {
	os << "<PTNetwork>" << std::endl;
	os << "<objectId>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</objectId>" << std::endl;
	os << "<name>" << tn->getName () << "</name>" << std::endl;
	os << "<lineId>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineId>" << std::endl;
	os << "<registration>" << std::endl;
	os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << std::endl;
	os << "</registration>" << std::endl;

	os << "</PTNetwork>" << std::endl;
    }

    // --------------------------------------------------- GroupOfLine


    // --------------------------------------------------- Company
    os << "<Company>" << std::endl;
    os << "<objectId>SYNTHESE:Company:1</objectId>" << std::endl;
    os << "<name>Tisséo Réseau Urbain</name>" << std::endl;
    os << "<shortName>TRU</shortName>" << std::endl;
    os << "<organisationalUnit></organisationalUnit>" << std::endl;
    os << "<operatingDepartmentName></operatingDepartmentName>" << std::endl;
    os << "<code>31000</code>" << std::endl;
    os << "<phone>0561417070</phone>" << std::endl;
    os << "<fax></fax>" << std::endl;
    os << "<email></email>" << std::endl;
    os << "<registration><registrationNumber>1</registrationNumber></registration>" << std::endl;
    os << "</Company>" << std::endl;

    os << "<ChouetteArea>" << std::endl;

    // --------------------------------------------------- StopArea (type = BoardingPosition)
    // BoardingPosition corresponds to a very accurate location along a quay for instance.
    // Not implemented right now.

    // --------------------------------------------------- StopArea (type = Quay) <=> PysicalStop
    {
	for (std::set<const PhysicalStop*>::const_iterator it = physicalStops.begin ();
	     it != physicalStops.end (); ++it)
	{
	    const PhysicalStop* ps = (*it);
	    os << "<StopArea>" << std::endl;
	    std::string stopname (ps->getName ());
	    stopname += " (" + Conversion::ToString (ps->getKey ()) + ")";
	    os << "<objectId>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</objectId>" << std::endl;
	    os << "<name>" << stopname << "</name>" << std::endl;
	    os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</centroidOfArea>" << std::endl;
	    
	    os << "<StopAreaExtension>" << std::endl;
	    os << "<areaType>" << "Quay" << "</areaType>" << std::endl;
	    os << "</StopAreaExtension>" << std::endl;
	    os << "</StopArea>" << std::endl;
	    
	}
    }
    
    // --------------------------------------------------- StopArea (type = CommercialStopPoint)
    {
	for (std::set<const ConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
	     it != connectionPlaces.end (); ++it)
	{
	    const ConnectionPlace* cp = (*it);
	    os << "<StopArea>" << std::endl;
	    os << "<objectId>" << TridentId (peerid, "StopArea", cp->getKey ()) << "</objectId>" << std::endl;
	    os << "<name>" << cp->getName () << "</name>" << std::endl;

	    // Contained physical stops
	    const std::set<const PhysicalStop*>& cpps = cp->getPhysicalStops ();
	    for (std::set<const PhysicalStop*>::const_iterator itps = cpps.begin ();
		 itps != cpps.end (); ++itps)
	    {
		// filter physical stops not concerned by this line.
		if (physicalStops.find (*itps) == physicalStops.end ()) continue;
		
		os << "<contains>" << TridentId (peerid, "StopArea", (*itps)->getKey ())  << "</contains>" << std::endl;
	    }

	    // Decide what to take for centroidOfArea of a connectionPlace. Only regarding physical stops coordinates
	    // or also regarding addresses coordinates, or fixed manually ? 
	    // os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << std::endl;
	    
	    os << "<StopAreaExtension>" << std::endl;
	    os << "<areaType>" << "CommercialStopPoint" << "</areaType>" << std::endl;
	    os << "</StopAreaExtension>" << std::endl;
	    os << "</StopArea>" << std::endl;
	}

    }

    // --------------------------------------------------- StopArea (type = StopPlace)
    // A higher level of inclusion. Mapped on cities right now... Add zones later...
    {
	for (std::set<const City*>::const_iterator it = cities.begin ();
	     it != cities.end (); ++it)
	{
	    const City* ci = (*it);
	    os << "<StopArea>" << std::endl;
	    os << "<objectId>" << TridentId (peerid, "StopArea", ci->getKey ()) << "</objectId>" << std::endl;
	    os << "<name>" << ci->getName () << "</name>" << std::endl;
	    
	    
	    // Contained connection places
	    const std::vector<const Place*>& ciip = ci->getIncludedPlaces ();
	    for (std::vector<const Place*>::const_iterator itip = ciip.begin ();
		 itip != ciip.end (); ++itip)
	    {
		const ConnectionPlace* cp = dynamic_cast<const ConnectionPlace*> (*itip);
		if (cp == 0) continue;

		// filter physical stops not concerned by this line.
		if (connectionPlaces.find (cp) == connectionPlaces.end ()) continue;

		os << "<contains>" << TridentId (peerid, "StopArea", cp->getKey ())  << "</contains>" << std::endl;
	    }
	    
	    // Decide what to take for centroidOfArea of a city. 
	    // os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << std::endl;
	    
	    os << "<StopAreaExtension>" << std::endl;
	    os << "<areaType>" << "StopPlace" << "</areaType>" << std::endl;
	    os << "</StopAreaExtension>" << std::endl;
	    os << "</StopArea>" << std::endl;
	}
    }

    
    // --------------------------------------------------- StopArea (type = ITL)
    // "Interdiction de traffic local" 
    // Not mapped right now.
    


    // --------------------------------------------------- AreaCentroid
    {
	// we do not provide addresses right now.

	for (std::set<const PhysicalStop*>::const_iterator it = physicalStops.begin ();
	     it != physicalStops.end (); ++it)
	{
	    const PhysicalStop* ps = (*it);
	    os << "<AreaCentroid>" << std::endl;
	    os << "<objectId>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</objectId>" << std::endl;
	    
	    Point2D pt (ps->getX (), ps->getY ());
	    GeoPoint gp = FromLambertIIe (pt);
	    
	    os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << std::endl;
	    os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << std::endl;
	    os << "<longLatType>" << "WGS84" << "</longLatType>" << std::endl;

	    os << "<projectedPoint>" << std::endl;
	    os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << std::endl;
	    os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << std::endl;
	    os << "<projectionType>" << "LambertIIe" << "</projectionType>" << std::endl;
	    os << "</projectedPoint>" << std::endl;
      
	    os << "</AreaCentroid>" << std::endl;
	    
	}
    }

    os << "</ChouetteArea>" << std::endl;

    // --------------------------------------------------- ConnectionLink
    {
	for (std::set<const ConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
	     it != connectionPlaces.end (); ++it)
	{
	    const ConnectionPlace* cp = (*it);
	    

	    // Contained physical stops
	    const std::set<const PhysicalStop*>& cpps = cp->getPhysicalStops ();
	    for (std::set<const PhysicalStop*>::const_iterator itps = cpps.begin ();
		 itps != cpps.end (); ++itps)
	    {
		// filter physical stops not concerned by this line.
		if (physicalStops.find (*itps) == physicalStops.end ()) continue;

		const PhysicalStop* from = (*itps);
		for (std::set<const PhysicalStop*>::const_iterator itps2 = cpps.begin ();
		     itps2 != cpps.end (); ++itps2)
		{
		    // filter physical stops not concerned by this line.
		    if (physicalStops.find (*itps2) == physicalStops.end ()) continue;
		    
		    const PhysicalStop* to = (*itps2);
		    
		    os << "<ConnectionLink>" << std::endl;
		    std::stringstream clkey;
		    clkey << from->getKey () << "_" << to->getKey ();
		    os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << std::endl;
		    os << "<startOfLink>" << TridentId (peerid, "StopArea", from->getKey ()) << "</startOfLink>" << std::endl;
		    os << "<endOfLink>" << TridentId (peerid, "StopArea", from->getKey ()) << "</endOfLink>" << std::endl;
		    os << "<defaultDuration>" << ToXsdDuration (cp->getTransferDelay (from, to)) << "</defaultDuration>" << std::endl;
		    os << "</ConnectionLink>" << std::endl;
		}

	    }
	}

    }
    
    // --------------------------------------------------- Timetable
    {
	for (ServiceSet::const_iterator it = services.begin ();
	     it != services.end (); ++it)
	{
	    // One timetable per service
	    const ScheduledService* srv(dynamic_cast<const ScheduledService*>(*it));
		if (srv == NULL)
			continue;

	    const Calendar& cal = srv->getCalendar ();
	    
	    os << "<Timetable>" << std::endl;
	    os << "<objectId>" << TridentId (peerid, "Timetable", srv) << "</objectId>" << std::endl;
	    
	    std::vector<Date> markedDates = cal.getMarkedDates ();
	    for (int d=0; d<markedDates.size (); ++d)
	    {
		os << "<calendarDay>" << ToXsdDate (markedDates[d]) << "</calendarDay>" << std::endl;
	    }
	    os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv) << "</vehicleJourneyId>" << std::endl;

	    os << "</Timetable>" << std::endl;
	    
	}

    }

    // --------------------------------------------------- TimeSlot
    {
    }

    // --------------------------------------------------- ChouetteLineDescription
    {
	os << "<ChouetteLineDescription>" << std::endl;
	
	// --------------------------------------------------- Line
	{
	    os << "<Line>" << std::endl;
	    os << "<objectId>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</objectId>" << std::endl;
	    os << "<name>" << commercialLine->getName () << "</name>" << std::endl;
	    os << "<publishedName>" << commercialLine->getLongName () << "</publishedName>" << std::endl;
	    
	    for (LineSet::const_iterator itline = lines.begin ();
		 itline != lines.end (); ++itline)
	    {
		os << "<routeId>" << TridentId (peerid, "ChouetteRoute", (*itline)->getKey ()) << "</routeId>" << std::endl;
	    }
	    os << "<registration>" << std::endl;
	    os << "<registrationNumber>" << Conversion::ToString (commercialLine->getKey ()) << "</registrationNumber>" << std::endl;
	    os << "</registration>" << std::endl;

	    os << "</Line>" << std::endl;
	}

	// --------------------------------------------------- ChouetteRoute
	{
	    for (LineSet::const_iterator itline = lines.begin ();
		 itline != lines.end (); ++itline)
	    {
		boost::shared_ptr<const Line> line = (*itline);
		os << "<ChouetteRoute>" << std::endl;
		os << "<objectId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</objectId>" << std::endl;
		os << "<name>" << line->getName () << "</name>" << std::endl;
		os << "<publishedName>" << line->getTimetableName () << "</publishedName>" << std::endl;
		
		const std::vector<Edge*>& edges = line->getEdges ();
		for (std::vector<Edge*>::const_iterator itedge = edges.begin ();
		     itedge != edges.end (); ++itedge)
		{
		    const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
		    if (lineStop->getNextInPath () == 0) continue;
		    os << "<ptLinkId>" << TridentId (peerid, "PtLink", lineStop->getKey ()) << "</ptLinkId>" << std::endl;
		}
		os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << std::endl;
		os << "</ChouetteRoute>" << std::endl;
	    }
	}

	// --------------------------------------------------- StopPoint
	{
	    for (std::set<const LineStop*>::const_iterator itls = lineStops.begin ();
		 itls != lineStops.end (); ++itls)
	    {
		const LineStop* ls = (*itls);
		const PhysicalStop* ps = (const PhysicalStop*) ls->getFromVertex ();

		os << "<StopPoint>" << std::endl;
		os << "<objectId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</objectId>" << std::endl;

		Point2D pt (ps->getX (), ps->getY ());
		GeoPoint gp = FromLambertIIe (pt);
		
		os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << std::endl;
		os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << std::endl;
		os << "<longLatType>" << "WGS84" << "</longLatType>" << std::endl;
		
		os << "<projectedPoint>" << std::endl;
		os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << std::endl;
		os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << std::endl;
		os << "<projectionType>" << "LambertIIe" << "</projectionType>" << std::endl;
		os << "</projectedPoint>" << std::endl;


		os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << std::endl;
		os << "<name>" << ps->getName () << "</name>" << std::endl;

		os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << std::endl;
		os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</ptNetworkIdShortcut>" << std::endl;

		os << "</StopPoint>" << std::endl;

	    }

	}
	
	// --------------------------------------------------- ITL
	// Not implemented
	{

	}


	// --------------------------------------------------- PtLink
	{
	    for (std::set<const LineStop*>::const_iterator itls = lineStops.begin ();
		 itls != lineStops.end (); ++itls)
	    {
		const LineStop* from = (*itls);
		const LineStop* to = dynamic_cast<const LineStop*> ((*itls)->getNextInPath ());
		if (to == 0) continue;

		const PhysicalStop* ps = (const PhysicalStop*) from->getFromVertex ();

		os << "<PtLink>" << std::endl;
		os << "<objectId>" << TridentId (peerid, "PtLink", from->getKey ()) << "</objectId>" << std::endl;
		os << "<startOfLink>" << TridentId (peerid, "StopPoint", from->getKey ()) << "</startOfLink>" << std::endl;
		os << "<endOfLink>" << TridentId (peerid, "StopPoint", to->getKey ()) << "</endOfLink>" << std::endl;
		os << "<linkDistance>" << Conversion::ToString (from->getLength ()) << "</linkDistance>" << std::endl;   // in meters!
		os << "</PtLink>" << std::endl;
	    }
	}

	// --------------------------------------------------- JourneyPattern
	{
	    // One per route 
	    for (LineSet::const_iterator itline = lines.begin ();
		 itline != lines.end (); ++itline)
	    {
		boost::shared_ptr<const Line> line = (*itline);
		os << "<JourneyPattern>" << std::endl;
		os << "<objectId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</objectId>" << std::endl;
		os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << std::endl;

		const std::vector<Edge*>& edges = line->getEdges ();
		os << "<origin>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (0))->getKey ()) << "</origin>" << std::endl;
		os << "<destination>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (edges.size ()-1))->getKey ()) << "</destination>" << std::endl;

		for (std::vector<Edge*>::const_iterator itedge = edges.begin ();
		     itedge != edges.end (); ++itedge)
		{
		    const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
		    os << "<stopPointList>" << TridentId (peerid, "StopPoint", lineStop->getKey ()) << "</stopPointList>" << std::endl;
		}

		os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << std::endl;
		os << "</JourneyPattern>" << std::endl;
	    }
	}

	// --------------------------------------------------- VehicleJourney
	{
	    for (ServiceSet::const_iterator it = services.begin ();
		 it != services.end (); ++it)
	    {
		// One timetable per service
		const ScheduledService* srv(dynamic_cast<const ScheduledService*>(*it));
		if (srv == NULL)
			continue;

		const Line* line = ((const Line*) srv->getPath ());
		const Calendar& cal = srv->getCalendar ();
		
		os << "<VehicleJourney>" << std::endl;
		os << "<objectId>" << TridentId (peerid, "VehicleJourney", srv) << "</objectId>" << std::endl;
		os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << std::endl;
		os << "<journeyPatternId>" << TridentId (peerid, "JourneyPatternId", line->getKey ()) << "</journeyPatternId>" << std::endl;
		os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << std::endl;
		os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeIdShortcut>" << std::endl;
		
		os << "<number>" << srv->getServiceNumber () << "</number>" << std::endl;
		os << "<comment>" << TridentId (peerid, "VehicleJourney", srv) << "</comment>" << std::endl;

		// --------------------------------------------------- VehicleJourneyAtStop
		{
		    const std::vector<Edge*>& edges = line->getEdges ();
		    
		    for (int e=0; e<edges.size (); ++e)
		    {
			const LineStop* ls = (const LineStop*) edges.at (e);
			
			os << "<vehicleJourneyAtStop>" << std::endl;
			os << "<stopPointId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</stopPointId>" << std::endl;
			os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv) << "</vehicleJourneyId>" << std::endl;

			if (e != 0)
			    os << "<arrivalTime>" << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls).getHour ()) 
			       << "</arrivalTime>" << std::endl;
			if (e != edges.size () - 1)
			    os << "<departureTime>" << ToXsdTime (srv->getDepartureBeginScheduleToIndex (ls).getHour ()) 
			       << "</departureTime>" << std::endl;


			os << "</vehicleJourneyAtStop>" << std::endl;
		    }
		}

		os << "</VehicleJourney>" << std::endl;
		
	    }
	}


	os << "</ChouetteLineDescription>" << std::endl;
    }

    os << "</ChouettePTNetwork>" <<  std::endl << std::flush;

}




std::string 
TridentExport::TridentId (const std::string& peer, const std::string clazz, const uid& id)
{
    std::stringstream ss;
    ss << peer << ":" << clazz << ":" << id;
    return ss.str ();
}

std::string 
TridentExport::TridentId (const std::string& peer, const std::string clazz, const std::string& s)
{
    std::stringstream ss;
    ss << peer << ":" << clazz << ":" << s;
    return ss.str ();
}



std::string 
TridentExport::TridentId (const std::string& peer,  const std::string clazz, const Service* service)
{
    std::stringstream ss;
    ss << peer << ":" << clazz << ":" << ((const Line*) service->getPath ())->getKey () 
       << "_" << service->getServiceNumber () ;

    return ss.str ();
}







}
}

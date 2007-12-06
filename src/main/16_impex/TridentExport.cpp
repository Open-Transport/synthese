
/** tridentexport class implementation.
	@file tridentexport.cpp

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


#include "TridentExport.h"

#include "15_env/EnvModule.h"
#include "15_env/CommercialLine.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/ScheduledService.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"
#include "15_env/TransportNetwork.h"
#include "15_env/City.h"
#include "15_env/Service.h"
#include "15_env/RollingStock.h"

#include "14_geography/Projection.h"
#include "06_geometry/Point2D.h"

#include "04_time/Date.h"
#include "04_time/Hour.h"

#include "01_util/Conversion.h"

#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>

namespace synthese
{

	using namespace geography;
	using namespace geometry;
	using namespace time;
	using namespace env;
	using namespace util;

	namespace impex
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

			boost::shared_ptr<const CommercialLine> commercialLine = CommercialLine::Get (commercialLineId);
			LineSet lines = EnvModule::fetchLines (commercialLineId);
			const TransportNetwork* tn = commercialLine->getNetwork ();
		    

			// Collect all data related to selected commercial line

			std::set<const PhysicalStop*> physicalStops;
			std::set<const PublicTransportStopZoneConnectionPlace*> connectionPlaces;
			std::set<const City*> cities;
			std::set<const LineStop*> lineStops;
			const RollingStock* rollingStock (0);
			ServiceSet services;

			std::set<const PhysicalStop*> filteredPhysicalStops; // if no departing/arriving edge

			for (LineSet::const_iterator itline = lines.begin ();
			 itline != lines.end (); ++itline)
			{
			    rollingStock = (*itline)->getRollingStock ();

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

					const PublicTransportStopZoneConnectionPlace* connectionPlace(physicalStop->getConnectionPlace ());
					if (connectionPlace) 
					{
						connectionPlaces.insert (connectionPlace);
						const PhysicalStops& cpps(connectionPlace->getPhysicalStops());

						// add also physical stops of each connection place otherwise we will
						// lack connection links.
						for (PhysicalStops::const_iterator itcpps = cpps.begin ();
						 itcpps != cpps.end (); ++itcpps) 
						{
						    // Skip physical stops which have no departing or arriving edges.
						    if ((itcpps->second->getDepartureEdges ().size () == 0) && 
							(itcpps->second->getArrivalEdges ().size () == 0)) continue;
						    
						    physicalStops.insert (itcpps->second);
						}

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
		    
			os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << std::endl;


			// --------------------------------------------------- PTNetwork 
			{
			os << "<PTNetwork>" << std::endl;
			os << "<objectId>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</objectId>" << std::endl;
			os << "<versionDate>" << ToXsdDate (Date (TIME_CURRENT)) << "</versionDate>" << std::endl;
			os << "<name>" << tn->getName () << "</name>" << std::endl;
			os << "<registration>" << std::endl;
			os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << std::endl;
			os << "</registration>" << std::endl;
			os << "<lineId>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineId>" << std::endl;
			os << "<comment/>" << std::endl;
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

			// --------------------------------------------------- StopArea (type = Quay) <=> PhysicalStop
			{
			for (std::set<const PhysicalStop*>::const_iterator it = physicalStops.begin ();
				 it != physicalStops.end (); ++it)
			{

				const PhysicalStop* ps = (*it);
				if ((ps->getDepartureEdges ().size () == 0) && (ps->getArrivalEdges ().size () == 0)) continue;

				os << "<StopArea>" << std::endl;
				// old : std::string stopname (ps->getName ());
				// old : stopname += " (" + Conversion::ToString (ps->getKey ()) + ")";

				os << "<objectId>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</objectId>" << std::endl;

				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
				    ps->getConnectionPlace ()->getName ();
				if (ps->getName ().empty () == false) os << " (" + ps->getName () + ")";
				os << " / " << ps->getOperatorCode ();
				os << "</name>" << std::endl;

				// Add all stop points referencing this physical stop
                                // Otherly said : all line stops based on this physical stop. highly redundant since the other link exists.
				const Vertex::Edges& departingLineStops = ps->getDepartureEdges ();
				for (Vertex::Edges::const_iterator itls = departingLineStops.begin ();
				     itls != departingLineStops.end (); ++itls) {
				    const LineStop* ls = dynamic_cast<const LineStop*> (*itls);
				    os << "<contains>" << TridentId (peerid, "StopPoint", ls->getKey ())  << "</contains>" << std::endl;
				}
				const Vertex::Edges& arrivingLineStops = ps->getArrivalEdges ();
				for (Vertex::Edges::const_iterator itls = arrivingLineStops.begin ();
				     itls != arrivingLineStops.end (); ++itls) {
				    const LineStop* ls = dynamic_cast<const LineStop*> (*itls);
				    os << "<contains>" << TridentId (peerid, "StopPoint", ls->getKey ())  << "</contains>" << std::endl;
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</centroidOfArea>" << std::endl;
				os << "<StopAreaExtension>" << std::endl;
				os << "<areaType>" << "Quay" << "</areaType>" << std::endl;
				os << "<registration><registrationNumber>" << ps->getOperatorCode () << "</registrationNumber></registration>" << std::endl;
				os << "</StopAreaExtension>" << std::endl;
				os << "</StopArea>" << std::endl;
			    
			}
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			{
			for (std::set<const PublicTransportStopZoneConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
				 it != connectionPlaces.end (); ++it)
			{
				const PublicTransportStopZoneConnectionPlace* cp = (*it);

				if (cp->getPhysicalStops ().size () == 0) continue;

				os << "<StopArea>" << std::endl;
				os << "<objectId>" << TridentId (peerid, "StopArea", cp->getKey ()) << "</objectId>" << std::endl;
				os << "<name>" << cp->getCity ()->getName () << " " << cp->getName () << "</name>" << std::endl;

				// Contained physical stops
				const PhysicalStops& cpps = cp->getPhysicalStops ();
				for (PhysicalStops::const_iterator itps = cpps.begin ();
				 itps != cpps.end (); ++itps)
				{
				// filter physical stops not concerned by this line.
				if (physicalStops.find (itps->second) == physicalStops.end ()) continue;
				
				os << "<contains>" << TridentId (peerid, "StopArea", itps->first)  << "</contains>" << std::endl;
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
				std::vector<std::string> containedStopAreas;

				// Contained connection places
				const std::vector<const Place*>& ciip = ci->getIncludedPlaces ();
				for (std::vector<const Place*>::const_iterator itip = ciip.begin ();
				 itip != ciip.end (); ++itip)
				{
				const PublicTransportStopZoneConnectionPlace* cp = dynamic_cast<const PublicTransportStopZoneConnectionPlace*> (*itip);
				if (cp == 0) continue;

				// filter physical stops not concerned by this line.
				if (connectionPlaces.find (cp) == connectionPlaces.end ()) continue;

				containedStopAreas.push_back (TridentId (peerid, "StopArea", cp->getKey ()));

				}
				if (containedStopAreas.size () == 0) continue;


				os << "<StopArea>" << std::endl;
				os << "<objectId>" << TridentId (peerid, "StopArea", ci->getKey ()) << "</objectId>" << std::endl;
				os << "<name>" << ci->getName () << "</name>" << std::endl;

				
				for (std::vector<std::string>::const_iterator itsa = containedStopAreas.begin ();
				     itsa != containedStopAreas.end (); ++itsa) {
				    os << "<contains>" << (*itsa)  << "</contains>" << std::endl;
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

				os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << std::endl;
				os << "<name/>" << std::endl;

				os << "</AreaCentroid>" << std::endl;
			    
			}
			}

			os << "</ChouetteArea>" << std::endl;

			// --------------------------------------------------- ConnectionLink
			{
			for (std::set<const PublicTransportStopZoneConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
				 it != connectionPlaces.end (); ++it)
			{
				const PublicTransportStopZoneConnectionPlace* cp = (*it);
			    

				// Contained physical stops
				const PhysicalStops& cpps = cp->getPhysicalStops ();
				for (PhysicalStops::const_iterator itps = cpps.begin ();
				 itps != cpps.end (); ++itps)
				{
				// filter physical stops not concerned by this line.
				if (physicalStops.find (itps->second) == physicalStops.end ()) continue;

				const PhysicalStop* from = (itps->second);
				for (PhysicalStops::const_iterator itps2 = cpps.begin ();
					 itps2 != cpps.end (); ++itps2)
				{
					// filter physical stops not concerned by this line.
					if (physicalStops.find (itps2->second) == physicalStops.end ()) continue;
				    
					const PhysicalStop* to = (itps2->second);
				    
					os << "<ConnectionLink>" << std::endl;
					std::stringstream clkey;
					clkey << from->getKey () << "t" << to->getKey ();
					os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << std::endl;
					os << "<startOfLink>" << TridentId (peerid, "StopArea", from->getKey ()) << "</startOfLink>" << std::endl;
					os << "<endOfLink>" << TridentId (peerid, "StopArea", to->getKey ()) << "</endOfLink>" << std::endl;
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
				os << "<number>" << commercialLine->getShortName () << "</number>" << std::endl;
				os << "<publishedName>" << commercialLine->getLongName () << "</publishedName>" << std::endl;
				
				std::string tm ("");
				if (rollingStock != 0)
				{
				    switch (rollingStock->getKey ())
				    {
				    case 13792273858822157LL : tm = "RapidTransit"; break;  // train Eurostar
				    case 13792273858822158LL : tm = "RapidTransit"; break;  // train intercity
				    case 13792273858822159LL : tm = "LocalTrain"; break;  // train de banlieue
				    case 13792273858822160LL : tm = "LongDistanceTrain"; break;  // train de Grandes Lignes
				    case 13792273858822583LL : tm = "LocalTrain"; break;  // bus scolaire
				    case 13792273858822584LL : tm = "Coach"; break;  // autocar
				    case 13792273858822585LL : tm = "Bus"; break;  // bus
				    case 13792273858822586LL : tm = "Metro"; break;  // metro
				    case 13792273858822587LL : tm = "Train"; break;  // train regional
				    case 13792273858822588LL : tm = "Tramway"; break;  // tramway
				    case 13792273858822589LL : tm = "Other"; break;  // transport a la demande
				    case 13792273858822590LL : tm = "RapidTransit"; break;  // train a grande vitesse
				    case 13792273858822591LL : tm = "Other"; break;  // telecabine
				    case 13792273858822594LL : tm = "Bus"; break;  // ligne de bus speciale
				    case 13792273858822638LL : tm = "LongDistanceTrain"; break;  // train de nuit
				    case 13792273859967672LL : tm = "LongDistanceTrain"; break;  // train de nuit Corail Lunea
				    case 13792273859967678LL : tm = "LongDistanceTrain"; break;  // train grandes lignes Corail Teoz
				    default: tm = "Other"; 
				    }
				}
				os << "<transportModeName>" << tm << "</transportModeName>" << std::endl;
			    
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
				os << "<publishedName>";
				{
				    const PhysicalStop* ps = line->getOrigin ();
				    os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << " -&gt; ";
				{
				    const PhysicalStop* ps = line->getDestination ();
				    os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << "</publishedName>" << std::endl;
				
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
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << std::endl;
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << std::endl;
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeIdShortcut>" << std::endl;
				
				os << "<number>" << Conversion::ToString (srv->getServiceNumber ()) << "</number>" << std::endl;
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
				os << "<comment>" << TridentId (peerid, "VehicleJourney", srv) << "</comment>" << std::endl;

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
		TridentExport::TridentId (const std::string& peer,  const std::string clazz, const synthese::env::Service* service)
		{
			std::stringstream ss;
			int skey = service->getServiceNumber ();
			if (skey == 0) skey = service->getId ();

			ss << peer << ":" << clazz << ":" << ((const Line*) service->getPath ())->getKey () 
			   << "s" << skey ;

			return ss.str ();
		}







	}
}

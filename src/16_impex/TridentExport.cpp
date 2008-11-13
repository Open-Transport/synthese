
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

// 36 Impex
#include "TridentExport.h"

// 35 Env
#include "EnvModule.h"
#include "CommercialLine.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "Line.h"
#include "SubLine.h"
#include "LineStop.h"
#include "LineStopTableSync.h"
#include "TransportNetwork.h"
#include "City.h"
#include "Service.h"
#include "RollingStock.h"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationRule.h"
#include "ReservationRuleTableSync.h"
#include "HandicappedCompliance.h"
#include "BikeCompliance.h"

// 06 Geometry
#include "Projection.h"
#include "Point2D.h"

// 04 Time
#include "Date.h"
#include "Hour.h"

// 01 Util
#include "Conversion.h"

// Std
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
	using namespace geometry;
	using namespace time;
	using namespace env;
	using namespace util;

	namespace impex
	{

		string ToXsdDaysDuration (int daysDelay)
		{
			stringstream s;
			s << "P" << daysDelay << "D";
			return s.str();
		}


		string ToXsdDuration (int transferDelayMinutes)
		{
			stringstream s;
			s << "PT";
			if (transferDelayMinutes > 59)
				s << (transferDelayMinutes / 60) << "H";
			s << (transferDelayMinutes % 60) << "M";
			return s.str();
		}

		string ToXsdDate (const Date& date)
		{
			stringstream ss;
			ss << setw( 4 ) << setfill ( '0' )
			   << date.getYear () << "-"
			   << setw( 2 ) << setfill ( '0' )
			   << date.getMonth () << "-"
			   << setw( 2 ) << setfill ( '0' )
			   << date.getDay ();
			return ss.str ();
		}



		string ToXsdTime (const Hour& time)
		{
			stringstream ss;
			ss << setw( 2 ) << setfill ( '0' )
			   << time.getHours () << ":"
			   << setw( 2 ) << setfill ( '0' )
			   << time.getMinutes () << ":00";
			return ss.str ();
		}


		void TridentExport::Export (
			ostream& os
			, const uid& commercialLineId
			, bool withTisseoExtensions
		){

			static const string peerid ("SYNTHESE");

			os.imbue (locale(""));

			// os.imbue (locale("en_US.ISO-8859-15"));
			cerr << "locale = " << os.getloc ().name () << endl;



			os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << endl << endl;

			shared_ptr<const CommercialLine> commercialLine = CommercialLine::Get (commercialLineId);
			LineSet lines = EnvModule::fetchLines (commercialLineId);
			const TransportNetwork* tn = commercialLine->getNetwork ();
		    

			// Collect all data related to selected commercial line



			// Physical stops ------------------------------------------------------------------------

			set<const PhysicalStop*> physicalStops;
			set<const PublicTransportStopZoneConnectionPlace*> connectionPlaces;
			set<const City*> cities;
			set<const LineStop*> lineStops;
			const RollingStock* rollingStock (0);
			ServiceSet services;

			set<const PhysicalStop*> filteredPhysicalStops; // if no departing/arriving edge

			for(LineSet::const_iterator itline = lines.begin ();
				itline != lines.end ();
				++itline
			){
			    rollingStock = (*itline)->getRollingStock ();

			const vector<Edge*>& edges = (*itline)->getEdges ();
			for (vector<Edge*>::const_iterator itedge = edges.begin ();
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

						cities.insert (connectionPlace->getCity ());
					}
					
				}    
			    
				const ServiceSet& lservices = (*itline)->getServices ();
				for (ServiceSet::const_iterator itsrv = lservices.begin ();
				 itsrv != lservices.end (); ++itsrv)
				{
				services.insert ((*itsrv));
				}

				// Sub-lines : will be useless when the trident export will work with database rows instead of objects in ram
				const Line::SubLines& sublines((*itline)->getSubLines());
				for (Line::SubLines::const_iterator itsubline(sublines.begin()); itsubline != sublines.end(); ++itsubline)
				{
					const ServiceSet& lservices((*itsubline)->getServices ());
					for (ServiceSet::const_iterator itsrv = lservices.begin ();	itsrv != lservices.end (); ++itsrv)
						services.insert(*itsrv);
				}
			}

			}
		    
		    if (withTisseoExtensions)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident http://www.reseaux-conseil.com/trident/tisseo-chouette-extension.xsd'>" << endl;
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << endl;

			// --------------------------------------------------- PTNetwork 
			{
			os << "<PTNetwork>" << endl;
			os << "<objectId>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</objectId>" << endl;
			os << "<versionDate>" << ToXsdDate (Date (TIME_CURRENT)) << "</versionDate>" << endl;
			os << "<name>" << tn->getName () << "</name>" << endl;
			os << "<registration>" << endl;
			os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << endl;
			os << "</registration>" << endl;
			os << "<lineId>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineId>" << endl;
			os << "<comment/>" << endl;
			os << "</PTNetwork>" << endl;
			}

			// --------------------------------------------------- GroupOfLine


			// --------------------------------------------------- Company
			os << "<Company>" << endl;
			os << "<objectId>SYNTHESE:Company:1</objectId>" << endl;
			os << "<name>Tisséo Réseau Urbain</name>" << endl;
			os << "<shortName>TRU</shortName>" << endl;
			os << "<organisationalUnit></organisationalUnit>" << endl;
			os << "<operatingDepartmentName></operatingDepartmentName>" << endl;
			os << "<code>31000</code>" << endl;
			os << "<phone>0561417070</phone>" << endl;
			os << "<fax></fax>" << endl;
			os << "<email></email>" << endl;
			os << "<registration><registrationNumber>1</registrationNumber></registration>" << endl;
			os << "</Company>" << endl;

			os << "<ChouetteArea>" << endl;

			// --------------------------------------------------- StopArea (type = BoardingPosition)
			// BoardingPosition corresponds to a very accurate location along a quay for instance.
			// Not implemented right now.

			// --------------------------------------------------- StopArea (type = Quay) <=> PhysicalStop
			{
			for (set<const PhysicalStop*>::const_iterator it = physicalStops.begin ();
				 it != physicalStops.end (); ++it)
			{

				const PhysicalStop* ps = (*it);
				if ((ps->getDepartureEdges ().size () == 0) && (ps->getArrivalEdges ().size () == 0)) continue;

				os << "<StopArea>" << endl;

				os << "<objectId>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</objectId>" << endl;
				os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << endl;

				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
				    ps->getConnectionPlace ()->getName ();
				if (!ps->getName().empty()) os << " (" + ps->getName () + ")";
				os << "</name>" << endl;

				vector<shared_ptr<LineStop> > lstops(LineStopTableSync::Search(
					UNKNOWN_VALUE,
					ps->getKey()
				));
				for (vector<shared_ptr<LineStop> >::const_iterator itls = lstops.begin ();
				     itls != lstops.end (); ++itls)
				{
				    os << "<contains>" << TridentId (peerid, "StopPoint", (*itls)->getKey ())  << "</contains>" << endl;
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</centroidOfArea>" << endl;
				os << "<StopAreaExtension>" << endl;
				os << "<areaType>" << "Quay" << "</areaType>" << endl;
				string rn = ps->getOperatorCode ();
				if (rn.empty ()) rn = "0";
				os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << endl;
				os << "</StopAreaExtension>" << endl;
				os << "</StopArea>" << endl;
			    
			}
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			{
			for (set<const PublicTransportStopZoneConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
				 it != connectionPlaces.end (); ++it)
			{
				const PublicTransportStopZoneConnectionPlace* cp = (*it);

				if (cp->getPhysicalStops ().size () == 0) continue;

				os << "<StopArea>" << endl;
				os << "<objectId>" << TridentId (peerid, "StopArea", cp->getKey ()) << "</objectId>" << endl;
				os << "<name>" << cp->getCity ()->getName () << " " << cp->getName () << "</name>" << endl;

				// Contained physical stops
				const PhysicalStops& cpps = cp->getPhysicalStops ();
				for (PhysicalStops::const_iterator itps = cpps.begin ();
				 itps != cpps.end (); ++itps)
				{
				// filter physical stops not concerned by this line.
				if (physicalStops.find (itps->second) == physicalStops.end ()) continue;
				
				os << "<contains>" << TridentId (peerid, "StopArea", itps->first)  << "</contains>" << endl;
				}

				// Decide what to take for centroidOfArea of a connectionPlace. Only regarding physical stops coordinates
				// or also regarding addresses coordinates, or fixed manually ? 
				// os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << endl;
			    
				os << "<StopAreaExtension>" << endl;
				os << "<areaType>" << "CommercialStopPoint" << "</areaType>" << endl;
				os << "</StopAreaExtension>" << endl;
				os << "</StopArea>" << endl;
			}

			}

		    
			// --------------------------------------------------- StopArea (type = ITL)
			// "Interdiction de traffic local" 
			// Not mapped right now.
		    


			// --------------------------------------------------- AreaCentroid
			{
				// we do not provide addresses right now.

				for (set<const PhysicalStop*>::const_iterator it = physicalStops.begin ();
					 it != physicalStops.end (); ++it)
				{
					const PhysicalStop* ps = (*it);
					os << "<AreaCentroid>" << endl;
					os << "<objectId>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</objectId>" << endl;
				    
					Point2D pt (ps->getX (), ps->getY ());
					GeoPoint gp = FromLambertIIe (pt);
				    
					os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << endl;
					os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << endl;
					os << "<longLatType>" << "WGS84" << "</longLatType>" << endl;

					os << "<projectedPoint>" << endl;
					os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << endl;
					os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << endl;
					os << "<projectionType>" << "LambertIIe" << "</projectionType>" << endl;
					os << "</projectedPoint>" << endl;

					os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << endl;
					os << "<name>" << Conversion::ToString (ps->getKey ()) << "</name>" << endl;

					os << "</AreaCentroid>" << endl;
				    
				}
			}

			os << "</ChouetteArea>" << endl;

			// --------------------------------------------------- ConnectionLink
			{
				for (set<const PublicTransportStopZoneConnectionPlace*>::const_iterator it = connectionPlaces.begin ();
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

							//if (physicalStops.find (itps2->second) == physicalStops.end ()) continue;
						    
							const PhysicalStop* to = (itps2->second);
						    
							os << "<ConnectionLink>" << endl;
							stringstream clkey;
							clkey << from->getKey () << "t" << to->getKey ();
							os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << endl;
							os << "<startOfLink>" << TridentId (peerid, "StopArea", from->getKey ()) << "</startOfLink>" << endl;
							os << "<endOfLink>" << TridentId (peerid, "StopArea", to->getKey ()) << "</endOfLink>" << endl;
							os << "<defaultDuration>" << ToXsdDuration (cp->getTransferDelay (from, to)) << "</defaultDuration>" << endl;
							os << "</ConnectionLink>" << endl;
						}

					}
				}
			}
		    
			// --------------------------------------------------- Timetable
			{
				for (ServiceSet::const_iterator it = services.begin ();
					 it != services.end (); ++it
				){
					// One timetable per service
					const ScheduledService* srv(dynamic_cast<const ScheduledService*>(*it));
					if (srv == NULL)
						continue;

					const Calendar& cal = srv->getCalendar ();
				    
					os << "<Timetable>" << endl;
					os << "<objectId>" << TridentId (peerid, "Timetable", srv) << "</objectId>" << endl;
				    
					vector<Date> markedDates = cal.getMarkedDates ();
					for (int d=0; d<markedDates.size (); ++d)
					{
					os << "<calendarDay>" << ToXsdDate (markedDates[d]) << "</calendarDay>" << endl;
					}
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv) << "</vehicleJourneyId>" << endl;

					os << "</Timetable>" << endl;
				}
			}


			// --------------------------------------------------- TimeSlot
			{
				for(ServiceSet::const_iterator it = services.begin ();
					it != services.end ();
					++it
				){
					const ContinuousService* csrv(dynamic_cast<const ContinuousService*>(*it));
					if (csrv == NULL) continue;

					const NonPermanentService* srv(dynamic_cast<const NonPermanentService*>(*it));
					string timeSlotId;
					timeSlotId = TridentId(peerid, "TimeSlot", srv);

					os << "<TimeSlot>" << endl;
					os << "<objectId>" << timeSlotId << "</objectId>" << endl;
					os << "<beginningSlotTime>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</beginningSlotTime>" << endl;
					os << "<endSlotTime>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</endSlotTime>" << endl;
					os << "<firstDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</firstDepartureTimeInSlot>" << endl;
					os << "<lastDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</lastDepartureTimeInSlot>" << endl;
					os << "</TimeSlot>" << endl;
				}
			}


			// --------------------------------------------------- ChouetteLineDescription
			{
			os << "<ChouetteLineDescription>" << endl;
			
			// --------------------------------------------------- Line
			{
				os << "<Line>" << endl;
				os << "<objectId>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</objectId>" << endl;
				os << "<name>" << commercialLine->getName () << "</name>" << endl;
				os << "<number>" << commercialLine->getShortName () << "</number>" << endl;
				os << "<publishedName>" << commercialLine->getLongName () << "</publishedName>" << endl;
				
				string tm ("");
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
				os << "<transportModeName>" << tm << "</transportModeName>" << endl;
			    
				for (LineSet::const_iterator itline = lines.begin ();
				 itline != lines.end (); ++itline)
				{
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", (*itline)->getKey ()) << "</routeId>" << endl;
				}
				os << "<registration>" << endl;
				os << "<registrationNumber>" << Conversion::ToString (commercialLine->getKey ()) << "</registrationNumber>" << endl;
				os << "</registration>" << endl;

				os << "</Line>" << endl;
			}

			// --------------------------------------------------- ChouetteRoute
			{
				for (LineSet::const_iterator itline = lines.begin ();
				 itline != lines.end (); ++itline)
				{
					shared_ptr<const Line> line = (*itline);
					os << "<ChouetteRoute>" << endl;
					os << "<objectId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</objectId>" << endl;
					os << "<name>" << line->getName () << "</name>" << endl;
					os << "<publishedName>";
					{
						const PhysicalStop* ps = line->getOrigin ();
						if (ps)
							os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
					}
					os << " -&gt; ";
					{
						const PhysicalStop* ps = line->getDestination ();
						if (ps)
							os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
					}
					os << "</publishedName>" << endl;
					
					const vector<Edge*>& edges = line->getEdges ();
					for (vector<Edge*>::const_iterator itedge = edges.begin ();
						 itedge != edges.end (); ++itedge)
					{
						const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
						if (lineStop->getNextInPath () == 0) continue;
						os << "<ptLinkId>" << TridentId (peerid, "PtLink", lineStop->getKey ()) << "</ptLinkId>" << endl;
					}
					os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << endl;
					
					// Wayback
					int wayback(line->getWayBack() ? 1 : 0);
					if (withTisseoExtensions)
						++wayback;

					os << "<RouteExtension><wayBack>";
					if (!logic::indeterminate(line->getWayBack()))
						os << Conversion::ToString(wayback);
					os << "</wayBack></RouteExtension>" << endl;
					os << "</ChouetteRoute>" << endl;
				}
			}

			// --------------------------------------------------- StopPoint
			{
				for (set<const LineStop*>::const_iterator itls = lineStops.begin ();
				 itls != lineStops.end (); ++itls)
				{
					const LineStop* ls = (*itls);
					const PhysicalStop* ps = (const PhysicalStop*) ls->getFromVertex ();

					os << "<StopPoint" << (withTisseoExtensions ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << endl;
					os << "<objectId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</objectId>" << endl;
					os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << endl;

					Point2D pt (ps->getX (), ps->getY ());
					GeoPoint gp = FromLambertIIe (pt);
					
					os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << endl;
					os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << endl;
					os << "<longLatType>" << "WGS84" << "</longLatType>" << endl;
					
					os << "<projectedPoint>" << endl;
					os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << endl;
					os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << endl;
					os << "<projectionType>" << "LambertIIe" << "</projectionType>" << endl;
					os << "</projectedPoint>" << endl;


					os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << endl;
					os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
						ps->getConnectionPlace ()->getName ();
					if (ps->getName ().empty () == false) os << " (" + ps->getName () + ")";
					os << "</name>" << endl;
					
					os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << endl;
					os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</ptNetworkIdShortcut>" << endl;

					if (withTisseoExtensions)
					{
						os << "<mobilityRestrictedSuitability>0</mobilityRestrictedSuitability>" << endl;
					}
					os << "</StopPoint>" << endl;
				}

			}
			
			// --------------------------------------------------- ITL
			// Not implemented
			{

			}


			// --------------------------------------------------- PtLink
			{
				for (set<const LineStop*>::const_iterator itls = lineStops.begin ();
				 itls != lineStops.end (); ++itls)
				{
				const LineStop* from = (*itls);
				const LineStop* to = dynamic_cast<const LineStop*> ((*itls)->getNextInPath ());
				if (to == 0) continue;

				const PhysicalStop* ps = (const PhysicalStop*) from->getFromVertex ();

				os << "<PtLink>" << endl;
				os << "<objectId>" << TridentId (peerid, "PtLink", from->getKey ()) << "</objectId>" << endl;
				os << "<startOfLink>" << TridentId (peerid, "StopPoint", from->getKey ()) << "</startOfLink>" << endl;
				os << "<endOfLink>" << TridentId (peerid, "StopPoint", to->getKey ()) << "</endOfLink>" << endl;
				os << "<linkDistance>" << Conversion::ToString (from->getLength ()) << "</linkDistance>" << endl;   // in meters!
				os << "</PtLink>" << endl;
				}
			}

			// --------------------------------------------------- JourneyPattern
			{
				// One per route 
				for (LineSet::const_iterator itline = lines.begin ();
				 itline != lines.end (); ++itline)
				{
				shared_ptr<const Line> line = (*itline);

				if (line->getEdges().empty())
					continue;

				os << "<JourneyPattern>" << endl;
				os << "<objectId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</objectId>" << endl;
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << endl;

				const vector<Edge*>& edges = line->getEdges ();
				os << "<origin>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (0))->getKey ()) << "</origin>" << endl;
				os << "<destination>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (edges.size ()-1))->getKey ()) << "</destination>" << endl;

				for (vector<Edge*>::const_iterator itedge = edges.begin ();
					 itedge != edges.end (); ++itedge)
				{
					const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
					os << "<stopPointList>" << TridentId (peerid, "StopPoint", lineStop->getKey ()) << "</stopPointList>" << endl;
				}

				os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << endl;
				os << "</JourneyPattern>" << endl;
				}
			}

			// --------------------------------------------------- VehicleJourney
			{
				for (ServiceSet::const_iterator it = services.begin ();
				 it != services.end (); ++it)
				{
					const Line* line = ((const Line*) (*it)->getPath ());
					// subline tweak
					if (dynamic_cast<const SubLine*>(line))
						line = static_cast<const SubLine*>(line)->getMainLine();

					const NonPermanentService* srv(dynamic_cast<const NonPermanentService*>(*it));
					if (srv == NULL)
						continue;

					const Calendar& cal = srv->getCalendar ();

					const ScheduledService* ssrv(dynamic_cast<const ScheduledService*>(*it));
					const ContinuousService* csrv(dynamic_cast<const ContinuousService*>(*it));

					bool isDRT(srv->getReservationRule()->isCompliant() != false);

					os << "<VehicleJourney";
					if (withTisseoExtensions)
					{
						os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
					}
					os << ">" << endl;
					os << "<objectId>" << TridentId (peerid, "VehicleJourney", srv) << "</objectId>" << endl;
					os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << endl;
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << endl;
					os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << endl;
					os << "<lineIdShortcut>" << TridentId (peerid, "Line", commercialLine->getKey ()) << "</lineIdShortcut>" << endl;
					os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeIdShortcut>" << endl;
					if (!srv->getServiceNumber().empty())
					{
						os << "<number>" << srv->getServiceNumber() << "</number>" << endl;
					}

					// --------------------------------------------------- VehicleJourneyAtStop
					{
						const vector<Edge*>& edges = line->getEdges ();

						for (int e=0; e<edges.size (); ++e)
						{
							const LineStop* ls = (const LineStop*) edges.at (e);

							os << "<vehicleJourneyAtStop>" << endl;
							os << "<stopPointId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</stopPointId>" << endl;
							os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv) << "</vehicleJourneyId>" << endl;

							if (ssrv)
							{
								if (e != 0 && ls->isArrival())
									os << "<arrivalTime>" << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour ()) 
									<< "</arrivalTime>" << endl;
								
								os	<< "<departureTime>";
								if (e != edges.size () - 1 && ls->isDeparture())
								{
									os << ToXsdTime (srv->getDepartureBeginScheduleToIndex (ls->getRankInPath()).getHour());
								}
								else
								{
									os << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour());
								}
								os	<< "</departureTime>" << endl;
							}
							if (csrv)
							{
								const Schedule& schedule((e > 0 && ls->isArrival()) ? srv->getArrivalBeginScheduleToIndex(e) : srv->getDepartureBeginScheduleToIndex(e));
								os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(0)) << "</elapseDuration>" << endl;
								os << "<headwayFrequency>" << ToXsdDuration(csrv->getMaxWaitingTime()) << "</headwayFrequency>" << endl;
							}

							os << "</vehicleJourneyAtStop>" << endl;
						}
					}
					if (csrv) // Continuous service
					{
						os << "<timeSlotId>" << TridentId(peerid, "TimeSlot", srv) << "</timeSlotId>" << endl;
					}

					if (withTisseoExtensions)
					{
						os << "<mobilityRestrictedSuitability>" << Conversion::ToString(srv->getHandicappedCompliance()->isCompliant() != false) << "</mobilityRestrictedSuitability>" << endl;
						if (srv->getHandicappedCompliance()->getReservationRule())
							os << "<mobilityRestrictedSuitabilityReservationRule>" <<  TridentId(peerid, "ReservationRule", srv->getHandicappedCompliance()->getReservationRule()->getKey()) << "</mobilityRestrictedSuitabilityReservationRule>" << endl;
						os << "<bikeSuitability>" << Conversion::ToString(srv->getBikeCompliance()->isCompliant() != false) << "</bikeSuitability>" << endl;
						if (srv->getBikeCompliance()->getReservationRule())
							os << "<bikeReservationRule>" << TridentId(peerid, "ReservationRule", srv->getBikeCompliance()->getReservationRule()->getKey()) << "</bikeReservationRule>" << endl;
						if (isDRT)
						{
							os << "<reservationRule>" << TridentId(peerid, "ReservationRule", srv->getReservationRule()->getKey()) << "</reservationRule>" << endl;
						}
					}
					os << "</VehicleJourney>" << endl;
				}
			}


			os << "</ChouetteLineDescription>" << endl;
			}


			if (withTisseoExtensions)
			{
				// Reservation Rules -----------------------------------------------------------------------

				vector<shared_ptr<ReservationRule> > rules(ReservationRuleTableSync::Search());
				for (vector<shared_ptr<ReservationRule> >::const_iterator it(rules.begin()); it != rules.end(); ++it)
				{
					const ReservationRule& rule(**it);
					if (rule.isCompliant() == false || (rule.getMinDelayDays() == 0 && rule.getMinDelayMinutes() == 0))	continue;

					os << "<ReservationRule>" << endl;
					os << "<objectId>" << TridentId (peerid, "ReservationRule", (*it)->getKey ()) << "</objectId>" << endl;
					os << "<ReservationCompulsory>" << (rule.isCompliant() == true ? "compulsory" : "optional") << "</ReservationCompulsory>" << endl;
					os << "<deadLineIsTheCustomerDeparture>" << Conversion::ToString(!rule.getOriginIsReference()) << "</deadLineIsTheCustomerDeparture>" << endl;
					if (rule.getMinDelayMinutes() > 0)
					{
						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << endl;
					}
					if (rule.getMinDelayDays() > 0)
					{
						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << endl;
					}
					if (!rule.getHourDeadLine().isUnknown())
					{
						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << endl;
					}
					if (rule.getMaxDelayDays() > 0)
					{
						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << endl;
					}
					if (!rule.getPhoneExchangeNumber().empty())
					{
						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << endl;
						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << endl;
					}
					if (!rule.getWebSiteUrl().empty())
					{
						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << endl;
					}
					os << "</ReservationRule>" << endl;
				}

				// Non concurrency -----------------------------------------------------------------------
				vector<shared_ptr<NonConcurrencyRule> > ncrules(NonConcurrencyRuleTableSync::Search(
					commercialLineId, commercialLineId, false));
				for(vector<shared_ptr<NonConcurrencyRule> >::const_iterator itr(ncrules.begin()); itr != ncrules.end(); ++itr)
				{
					const NonConcurrencyRule& rule(**itr);
					os << "<LineConflict>" << endl;
					os << "<objectId>" << TridentId (peerid, "LineConflict", rule.getKey ()) << "</objectId>" << endl;
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule.getHiddenLine()) << "</forbiddenLine>" << endl;
					os << "<usedLine>" << TridentId (peerid, "Line", rule.getPriorityLine()) << "</usedLine>" << endl;
					os << "<conflictDelay>" << ToXsdDuration(rule.getDelay()) << "</conflictDelay>" << endl;
					os << "</LineConflict>" << endl;
				}


				// CityMainStops --------------------------------------------------- 
				for(set<const City*>::const_iterator itc(cities.begin());
					itc != cities.end ();
					++itc
				){
					const City& ci(**itc);
					vector<string> containedStopAreas;

					// Contained connection places
					const vector<const Place*>& ciip = ci.getIncludedPlaces ();
					for (vector<const Place*>::const_iterator itip = ciip.begin ();
						itip != ciip.end (); ++itip)
					{
						const PublicTransportStopZoneConnectionPlace* cp = dynamic_cast<const PublicTransportStopZoneConnectionPlace*> (*itip);
						if (cp == 0) continue;

						// filter physical stops not concerned by this line.
						if (connectionPlaces.find (cp) == connectionPlaces.end ()) continue;

						containedStopAreas.push_back (TridentId (peerid, "StopArea", cp->getKey ()));

					}
					if (containedStopAreas.size () == 0) continue;


					os << "<CityMainStops>" << endl;
					os << "<objectId>" << TridentId (peerid, "CityMainStops", ci.getKey ()) << "</objectId>" << endl;
					os << "<name>" << ci.getName () << "</name>" << endl;


					for (vector<string>::const_iterator itsa = containedStopAreas.begin ();
						itsa != containedStopAreas.end (); ++itsa) {
							os << "<contains>" << (*itsa)  << "</contains>" << endl;
					}


					os << "</CityMainStops>" << endl;
				}
			}


			if (withTisseoExtensions)
				os << "</TisseoPTNetwork>" <<  endl << flush;
			else
				os << "</ChouettePTNetwork>" <<  endl << flush;
		}


		string TridentExport::TridentId(
			const string& peer,
			const string clazz,
			const uid& id
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << id;
			return ss.str ();
		}


		string TridentExport::TridentId(
			const string& peer,
			const string clazz,
			const string& s
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << s;
			return ss.str ();
		}


		string TridentExport::TridentId(
			const string& peer,
			const string clazz,
			const synthese::env::Service* service
		){
			stringstream ss;
			string skey = service->getServiceNumber ();
			if (skey.empty()) skey = Conversion::ToString(service->getId());

			const Line* line(static_cast<const Line*>(service->getPath()));
			if (dynamic_cast<const SubLine*>(line))
				line = static_cast<const SubLine*>(line)->getMainLine();

			ss << peer << ":" << clazz << ":" << line->getKey() << "s" << skey ;

			return ss.str ();
		}
	}
}

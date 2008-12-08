
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
#include "ConnectionPlaceTableSync.h"
#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ServiceDateTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "Line.h"
#include "LineTableSync.h"
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
#include <utility>

// Boost
#include <boost/foreach.hpp>

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
		TridentExport::TridentExport(const CommercialLine* line, bool _withTisseoExtension)
			: _commercialLine(line)
			, _withTisseoExtension(_withTisseoExtension)
		{ }


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


		void TridentExport::run(
			ostream& os
		){

			static const string peerid ("SYNTHESE");

			os.imbue (locale(""));

			// os.imbue (locale("en_US.ISO-8859-15"));
			cerr << "locale = " << os.getloc ().name () << "\n";



			os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << "\n" << "\n";

			const TransportNetwork* tn = _commercialLine->getNetwork ();
		    

			// Collect all data related to selected commercial line
			Env env;
			LineTableSync::Search(env, _commercialLine->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
			NonConcurrencyRuleTableSync::Search(env, _commercialLine->getKey(), _commercialLine->getKey(), false);

			// Lines
			const RollingStock* rollingStock;
			BOOST_FOREACH(shared_ptr<Line> line, env.template getRegistry<Line>())
			{
				if (line->getRollingStock())
					rollingStock = line->getRollingStock();
				LineStopTableSync::Search(env, line->getKey(), UNKNOWN_VALUE, 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
				ScheduledServiceTableSync::Search(env, line->getKey(), UNKNOWN_VALUE, DATE(TIME_UNKNOWN), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
				ContinuousServiceTableSync::Search(env, line->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
			}    


			// Writing of the header
		    if (_withTisseoExtension)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident http://www.reseaux-conseil.com/trident/tisseo-chouette-extension.xsd'>" << "\n";
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << "\n";

			// --------------------------------------------------- PTNetwork 
			{
			os << "<PTNetwork>" << "\n";
			os << "<objectId>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</objectId>" << "\n";
			os << "<versionDate>" << ToXsdDate (Date (TIME_CURRENT)) << "</versionDate>" << "\n";
			os << "<name>" << tn->getName () << "</name>" << "\n";
			os << "<registration>" << "\n";
			os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << "\n";
			os << "</registration>" << "\n";
			os << "<lineId>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineId>" << "\n";
			os << "<comment/>" << "\n";
			os << "</PTNetwork>" << "\n";
			}

			// --------------------------------------------------- GroupOfLine


			// --------------------------------------------------- Company
			os << "<Company>" << "\n";
			os << "<objectId>SYNTHESE:Company:1</objectId>" << "\n";
			os << "<name>Tisséo Réseau Urbain</name>" << "\n";
			os << "<shortName>TRU</shortName>" << "\n";
			os << "<organisationalUnit></organisationalUnit>" << "\n";
			os << "<operatingDepartmentName></operatingDepartmentName>" << "\n";
			os << "<code>31000</code>" << "\n";
			os << "<phone>0561417070</phone>" << "\n";
			os << "<fax></fax>" << "\n";
			os << "<email></email>" << "\n";
			os << "<registration><registrationNumber>1</registrationNumber></registration>" << "\n";
			os << "</Company>" << "\n";

			os << "<ChouetteArea>" << "\n";

			// --------------------------------------------------- StopArea (type = BoardingPosition)
			// BoardingPosition corresponds to a very accurate location along a quay for instance.
			// Not implemented right now.

			// --------------------------------------------------- StopArea (type = Quay) <=> PhysicalStop
			BOOST_FOREACH(shared_ptr<PhysicalStop> ps, env.template getRegistry<PhysicalStop>())
			{
				if ((ps->getDepartureEdges ().size () == 0) && (ps->getArrivalEdges ().size () == 0)) continue;

				os << "<StopArea>" << "\n";

				os << "<objectId>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\n";

				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
					ps->getConnectionPlace ()->getName ();
				if (!ps->getName().empty()) os << " (" + ps->getName () + ")";
				os << "</name>" << "\n";

				Env lsenv;
				LineStopTableSync::Search(lsenv, UNKNOWN_VALUE,ps->getKey());
				BOOST_FOREACH(shared_ptr<LineStop> ls, lsenv.template getRegistry<LineStop>())
				{
					os << "<contains>" << TridentId (peerid, "StopPoint", ls->getKey ())  << "</contains>" << "\n";
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</centroidOfArea>" << "\n";
				os << "<StopAreaExtension>" << "\n";
				os << "<areaType>" << "Quay" << "</areaType>" << "\n";
				string rn = ps->getOperatorCode ();
				if (rn.empty ()) rn = "0";
				os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << "\n";
				os << "</StopAreaExtension>" << "\n";
				os << "</StopArea>" << "\n";
			    
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			BOOST_FOREACH(shared_ptr<PublicTransportStopZoneConnectionPlace> cp, env.template getRegistry<PublicTransportStopZoneConnectionPlace>())
			{
				os << "<StopArea>" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopArea", cp->getKey ()) << "</objectId>" << "\n";
				os << "<name>" << cp->getCity ()->getName () << " " << cp->getName () << "</name>" << "\n";

				// Contained physical stops
				const PhysicalStops& stops(cp->getPhysicalStops());
				for(PhysicalStops::const_iterator it(stops.begin()); it != stops.end(); ++it)
				{
					os << "<contains>" << TridentId (peerid, "StopArea", it->second->getKey())  << "</contains>" << "\n";
				}

				// Decide what to take for centroidOfArea of a connectionPlace. Only regarding physical stops coordinates
				// or also regarding addresses coordinates, or fixed manually ? 
				// os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << "\n";
			    
				os << "<StopAreaExtension>" << "\n";
				os << "<areaType>" << "CommercialStopPoint" << "</areaType>" << "\n";
				os << "</StopAreaExtension>" << "\n";
				os << "</StopArea>" << "\n";
			}

		    
			// --------------------------------------------------- StopArea (type = ITL)
			// "Interdiction de traffic local" 
			// Not mapped right now.
		    


			// --------------------------------------------------- AreaCentroid
			BOOST_FOREACH(shared_ptr<PhysicalStop> ps, env.template getRegistry<PhysicalStop>())
			{
				os << "<AreaCentroid>" << "\n";
				os << "<objectId>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</objectId>" << "\n";
			    
				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = FromLambertIIe (pt);
			    
				os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\n";
				os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";

				// we do not provide full addresses right now.
				os << "<address><countryCode>" << ps->getPlace()->getCity()->getCode() << "</countryCode></address>";

				os << "<projectedPoint>" << "\n";
				os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << "\n";
				os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << "\n";
				os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\n";
				os << "</projectedPoint>" << "\n";

				os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << "\n";
				os << "<name>" << Conversion::ToString (ps->getKey ()) << "</name>" << "\n";

				os << "</AreaCentroid>" << "\n";
			    
			}
		
			os << "</ChouetteArea>" << "\n";

			// --------------------------------------------------- ConnectionLink
			BOOST_FOREACH(shared_ptr<PublicTransportStopZoneConnectionPlace> cp, env.template getRegistry<PublicTransportStopZoneConnectionPlace>())
			{
				// Contained physical stops
				const PhysicalStops& stops(cp->getPhysicalStops());
				for(PhysicalStops::const_iterator it1(stops.begin()); it1 != stops.end(); ++it1)
				{
					for(PhysicalStops::const_iterator it2(stops.begin()); it2 != stops.end(); ++it2)
					{
						os << "<ConnectionLink>" << "\n";
						stringstream clkey;
						clkey << it1->second->getKey () << "t" << it2->second->getKey ();
						os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopArea", it1->second->getKey ()) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopArea", it2->second->getKey ()) << "</endOfLink>" << "\n";
						os << "<defaultDuration>" << ToXsdDuration (cp->getTransferDelay (it1->second, it2->second)) << "</defaultDuration>" << "\n";
						os << "</ConnectionLink>" << "\n";
					}

				}
			}
		    
			// --------------------------------------------------- Timetable
			// One timetable per service
			BOOST_FOREACH(shared_ptr<ScheduledService> srv, env.template getRegistry<ScheduledService>())
			{
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", srv.get()) << "</objectId>" << "\n";

				BOOST_FOREACH(Date date, ServiceDateTableSync::GetDatesOfService(srv->getKey()))
				{
					os << "<calendarDay>" << ToXsdDate (date) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}
			BOOST_FOREACH(shared_ptr<ContinuousService> srv, env.template getRegistry<ContinuousService>())
			{
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", srv.get()) << "</objectId>" << "\n";

				BOOST_FOREACH(Date date, ServiceDateTableSync::GetDatesOfService(srv->getKey()))
				{
					os << "<calendarDay>" << ToXsdDate (date) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}


			// --------------------------------------------------- TimeSlot
			BOOST_FOREACH(shared_ptr<ContinuousService> csrv, env.template getRegistry<ContinuousService>())
			{
				string timeSlotId;
				timeSlotId = TridentId(peerid, "TimeSlot", csrv.get());

				os << "<TimeSlot>" << "\n";
				os << "<objectId>" << timeSlotId << "</objectId>" << "\n";
				os << "<beginningSlotTime>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</beginningSlotTime>" << "\n";
				os << "<endSlotTime>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</endSlotTime>" << "\n";
				os << "<firstDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</firstDepartureTimeInSlot>" << "\n";
				os << "<lastDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</lastDepartureTimeInSlot>" << "\n";
				os << "</TimeSlot>" << "\n";
			}


			// --------------------------------------------------- ChouetteLineDescription
			{
			os << "<ChouetteLineDescription>" << "\n";
			
			// --------------------------------------------------- Line
			{
				os << "<Line>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</objectId>" << "\n";
				os << "<name>" << _commercialLine->getName () << "</name>" << "\n";
				os << "<number>" << _commercialLine->getShortName () << "</number>" << "\n";
				os << "<publishedName>" << _commercialLine->getLongName () << "</publishedName>" << "\n";
				
				string tm ("");
				if (rollingStock != NULL)
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
				os << "<transportModeName>" << tm << "</transportModeName>" << "\n";
			    
				BOOST_FOREACH(shared_ptr<Line> line, env.template getRegistry<Line>())
				{
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << "\n";
				}
				os << "<registration>" << "\n";
				os << "<registrationNumber>" << Conversion::ToString (_commercialLine->getKey ()) << "</registrationNumber>" << "\n";
				os << "</registration>" << "\n";

				os << "</Line>" << "\n";
			}

			// --------------------------------------------------- ChouetteRoute
			BOOST_FOREACH(shared_ptr<Line> line, env.template getRegistry<Line>())
			{
				os << "<ChouetteRoute>" << "\n";
				os << "<objectId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</objectId>" << "\n";
				os << "<name>" << line->getName () << "</name>" << "\n";
				
				os << "<publishedName>";
				{
					const PhysicalStop* ps(line->getOrigin());
					if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
						os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << " -&gt; ";
				{
					const PhysicalStop* ps(line->getDestination());
					if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
						os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << "</publishedName>" << "\n";
				
				Env lenv;
				LineStopTableSync::Search(lenv,line->getKey());
				BOOST_FOREACH(shared_ptr<LineStop> lineStop, lenv.template getRegistry<LineStop>())
				{
					os << "<ptLinkId>" << TridentId (peerid, "PtLink", lineStop->getKey ()) << "</ptLinkId>" << "\n";
				}
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << "\n";
				
				// Wayback
				int wayback(line->getWayBack() ? 1 : 0);
				if (_withTisseoExtension)
					++wayback;

				os << "<RouteExtension><wayBack>";
				if (!logic::indeterminate(line->getWayBack()))
					os << Conversion::ToString(wayback);
				os << "</wayBack></RouteExtension>" << "\n";
				os << "</ChouetteRoute>" << "\n";
			}
		
			// --------------------------------------------------- StopPoint
			BOOST_FOREACH(shared_ptr<LineStop> ls, env.template getRegistry<LineStop>())
			{
				const PhysicalStop* ps = static_cast<const PhysicalStop*>(ls->getFromVertex());

				os << "<StopPoint" << (_withTisseoExtension ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\n";

				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = FromLambertIIe (pt);
				
				os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\n";
				os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";
				
				os << "<address><countryCode>" << ps->getPlace()->getCity()->getCode() << "</countryCode></address>";

				os << "<projectedPoint>" << "\n";
				os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << "\n";
				os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << "\n";
				os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\n";
				os << "</projectedPoint>" << "\n";


				os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << "\n";
				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
					ps->getConnectionPlace ()->getName ();
				if (ps->getName ().empty () == false) os << " (" + ps->getName () + ")";
				os << "</name>" << "\n";
				
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\n";
				os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</ptNetworkIdShortcut>" << "\n";

				if (_withTisseoExtension)
				{
					os << "<mobilityRestrictedSuitability>0</mobilityRestrictedSuitability>" << "\n";
				}
				os << "</StopPoint>" << "\n";
			}
			
			// --------------------------------------------------- ITL
			// Not implemented
			{

			}


			// --------------------------------------------------- PtLink
			BOOST_FOREACH(shared_ptr<Line> line, env.template getRegistry<Line>())
			{
				Env lenv;
				LineStopTableSync::Search(lenv, line->getKey());
				shared_ptr<LineStop> from;
				BOOST_FOREACH(shared_ptr<LineStop> to, env.template getRegistry<LineStop>())
				{
					if (from.get())
					{
						os << "<PtLink>" << "\n";
						os << "<objectId>" << TridentId (peerid, "PtLink", from->getKey()) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopPoint", from->getKey ()) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopPoint", to->getKey ()) << "</endOfLink>" << "\n";
						os << "<linkDistance>" << Conversion::ToString (from->getLength ()) << "</linkDistance>" << "\n";   // in meters!
						os << "</PtLink>" << "\n";
					}
					from = to;
				}
			}		

			// --------------------------------------------------- JourneyPattern
			// One per route 
			BOOST_FOREACH(shared_ptr<Line> line, env.template getRegistry<Line>())
			{
				if (line->getEdges().empty())
					continue;

				os << "<JourneyPattern>" << "\n";
				os << "<objectId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</objectId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << "\n";

				const vector<Edge*>& edges = line->getEdges ();
				os << "<origin>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (0))->getKey ()) << "</origin>" << "\n";
				os << "<destination>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (edges.size ()-1))->getKey ()) << "</destination>" << "\n";

				for (vector<Edge*>::const_iterator itedge = edges.begin ();
					 itedge != edges.end (); ++itedge)
				{
					const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
					os << "<stopPointList>" << TridentId (peerid, "StopPoint", lineStop->getKey ()) << "</stopPointList>" << "\n";
				}

				os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\n";
				os << "</JourneyPattern>" << "\n";
			}
		
			// --------------------------------------------------- VehicleJourney
			BOOST_FOREACH(shared_ptr<ScheduledService> srv, env.template getRegistry<ScheduledService>())
			{
				bool isDRT(srv->getReservationRule()->isCompliant() != false);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPathId()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop
				Env lenv;
				LineStopTableSync::Search(lenv, srv->getPathId());
				BOOST_FOREACH(shared_ptr<LineStop> ls, lenv.template getRegistry<LineStop>())
				{
					os << "<vehicleJourneyAtStop>" << "\n";
					os << "<stopPointId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</stopPointId>" << "\n";
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\n";

					if (ls->getRankInPath() > 0 && ls->isArrival())
						os << "<arrivalTime>" << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour ()) 
						<< "</arrivalTime>" << "\n";

					os	<< "<departureTime>";
					if (ls->getRankInPath() != lenv.template getRegistry<LineStop>().size () - 1 && ls->isDeparture())
					{
						os << ToXsdTime (srv->getDepartureBeginScheduleToIndex (ls->getRankInPath()).getHour());
					}
					else
					{
						os << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour());
					}
					os	<< "</departureTime>" << "\n";

					os << "</vehicleJourneyAtStop>" << "\n";
				}

				if (_withTisseoExtension)
				{
					os << "<mobilityRestrictedSuitability>" << Conversion::ToString(srv->getHandicappedCompliance()->isCompliant() != false) << "</mobilityRestrictedSuitability>" << "\n";
					if (srv->getHandicappedCompliance()->getReservationRule())
						os << "<mobilityRestrictedSuitabilityReservationRule>" <<  TridentId(peerid, "ReservationRule", srv->getHandicappedCompliance()->getReservationRule()->getKey()) << "</mobilityRestrictedSuitabilityReservationRule>" << "\n";
					os << "<bikeSuitability>" << Conversion::ToString(srv->getBikeCompliance()->isCompliant() != false) << "</bikeSuitability>" << "\n";
					if (srv->getBikeCompliance()->getReservationRule())
						os << "<bikeReservationRule>" << TridentId(peerid, "ReservationRule", srv->getBikeCompliance()->getReservationRule()->getKey()) << "</bikeReservationRule>" << "\n";
					if (isDRT)
					{
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", srv->getReservationRule()->getKey()) << "</reservationRule>" << "\n";
					}
				}
				os << "</VehicleJourney>" << "\n";
			}

			BOOST_FOREACH(shared_ptr<ContinuousService> srv, env.template getRegistry<ContinuousService>())
			{
				bool isDRT(srv->getReservationRule()->isCompliant() != false);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPathId()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop
				{
					Env lenv;
					LineStopTableSync::Search(lenv, srv->getPathId());
					BOOST_FOREACH(shared_ptr<LineStop> ls, lenv.template getRegistry<LineStop>())
					{
						os << "<vehicleJourneyAtStop>" << "\n";
						os << "<stopPointId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</stopPointId>" << "\n";
						os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\n";

						const Schedule& schedule((ls->getRankInPath() > 0 && ls->isArrival()) ? srv->getArrivalBeginScheduleToIndex(ls->getRankInPath()) : srv->getDepartureBeginScheduleToIndex(ls->getRankInPath()));
						os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(0)) << "</elapseDuration>" << "\n";
						os << "<headwayFrequency>" << ToXsdDuration(srv->getMaxWaitingTime()) << "</headwayFrequency>" << "\n";

						os << "</vehicleJourneyAtStop>" << "\n";
					}
				}
				os << "<timeSlotId>" << TridentId(peerid, "TimeSlot", srv.get()) << "</timeSlotId>" << "\n";

				if (_withTisseoExtension)
				{
					os << "<mobilityRestrictedSuitability>" << Conversion::ToString(srv->getHandicappedCompliance()->isCompliant() != false) << "</mobilityRestrictedSuitability>" << "\n";
					if (srv->getHandicappedCompliance()->getReservationRule())
						os << "<mobilityRestrictedSuitabilityReservationRule>" <<  TridentId(peerid, "ReservationRule", srv->getHandicappedCompliance()->getReservationRule()->getKey()) << "</mobilityRestrictedSuitabilityReservationRule>" << "\n";
					os << "<bikeSuitability>" << Conversion::ToString(srv->getBikeCompliance()->isCompliant() != false) << "</bikeSuitability>" << "\n";
					if (srv->getBikeCompliance()->getReservationRule())
						os << "<bikeReservationRule>" << TridentId(peerid, "ReservationRule", srv->getBikeCompliance()->getReservationRule()->getKey()) << "</bikeReservationRule>" << "\n";
					if (isDRT)
					{
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", srv->getReservationRule()->getKey()) << "</reservationRule>" << "\n";
					}
				}
				os << "</VehicleJourney>" << "\n";
			}


			os << "</ChouetteLineDescription>" << "\n";
			}


			if (_withTisseoExtension)
			{
				// Reservation Rules -----------------------------------------------------------------------

				BOOST_FOREACH(shared_ptr<ReservationRule> r, env.template getRegistry<ReservationRule>())
				{
					const ReservationRule& rule(*r);

					if (rule.isCompliant() == false || (rule.getMinDelayDays() == 0 && rule.getMinDelayMinutes() == 0))	continue;

					os << "<ReservationRule>" << "\n";
					os << "<objectId>" << TridentId (peerid, "ReservationRule", rule.getKey ()) << "</objectId>" << "\n";
					os << "<ReservationCompulsory>" << (rule.isCompliant() == true ? "compulsory" : "optional") << "</ReservationCompulsory>" << "\n";
					os << "<deadLineIsTheCustomerDeparture>" << Conversion::ToString(!rule.getOriginIsReference()) << "</deadLineIsTheCustomerDeparture>" << "\n";
					if (rule.getMinDelayMinutes() > 0)
					{
						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << "\n";
					}
					if (rule.getMinDelayDays() > 0)
					{
						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << "\n";
					}
					if (!rule.getHourDeadLine().isUnknown())
					{
						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << "\n";
					}
					if (rule.getMaxDelayDays() > 0)
					{
						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << "\n";
					}
					if (!rule.getPhoneExchangeNumber().empty())
					{
						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << "\n";
						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << "\n";
					}
					if (!rule.getWebSiteUrl().empty())
					{
						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << "\n";
					}
					os << "</ReservationRule>" << "\n";
				}

				// Non concurrency -----------------------------------------------------------------------
				BOOST_FOREACH(shared_ptr<NonConcurrencyRule> rule, env.template getRegistry<NonConcurrencyRule>())
				{
					os << "<LineConflict>" << "\n";
					os << "<objectId>" << TridentId (peerid, "LineConflict", rule->getKey ()) << "</objectId>" << "\n";
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule->getHiddenLine()) << "</forbiddenLine>" << "\n";
					os << "<usedLine>" << TridentId (peerid, "Line", rule->getPriorityLine()) << "</usedLine>" << "\n";
					os << "<conflictDelay>" << ToXsdDuration(rule->getDelay()) << "</conflictDelay>" << "\n";
					os << "</LineConflict>" << "\n";
				}


				// CityMainStops --------------------------------------------------- 
				BOOST_FOREACH(shared_ptr<City> city, env.template getRegistry<City>())
				{
					vector<string> containedStopAreas;

					// Contained connection places
					Env senv;
					ConnectionPlaceTableSync::Search(senv, city->getKey(), true);
					BOOST_FOREACH(shared_ptr<const PublicTransportStopZoneConnectionPlace> cp, senv.template getRegistry<PublicTransportStopZoneConnectionPlace>())
					{
						// filter physical stops not concerned by this line.
						if(!env.template getRegistry<PublicTransportStopZoneConnectionPlace>().contains(cp->getKey())) continue;

						containedStopAreas.push_back (TridentId (peerid, "StopArea", cp->getKey()));

					}
					if (containedStopAreas.size () == 0) continue;


					os << "<CityMainStops>" << "\n";
					os << "<objectId>" << TridentId (peerid, "CityMainStops", city->getKey ()) << "</objectId>" << "\n";
					os << "<name>" << city->getName () << "</name>" << "\n";


					BOOST_FOREACH(string sa, containedStopAreas)
					{
						os << "<contains>" << sa  << "</contains>" << "\n";
					}

					os << "</CityMainStops>" << "\n";
				}
			}


			if (_withTisseoExtension)
				os << "</TisseoPTNetwork>" <<  "\n" << flush;
			else
				os << "</ChouettePTNetwork>" <<  "\n" << flush;
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
			const NonPermanentService* service
		){
			stringstream ss;
			string skey = service->getServiceNumber ();
			if (skey.empty()) skey = Conversion::ToString(service->getKey());
			
			ss << peer << ":" << clazz << ":" << service->getPathId() << "s" << skey ;

			return ss.str ();
		}



		TridentExport::~TridentExport()
		{

		}
	}
}

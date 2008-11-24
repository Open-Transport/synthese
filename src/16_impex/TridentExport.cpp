
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
			cerr << "locale = " << os.getloc ().name () << "\r";



			os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << "\r" << "\r";

			const TransportNetwork* tn = _commercialLine->getNetwork ();
		    

			// Collect all data related to selected commercial line

			typedef map<uid, const PhysicalStop* > PhysicalStops;
			PhysicalStops physicalStops;
			typedef map<uid, const PublicTransportStopZoneConnectionPlace* > ConnectionPlaces;
			ConnectionPlaces connectionPlaces;
			typedef map<uid, const City* > Cities;
			Cities cities;
			typedef map<uid, const ReservationRule* > ReservationRules;
			ReservationRules reservationRules;
			vector<shared_ptr<LineStop> > lineStops;
			vector<shared_ptr<NonPermanentService> > services;
			vector<shared_ptr<Line> > lines(LineTableSync::search(_commercialLine->getKey()));
			const RollingStock* rollingStock;

			
			// Lines
			BOOST_FOREACH(shared_ptr<Line> line, lines)
			{
				// Rolling stock
				rollingStock = line->getRollingStock();

				// Reservation rule
				const ReservationRule* reservationRule(line->getReservationRule());
				if (reservationRule && reservationRules.find(reservationRule->getKey()) == reservationRules.end())
					reservationRules.insert(make_pair(reservationRule->getKey(), reservationRule));

				// Linestops
				BOOST_FOREACH(shared_ptr<LineStop> lineStop, LineStopTableSync::Search(line->getKey()))
				{
					lineStops.push_back(lineStop);
					
					if (lineStop->getFromVertex ()->isPhysicalStop ()) 
					{
						const PhysicalStop* physicalStop = dynamic_cast<const PhysicalStop*> (lineStop->getFromVertex ());

						// Physical stop
						if (physicalStop && physicalStops.find(physicalStop->getKey()) == physicalStops.end())
							physicalStops.insert(make_pair(physicalStop->getKey(), physicalStop));

						// Connection place
						const PublicTransportStopZoneConnectionPlace* connectionPlace(physicalStop->getConnectionPlace ());
						if (connectionPlace && connectionPlaces.find(connectionPlace->getKey()) == connectionPlaces.end()) 
						{
							connectionPlaces.insert(make_pair(connectionPlace->getKey(), connectionPlace));

							// City
							const City* city(connectionPlace->getCity());
							if (city && cities.find(city->getKey()) == cities.end())
								cities.insert(make_pair(city->getKey(), city));
						}
						
					}
				}
				    
				// Scheduled services
				BOOST_FOREACH(shared_ptr<ScheduledService> service, ScheduledServiceTableSync::search(line->getKey()))
				{
					services.push_back(static_pointer_cast<NonPermanentService, ScheduledService>(service));
				}

				// Continuous services
				BOOST_FOREACH(shared_ptr<ContinuousService> service, ContinuousServiceTableSync::search(line->getKey()))
				{
					services.push_back(static_pointer_cast<NonPermanentService, ContinuousService>(service));
				}
			}
		    

			// Writing of the header
		    if (_withTisseoExtension)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident http://www.reseaux-conseil.com/trident/tisseo-chouette-extension.xsd'>" << "\r";
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << "\r";

			// --------------------------------------------------- PTNetwork 
			{
			os << "<PTNetwork>" << "\r";
			os << "<objectId>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</objectId>" << "\r";
			os << "<versionDate>" << ToXsdDate (Date (TIME_CURRENT)) << "</versionDate>" << "\r";
			os << "<name>" << tn->getName () << "</name>" << "\r";
			os << "<registration>" << "\r";
			os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << "\r";
			os << "</registration>" << "\r";
			os << "<lineId>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineId>" << "\r";
			os << "<comment/>" << "\r";
			os << "</PTNetwork>" << "\r";
			}

			// --------------------------------------------------- GroupOfLine


			// --------------------------------------------------- Company
			os << "<Company>" << "\r";
			os << "<objectId>SYNTHESE:Company:1</objectId>" << "\r";
			os << "<name>Tisséo Réseau Urbain</name>" << "\r";
			os << "<shortName>TRU</shortName>" << "\r";
			os << "<organisationalUnit></organisationalUnit>" << "\r";
			os << "<operatingDepartmentName></operatingDepartmentName>" << "\r";
			os << "<code>31000</code>" << "\r";
			os << "<phone>0561417070</phone>" << "\r";
			os << "<fax></fax>" << "\r";
			os << "<email></email>" << "\r";
			os << "<registration><registrationNumber>1</registrationNumber></registration>" << "\r";
			os << "</Company>" << "\r";

			os << "<ChouetteArea>" << "\r";

			// --------------------------------------------------- StopArea (type = BoardingPosition)
			// BoardingPosition corresponds to a very accurate location along a quay for instance.
			// Not implemented right now.

			// --------------------------------------------------- StopArea (type = Quay) <=> PhysicalStop
			{
				BOOST_FOREACH(PhysicalStops::value_type pps, physicalStops)
				{
					const PhysicalStop* ps = pps.second;

					if ((ps->getDepartureEdges ().size () == 0) && (ps->getArrivalEdges ().size () == 0)) continue;

					os << "<StopArea>" << "\r";

					os << "<objectId>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</objectId>" << "\r";
					os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\r";

					os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
						ps->getConnectionPlace ()->getName ();
					if (!ps->getName().empty()) os << " (" + ps->getName () + ")";
					os << "</name>" << "\r";

					vector<shared_ptr<LineStop> > allLineStops(LineStopTableSync::Search(UNKNOWN_VALUE,ps->getKey()));
					BOOST_FOREACH(shared_ptr<LineStop> ls, allLineStops)
					{
						os << "<contains>" << TridentId (peerid, "StopPoint", ls->getKey ())  << "</contains>" << "\r";
					}

					os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</centroidOfArea>" << "\r";
					os << "<StopAreaExtension>" << "\r";
					os << "<areaType>" << "Quay" << "</areaType>" << "\r";
					string rn = ps->getOperatorCode ();
					if (rn.empty ()) rn = "0";
					os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << "\r";
					os << "</StopAreaExtension>" << "\r";
					os << "</StopArea>" << "\r";
				    
				}
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			{
				BOOST_FOREACH(ConnectionPlaces::value_type pcp, connectionPlaces)
				{
					const PublicTransportStopZoneConnectionPlace* cp = pcp.second;

					vector<shared_ptr<PhysicalStop> > stops(PhysicalStopTableSync::Search(cp->getId()));

					if (stops.size () == 0) continue;

					os << "<StopArea>" << "\r";
					os << "<objectId>" << TridentId (peerid, "StopArea", cp->getKey ()) << "</objectId>" << "\r";
					os << "<name>" << cp->getCity ()->getName () << " " << cp->getName () << "</name>" << "\r";

					// Contained physical stops
					BOOST_FOREACH(shared_ptr<PhysicalStop> ps, stops)
					{
						// filter physical stops not concerned by this line.
						if (physicalStops.find (ps->getKey()) == physicalStops.end ()) continue;
						
						os << "<contains>" << TridentId (peerid, "StopArea", ps->getKey())  << "</contains>" << "\r";
					}

					// Decide what to take for centroidOfArea of a connectionPlace. Only regarding physical stops coordinates
					// or also regarding addresses coordinates, or fixed manually ? 
					// os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << "\r";
				    
					os << "<StopAreaExtension>" << "\r";
					os << "<areaType>" << "CommercialStopPoint" << "</areaType>" << "\r";
					os << "</StopAreaExtension>" << "\r";
					os << "</StopArea>" << "\r";
				}
			}

		    
			// --------------------------------------------------- StopArea (type = ITL)
			// "Interdiction de traffic local" 
			// Not mapped right now.
		    


			// --------------------------------------------------- AreaCentroid
			{
				BOOST_FOREACH(PhysicalStops::value_type pps, physicalStops)
				{
					const PhysicalStop* ps(pps.second);

					os << "<AreaCentroid>" << "\r";
					os << "<objectId>" << TridentId (peerid, "AreaCentroid", ps->getKey ()) << "</objectId>" << "\r";
				    
					Point2D pt (ps->getX (), ps->getY ());
					GeoPoint gp = FromLambertIIe (pt);
				    
					os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\r";
					os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\r";
					os << "<longLatType>" << "WGS84" << "</longLatType>" << "\r";

					os << "<projectedPoint>" << "\r";
					os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << "\r";
					os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << "\r";
					os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\r";
					os << "</projectedPoint>" << "\r";

					os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << "\r";
					os << "<name>" << Conversion::ToString (ps->getKey ()) << "</name>" << "\r";

					// we do not provide full addresses right now.
					os << "<address><countryCode>" << ps->getPlace()->getCity()->getCode() << "</countryCode></address>";

					os << "</AreaCentroid>" << "\r";
				    
				}
			}

			os << "</ChouetteArea>" << "\r";

			// --------------------------------------------------- ConnectionLink
			{
				BOOST_FOREACH(ConnectionPlaces::value_type cpp, connectionPlaces)
				{
					const ConnectionPlace* cp(cpp.second);

					// Contained physical stops
					BOOST_FOREACH(shared_ptr<PhysicalStop> from, PhysicalStopTableSync::Search(cp->getId()))
					{
						// filter physical stops not concerned by this line.
						if (physicalStops.find (from->getKey()) == physicalStops.end ()) continue;

						BOOST_FOREACH(shared_ptr<PhysicalStop> to, PhysicalStopTableSync::Search(cp->getId()))
						{
							// filter physical stops not concerned by this line.
							if (physicalStops.find (to->getKey()) == physicalStops.end ()) continue;
						    
							os << "<ConnectionLink>" << "\r";
							stringstream clkey;
							clkey << from->getKey () << "t" << to->getKey ();
							os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << "\r";
							os << "<startOfLink>" << TridentId (peerid, "StopArea", from->getKey ()) << "</startOfLink>" << "\r";
							os << "<endOfLink>" << TridentId (peerid, "StopArea", to->getKey ()) << "</endOfLink>" << "\r";
							os << "<defaultDuration>" << ToXsdDuration (cp->getTransferDelay (from.get(), to.get())) << "</defaultDuration>" << "\r";
							os << "</ConnectionLink>" << "\r";
						}

					}
				}
			}
		    
			// --------------------------------------------------- Timetable
			{
				// One timetable per service
				BOOST_FOREACH(shared_ptr<NonPermanentService> srv, services)
				{
					os << "<Timetable>" << "\r";
					os << "<objectId>" << TridentId (peerid, "Timetable", srv.get()) << "</objectId>" << "\r";
	
					BOOST_FOREACH(Date date, ServiceDateTableSync::GetDatesOfService(srv->getId()))
					{
						os << "<calendarDay>" << ToXsdDate (date) << "</calendarDay>" << "\r";
					}
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\r";

					os << "</Timetable>" << "\r";
				}
			}


			// --------------------------------------------------- TimeSlot
			{
				BOOST_FOREACH(shared_ptr<NonPermanentService> srv, services)
				{
					shared_ptr<ContinuousService> csrv(dynamic_pointer_cast<ContinuousService, Service>(srv));
					if (!csrv.get()) continue;

					string timeSlotId;
					timeSlotId = TridentId(peerid, "TimeSlot", srv.get());

					os << "<TimeSlot>" << "\r";
					os << "<objectId>" << timeSlotId << "</objectId>" << "\r";
					os << "<beginningSlotTime>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</beginningSlotTime>" << "\r";
					os << "<endSlotTime>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</endSlotTime>" << "\r";
					os << "<firstDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureBeginScheduleToIndex(0).getHour()) << "</firstDepartureTimeInSlot>" << "\r";
					os << "<lastDepartureTimeInSlot>" << ToXsdTime(csrv->getDepartureEndScheduleToIndex(0).getHour()) << "</lastDepartureTimeInSlot>" << "\r";
					os << "</TimeSlot>" << "\r";
				}
			}


			// --------------------------------------------------- ChouetteLineDescription
			{
			os << "<ChouetteLineDescription>" << "\r";
			
			// --------------------------------------------------- Line
			{
				os << "<Line>" << "\r";
				os << "<objectId>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</objectId>" << "\r";
				os << "<name>" << _commercialLine->getName () << "</name>" << "\r";
				os << "<number>" << _commercialLine->getShortName () << "</number>" << "\r";
				os << "<publishedName>" << _commercialLine->getLongName () << "</publishedName>" << "\r";
				
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
				os << "<transportModeName>" << tm << "</transportModeName>" << "\r";
			    
				BOOST_FOREACH(shared_ptr<Line> line, lines)
				{
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << "\r";
				}
				os << "<registration>" << "\r";
				os << "<registrationNumber>" << Conversion::ToString (_commercialLine->getKey ()) << "</registrationNumber>" << "\r";
				os << "</registration>" << "\r";

				os << "</Line>" << "\r";
			}

			// --------------------------------------------------- ChouetteRoute
			{
				BOOST_FOREACH(shared_ptr<Line> line, lines)
				{
					vector<shared_ptr<LineStop> > lineLineStops(LineStopTableSync::Search(line->getKey()));
					
					if (lineLineStops.size() < 2) continue;

					os << "<ChouetteRoute>" << "\r";
					os << "<objectId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</objectId>" << "\r";
					os << "<name>" << line->getName () << "</name>" << "\r";
					
					os << "<publishedName>";
					{
						const PhysicalStop* ps(static_cast<const PhysicalStop*>(lineLineStops[0]->getFromVertex()));
						if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
							os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
					}
					os << " -&gt; ";
					{
						const PhysicalStop* ps(static_cast<const PhysicalStop*>(lineLineStops[lineLineStops.size() - 1]->getFromVertex()));
						if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
							os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
					}
					os << "</publishedName>" << "\r";
					
					BOOST_FOREACH(shared_ptr<LineStop> lineStop, lineLineStops)
					{
						if (lineStop->getNextInPath () == 0) continue;
						os << "<ptLinkId>" << TridentId (peerid, "PtLink", lineStop->getKey ()) << "</ptLinkId>" << "\r";
					}
					os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</journeyPatternId>" << "\r";
					
					// Wayback
					int wayback(line->getWayBack() ? 1 : 0);
					if (_withTisseoExtension)
						++wayback;

					os << "<RouteExtension><wayBack>";
					if (!logic::indeterminate(line->getWayBack()))
						os << Conversion::ToString(wayback);
					os << "</wayBack></RouteExtension>" << "\r";
					os << "</ChouetteRoute>" << "\r";
				}
			}

			// --------------------------------------------------- StopPoint
			{
				BOOST_FOREACH(shared_ptr<LineStop> ls, lineStops)
				{
					const PhysicalStop* ps = (const PhysicalStop*) ls->getFromVertex ();

					os << "<StopPoint" << (_withTisseoExtension ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << "\r";
					os << "<objectId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</objectId>" << "\r";
					os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\r";

					Point2D pt (ps->getX (), ps->getY ());
					GeoPoint gp = FromLambertIIe (pt);
					
					os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\r";
					os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\r";
					os << "<longLatType>" << "WGS84" << "</longLatType>" << "\r";
					
					os << "<projectedPoint>" << "\r";
					os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << "\r";
					os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << "\r";
					os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\r";
					os << "</projectedPoint>" << "\r";


					os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << "\r";
					os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
						ps->getConnectionPlace ()->getName ();
					if (ps->getName ().empty () == false) os << " (" + ps->getName () + ")";
					os << "</name>" << "\r";
					
					os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\r";
					os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", tn->getKey ()) << "</ptNetworkIdShortcut>" << "\r";

					if (_withTisseoExtension)
					{
						os << "<mobilityRestrictedSuitability>0</mobilityRestrictedSuitability>" << "\r";
					}
					os << "</StopPoint>" << "\r";
				}

			}
			
			// --------------------------------------------------- ITL
			// Not implemented
			{

			}


			// --------------------------------------------------- PtLink
			{
				for (vector<shared_ptr<LineStop> >::const_iterator its(lineStops.begin()); (its+1) != lineStops.end(); ++its)
				{
					const LineStop& from(**its);
					const LineStop& to(**(its+1));
					
					os << "<PtLink>" << "\r";
					os << "<objectId>" << TridentId (peerid, "PtLink", from.getKey ()) << "</objectId>" << "\r";
					os << "<startOfLink>" << TridentId (peerid, "StopPoint", from.getKey ()) << "</startOfLink>" << "\r";
					os << "<endOfLink>" << TridentId (peerid, "StopPoint", to.getKey ()) << "</endOfLink>" << "\r";
					os << "<linkDistance>" << Conversion::ToString (from.getLength ()) << "</linkDistance>" << "\r";   // in meters!
					os << "</PtLink>" << "\r";
				}
			}

			// --------------------------------------------------- JourneyPattern
			{
				// One per route 
				BOOST_FOREACH(shared_ptr<Line> line, lines)
				{
					if (line->getEdges().empty())
						continue;

					os << "<JourneyPattern>" << "\r";
					os << "<objectId>" << TridentId (peerid, "JourneyPattern", line->getKey ()) << "</objectId>" << "\r";
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</routeId>" << "\r";

					const vector<Edge*>& edges = line->getEdges ();
					os << "<origin>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (0))->getKey ()) << "</origin>" << "\r";
					os << "<destination>" << TridentId (peerid, "StopPoint", ((const LineStop*) edges.at (edges.size ()-1))->getKey ()) << "</destination>" << "\r";

					for (vector<Edge*>::const_iterator itedge = edges.begin ();
						 itedge != edges.end (); ++itedge)
					{
						const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
						os << "<stopPointList>" << TridentId (peerid, "StopPoint", lineStop->getKey ()) << "</stopPointList>" << "\r";
					}

					os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\r";
					os << "</JourneyPattern>" << "\r";
				}
			}

			// --------------------------------------------------- VehicleJourney
			{
				BOOST_FOREACH(shared_ptr<NonPermanentService> srv, services)
				{
					shared_ptr<ScheduledService> ssrv(dynamic_pointer_cast<ScheduledService, NonPermanentService>(srv));
					shared_ptr<ContinuousService> csrv(dynamic_pointer_cast<ContinuousService, NonPermanentService>(srv));

					bool isDRT(srv->getReservationRule()->isCompliant() != false);

					os << "<VehicleJourney";
					if (_withTisseoExtension)
					{
						os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
					}
					os << ">" << "\r";
					os << "<objectId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</objectId>" << "\r";
					os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\r";
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeId>" << "\r";
					os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPathId()) << "</journeyPatternId>" << "\r";
					os << "<lineIdShortcut>" << TridentId (peerid, "Line", _commercialLine->getKey ()) << "</lineIdShortcut>" << "\r";
					os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeIdShortcut>" << "\r";
					if (!srv->getServiceNumber().empty())
					{
						os << "<number>" << srv->getServiceNumber() << "</number>" << "\r";
					}

					// --------------------------------------------------- VehicleJourneyAtStop
					{
						vector<shared_ptr<LineStop> > edges(LineStopTableSync::Search(srv->getPathId()));
						BOOST_FOREACH(shared_ptr<LineStop> ls, edges)
						{
							os << "<vehicleJourneyAtStop>" << "\r";
							os << "<stopPointId>" << TridentId (peerid, "StopPoint", ls->getKey ()) << "</stopPointId>" << "\r";
							os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", srv.get()) << "</vehicleJourneyId>" << "\r";

							if (ssrv)
							{
								if (ls->getRankInPath() > 0 && ls->isArrival())
									os << "<arrivalTime>" << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour ()) 
									<< "</arrivalTime>" << "\r";
								
								os	<< "<departureTime>";
								if (ls->getRankInPath() != edges.size () - 1 && ls->isDeparture())
								{
									os << ToXsdTime (srv->getDepartureBeginScheduleToIndex (ls->getRankInPath()).getHour());
								}
								else
								{
									os << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour());
								}
								os	<< "</departureTime>" << "\r";
							}
							if (csrv)
							{
								const Schedule& schedule((ls->getRankInPath() > 0 && ls->isArrival()) ? srv->getArrivalBeginScheduleToIndex(ls->getRankInPath()) : srv->getDepartureBeginScheduleToIndex(ls->getRankInPath()));
								os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(0)) << "</elapseDuration>" << "\r";
								os << "<headwayFrequency>" << ToXsdDuration(csrv->getMaxWaitingTime()) << "</headwayFrequency>" << "\r";
							}

							os << "</vehicleJourneyAtStop>" << "\r";
						}
					}
					if (csrv) // Continuous service
					{
						os << "<timeSlotId>" << TridentId(peerid, "TimeSlot", srv.get()) << "</timeSlotId>" << "\r";
					}

					if (_withTisseoExtension)
					{
						os << "<mobilityRestrictedSuitability>" << Conversion::ToString(srv->getHandicappedCompliance()->isCompliant() != false) << "</mobilityRestrictedSuitability>" << "\r";
						if (srv->getHandicappedCompliance()->getReservationRule())
							os << "<mobilityRestrictedSuitabilityReservationRule>" <<  TridentId(peerid, "ReservationRule", srv->getHandicappedCompliance()->getReservationRule()->getKey()) << "</mobilityRestrictedSuitabilityReservationRule>" << "\r";
						os << "<bikeSuitability>" << Conversion::ToString(srv->getBikeCompliance()->isCompliant() != false) << "</bikeSuitability>" << "\r";
						if (srv->getBikeCompliance()->getReservationRule())
							os << "<bikeReservationRule>" << TridentId(peerid, "ReservationRule", srv->getBikeCompliance()->getReservationRule()->getKey()) << "</bikeReservationRule>" << "\r";
						if (isDRT)
						{
							os << "<reservationRule>" << TridentId(peerid, "ReservationRule", srv->getReservationRule()->getKey()) << "</reservationRule>" << "\r";
						}
					}
					os << "</VehicleJourney>" << "\r";
				}
			}


			os << "</ChouetteLineDescription>" << "\r";
			}


			if (_withTisseoExtension)
			{
				// Reservation Rules -----------------------------------------------------------------------

				BOOST_FOREACH(ReservationRules::value_type r, reservationRules)
				{
					const ReservationRule& rule(*r.second);

					if (rule.isCompliant() == false || (rule.getMinDelayDays() == 0 && rule.getMinDelayMinutes() == 0))	continue;

					os << "<ReservationRule>" << "\r";
					os << "<objectId>" << TridentId (peerid, "ReservationRule", rule.getKey ()) << "</objectId>" << "\r";
					os << "<ReservationCompulsory>" << (rule.isCompliant() == true ? "compulsory" : "optional") << "</ReservationCompulsory>" << "\r";
					os << "<deadLineIsTheCustomerDeparture>" << Conversion::ToString(!rule.getOriginIsReference()) << "</deadLineIsTheCustomerDeparture>" << "\r";
					if (rule.getMinDelayMinutes() > 0)
					{
						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << "\r";
					}
					if (rule.getMinDelayDays() > 0)
					{
						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << "\r";
					}
					if (!rule.getHourDeadLine().isUnknown())
					{
						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << "\r";
					}
					if (rule.getMaxDelayDays() > 0)
					{
						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << "\r";
					}
					if (!rule.getPhoneExchangeNumber().empty())
					{
						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << "\r";
						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << "\r";
					}
					if (!rule.getWebSiteUrl().empty())
					{
						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << "\r";
					}
					os << "</ReservationRule>" << "\r";
				}

				// Non concurrency -----------------------------------------------------------------------
				vector<shared_ptr<NonConcurrencyRule> > rules(NonConcurrencyRuleTableSync::Search(_commercialLine->getKey(), _commercialLine->getKey(), false));
				BOOST_FOREACH(shared_ptr<NonConcurrencyRule> rule, rules)
				{
					os << "<LineConflict>" << "\r";
					os << "<objectId>" << TridentId (peerid, "LineConflict", rule->getKey ()) << "</objectId>" << "\r";
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule->getHiddenLine()) << "</forbiddenLine>" << "\r";
					os << "<usedLine>" << TridentId (peerid, "Line", rule->getPriorityLine()) << "</usedLine>" << "\r";
					os << "<conflictDelay>" << ToXsdDuration(rule->getDelay()) << "</conflictDelay>" << "\r";
					os << "</LineConflict>" << "\r";
				}


				// CityMainStops --------------------------------------------------- 
				BOOST_FOREACH(Cities::value_type cip, cities)
				{
					const City& ci(*cip.second);

					vector<string> containedStopAreas;

					// Contained connection places
					ConnectionPlaceTableSync::SearchResult mainConnectionPlaces(ConnectionPlaceTableSync::Search(ci.getKey(), true));
					BOOST_FOREACH(shared_ptr<const PublicTransportStopZoneConnectionPlace> cp, mainConnectionPlaces)
					{
						// filter physical stops not concerned by this line.
						if (connectionPlaces.find(cp->getKey()) == connectionPlaces.end ()) continue;

						containedStopAreas.push_back (TridentId (peerid, "StopArea", cp->getKey()));

					}
					if (containedStopAreas.size () == 0) continue;


					os << "<CityMainStops>" << "\r";
					os << "<objectId>" << TridentId (peerid, "CityMainStops", ci.getKey ()) << "</objectId>" << "\r";
					os << "<name>" << ci.getName () << "</name>" << "\r";


					BOOST_FOREACH(string sa, containedStopAreas)
					{
						os << "<contains>" << sa  << "</contains>" << "\r";
					}

					os << "</CityMainStops>" << "\r";
				}
			}


			if (_withTisseoExtension)
				os << "</TisseoPTNetwork>" <<  "\r" << flush;
			else
				os << "</ChouettePTNetwork>" <<  "\r" << flush;
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
			
			ss << peer << ":" << clazz << ":" << service->getPathId() << "s" << skey ;

			return ss.str ();
		}



		TridentExport::~TridentExport()
		{

		}
	}
}

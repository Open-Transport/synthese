
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
#include "DataSource.h"
#include "SQLite.h"
#include "DBModule.h"
// 35 PT
#include "TridentFileFormat.h"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
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
#include "TransportNetworkTableSync.h"
#include "City.h"
#include "Service.h"
#include "RollingStock.h"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "UseRules.h"
#include "ServiceDate.h"

// 06 Geometry
#include "Projection.h"
#include "Point2D.h"

// 04 Time
#include "Date.h"
#include "Hour.h"

// 01 Util
#include "Conversion.h"
#include "XmlToolkit.h"

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
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::TridentFileFormat>::FACTORY_KEY("Trident");
	}


	namespace pt
	{
		TridentFileFormat::TridentFileFormat(
			Env* env,
			RegistryKeyType lineId,
			bool withTisseoExtension
		):	FactorableTemplate<FileFormat,TridentFileFormat>(),
			_commercialLineId(lineId),
			_withTisseoExtension(withTisseoExtension)
		{
			_env = env;
		}


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


		void TridentFileFormat::build(
			ostream& os
		){
			static const string peerid ("SYNTHESE");

			os.imbue (locale(""));
			// os.imbue (locale("en_US.ISO-8859-15"));
			cerr << "locale = " << os.getloc ().name () << "\n";

			// Collect all data related to selected commercial line
			shared_ptr<CommercialLine> _commercialLine(
				CommercialLineTableSync::GetEditable(_commercialLineId, *_env, UP_LINKS_LOAD_LEVEL)
			);
			LineTableSync::Search(
				*_env,
				_commercialLine->getKey(),
				UNKNOWN_VALUE,
				0, 0, true, true, UP_LINKS_LOAD_LEVEL
			);
			NonConcurrencyRuleTableSync::Search(
				*_env, _commercialLine->getKey(), _commercialLine->getKey(), false
			);

			// Lines
			const RollingStock* rollingStock(NULL);
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
			{
				if (line->getRollingStock())
					rollingStock = line->getRollingStock();
				LineStopTableSync::Search(
					*_env,
					line->getKey(),
					UNKNOWN_VALUE,
					0, 0, true, true,
					UP_LINKS_LOAD_LEVEL
				);
				ScheduledServiceTableSync::Search(
					*_env,
					line->getKey(),
					UNKNOWN_VALUE,
					_dataSource->getKey(),
					Date(TIME_UNKNOWN),
					0, 0, true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
				ContinuousServiceTableSync::Search(
					*_env,
					line->getKey(),
					0, 0, true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
			}


			// Writing of the header
			os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << "\n" << "\n";
		    if (_withTisseoExtension)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident http://www.reseaux-conseil.com/trident/tisseo-chouette-extension.xsd'>" << "\n";
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident Chouette.xsd'>" << "\n";

			// --------------------------------------------------- PTNetwork 
			const TransportNetwork* tn(_commercialLine->getNetwork());
			os << "<PTNetwork>" << "\n";
			os << "<objectId>" << TridentId (peerid, "PTNetwork", *tn) << "</objectId>" << "\n";
			os << "<versionDate>" << ToXsdDate (Date (TIME_CURRENT)) << "</versionDate>" << "\n";
			os << "<name>" << tn->getName () << "</name>" << "\n";
			os << "<registration>" << "\n";
			os << "<registrationNumber>" << Conversion::ToString (tn->getKey ()) << "</registrationNumber>" << "\n";
			os << "</registration>" << "\n";
			os << "<lineId>" << TridentId (peerid, "Line", *_commercialLine) << "</lineId>" << "\n";
			os << "<comment/>" << "\n";
			os << "</PTNetwork>" << "\n";

			// --------------------------------------------------- GroupOfLine


			// --------------------------------------------------- Company
			os << "<Company>" << "\n";
			os << "<objectId>SYNTHESE:Company:1</objectId>" << "\n";
			os << "<name>Tiss�o R�seau Urbain</name>" << "\n";
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
			BOOST_FOREACH(shared_ptr<PhysicalStop> ps, _env->getRegistry<PhysicalStop>())
			{
				if ((ps->getDepartureEdges ().size () == 0) && (ps->getArrivalEdges ().size () == 0)) continue;

				os << "<StopArea>" << "\n";

				os << "<objectId>" << TridentId (peerid, "StopArea", *ps) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\n";

				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
					ps->getConnectionPlace ()->getName ();
				if (!ps->getName().empty()) os << " (" + ps->getName () + ")";
				os << "</name>" << "\n";

				Vertex::Edges edges(ps->getDepartureEdges());
				edges.insert(ps->getArrivalEdges().begin(), ps->getArrivalEdges().end());
				BOOST_FOREACH(const Edge* ls, edges)
				{
					os << "<contains>" << TridentId (peerid, "StopPoint", *ls)  << "</contains>" << "\n";
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", *ps) << "</centroidOfArea>" << "\n";
				os << "<StopAreaExtension>" << "\n";
				os << "<areaType>" << "Quay" << "</areaType>" << "\n";
				string rn = ps->getOperatorCode ();
				if (rn.empty ()) rn = "0";
				os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << "\n";
				os << "</StopAreaExtension>" << "\n";
				os << "</StopArea>" << "\n";
			    
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			BOOST_FOREACH(shared_ptr<PublicTransportStopZoneConnectionPlace> cp, _env->getRegistry<PublicTransportStopZoneConnectionPlace>())
			{
				os << "<StopArea>" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopArea", *cp) << "</objectId>" << "\n";
				os << "<name>" << cp->getCity ()->getName () << " " << cp->getName () << "</name>" << "\n";

				// Contained physical stops
				const PhysicalStops& stops(cp->getPhysicalStops());
				for(PhysicalStops::const_iterator it(stops.begin()); it != stops.end(); ++it)
				{
					os << "<contains>" << TridentId (peerid, "StopArea", *it->second)  << "</contains>" << "\n";
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
			BOOST_FOREACH(shared_ptr<PhysicalStop> ps, _env->getRegistry<PhysicalStop>())
			{
				os << "<AreaCentroid>" << "\n";
				os << "<objectId>" << TridentId (peerid, "AreaCentroid", *ps) << "</objectId>" << "\n";
			    
				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = FromLambertIIe (pt);
			    
				os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\n";
				os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";

				// we do not provide full addresses right now.
				os << "<address><countryCode>" << ps->getConnectionPlace()->getCity()->getCode() << "</countryCode></address>";

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
			BOOST_FOREACH(shared_ptr<PublicTransportStopZoneConnectionPlace> cp, _env->getRegistry<PublicTransportStopZoneConnectionPlace>())
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
			BOOST_FOREACH(shared_ptr<ScheduledService> srv, _env->getRegistry<ScheduledService>())
			{
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const Date& date, srv->getActiveDates())
				{
					os << "<calendarDay>" << ToXsdDate (date) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}
			BOOST_FOREACH(shared_ptr<ContinuousService> srv, _env->getRegistry<ContinuousService>())
			{
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const Date& date, srv->getActiveDates())
				{
					os << "<calendarDay>" << ToXsdDate (date) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}


			// --------------------------------------------------- TimeSlot
			BOOST_FOREACH(shared_ptr<ContinuousService> csrv, _env->getRegistry<ContinuousService>())
			{
				string timeSlotId;
				timeSlotId = TridentId(peerid, "TimeSlot", *csrv);

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
				os << "<objectId>" << TridentId (peerid, "Line", *_commercialLine) << "</objectId>" << "\n";
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
			    
				BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
				{
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", *line) << "</routeId>" << "\n";
				}
				os << "<registration>" << "\n";
				os << "<registrationNumber>" << Conversion::ToString (_commercialLine->getKey ()) << "</registrationNumber>" << "\n";
				os << "</registration>" << "\n";

				os << "</Line>" << "\n";
			}

			// --------------------------------------------------- ChouetteRoute
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
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
				
				const Edge* from(NULL);
				BOOST_FOREACH(const Edge* to, line->getEdges())
				{
					if (from != NULL)
					{
						os << "<ptLinkId>" << TridentId (peerid, "PtLink", *from) << "</ptLinkId>" << "\n";
					}
					from = to;
				}


				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", *line) << "</journeyPatternId>" << "\n";
				
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
			BOOST_FOREACH(shared_ptr<LineStop> ls, _env->getRegistry<LineStop>())
			{
				const PhysicalStop* ps = static_cast<const PhysicalStop*>(ls->getFromVertex());

				os << "<StopPoint" << (_withTisseoExtension ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopPoint", *ls) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getOperatorCode() << "</creatorId>" << "\n";

				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = FromLambertIIe (pt);
				
				os << "<longitude>" << Conversion::ToString (gp.getLongitude ()) << "</longitude>" << "\n";
				os << "<latitude>" << Conversion::ToString (gp.getLatitude ()) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";
				
				os << "<address><countryCode>" << ps->getConnectionPlace()->getCity()->getCode() << "</countryCode></address>";

				os << "<projectedPoint>" << "\n";
				os << "<X>" << Conversion::ToString (pt.getX ()) << "</X>" << "\n";
				os << "<Y>" << Conversion::ToString (pt.getY ()) << "</Y>" << "\n";
				os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\n";
				os << "</projectedPoint>" << "\n";


				os << "<containedIn>" << TridentId (peerid, "StopArea", *ps) << "</containedIn>" << "\n";
				os << "<name>" << ps->getConnectionPlace ()->getCity ()->getName () << " " << 
					ps->getConnectionPlace ()->getName ();
				if (ps->getName ().empty () == false) os << " (" + ps->getName () + ")";
				os << "</name>" << "\n";
				
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_commercialLine) << "</lineIdShortcut>" << "\n";
				os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", *tn) << "</ptNetworkIdShortcut>" << "\n";

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
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
			{
				const Edge* from(NULL);
				BOOST_FOREACH(const Edge* to, line->getEdges())
				{
					if (from != NULL)
					{
						os << "<PtLink>" << "\n";
						os << "<objectId>" << TridentId (peerid, "PtLink", *from) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopPoint", *from) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopPoint", *to) << "</endOfLink>" << "\n";
						os << "<linkDistance>" << Conversion::ToString(from->getLength()) << "</linkDistance>" << "\n";   // in meters!
						os << "</PtLink>" << "\n";
					}
					from = to;
				}
			}		

			// --------------------------------------------------- JourneyPattern
			// One per route 
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
			{
				if (line->getEdges().empty())
					continue;

				os << "<JourneyPattern>" << "\n";
				os << "<objectId>" << TridentId (peerid, "JourneyPattern", *line) << "</objectId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", *line) << "</routeId>" << "\n";

				const vector<Edge*>& edges = line->getEdges ();
				os << "<origin>" << TridentId (peerid, "StopPoint", *edges.at(0)) << "</origin>" << "\n";
				os << "<destination>" << TridentId (peerid, "StopPoint", *edges.at(edges.size()-1)) << "</destination>" << "\n";

				for (vector<Edge*>::const_iterator itedge = edges.begin ();
					 itedge != edges.end (); ++itedge)
				{
					const LineStop* lineStop = dynamic_cast<const LineStop*> (*itedge);
					os << "<stopPointList>" << TridentId (peerid, "StopPoint", *lineStop) << "</stopPointList>" << "\n";
				}

				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_commercialLine) << "</lineIdShortcut>" << "\n";
				os << "</JourneyPattern>" << "\n";
			}
		
			// --------------------------------------------------- VehicleJourney
			BOOST_FOREACH(shared_ptr<ScheduledService> srv, _env->getRegistry<ScheduledService>())
			{
				bool isDRT(
					srv->getUseRule(USER_PEDESTRIAN).getReservationType() != UseRule::RESERVATION_FORBIDDEN
				);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", *srv) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPathId()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_commercialLine) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop
				Env lenv;
				LineStopTableSync::Search(lenv, srv->getPathId());
				BOOST_FOREACH(shared_ptr<LineStop> ls, lenv.getRegistry<LineStop>())
				{
					os << "<vehicleJourneyAtStop>" << "\n";
					os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

					if (ls->getRankInPath() > 0 && ls->isArrival())
						os << "<arrivalTime>" << ToXsdTime (srv->getArrivalBeginScheduleToIndex (ls->getRankInPath()).getHour ()) 
						<< "</arrivalTime>" << "\n";

					os	<< "<departureTime>";
					if (ls->getRankInPath() != lenv.getRegistry<LineStop>().size () - 1 && ls->isDeparture())
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
					const UseRules* rules(srv->getActualRules());
					const UseRule& hRule(rules->getUseRule(USER_HANDICAPPED));
					os <<
						"<mobilityRestrictedSuitability>" <<
						Conversion::ToString(hRule.getAccess()) <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
/*					if(	hRule.getReservationType() != UseRule::RESERVATION_FORBIDDEN)
					{
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								srv->getHandicappedCompliance()->getReservationRule()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}*/
					const UseRule& bRule(srv->getUseRule(USER_BIKE_IN_PT));
					os <<
						"<bikeSuitability>" <<
						Conversion::ToString(bRule.getAccess()) <<
						"</bikeSuitability>" <<
						"\n"
					;
/*					if (bRule.getReservationType() != UseRule::RESERVATION_FORBIDDEN)
					{
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								*srv->getBikeCompliance()->getReservationRule()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}*/
/*					if (isDRT)
					{
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", *srv->getReservationRule()) << "</reservationRule>" << "\n";
					}*/
				}
				os << "</VehicleJourney>" << "\n";
			}

			BOOST_FOREACH(shared_ptr<ContinuousService> srv, _env->getRegistry<ContinuousService>())
			{
				bool isDRT(
					srv->getUseRule(USER_PEDESTRIAN).getReservationType() != UseRule::RESERVATION_FORBIDDEN
				);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", *srv) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPathId()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_commercialLine) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPathId()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop
				{
					Env lenv;
					LineStopTableSync::Search(lenv, srv->getPathId());
					BOOST_FOREACH(shared_ptr<LineStop> ls, lenv.getRegistry<LineStop>())
					{
						os << "<vehicleJourneyAtStop>" << "\n";
						os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
						os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

						const Schedule& schedule((ls->getRankInPath() > 0 && ls->isArrival()) ? srv->getArrivalBeginScheduleToIndex(ls->getRankInPath()) : srv->getDepartureBeginScheduleToIndex(ls->getRankInPath()));
						os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(0)) << "</elapseDuration>" << "\n";
						os << "<headwayFrequency>" << ToXsdDuration(srv->getMaxWaitingTime()) << "</headwayFrequency>" << "\n";

						os << "</vehicleJourneyAtStop>" << "\n";
					}
				}
				os << "<timeSlotId>" << TridentId(peerid, "TimeSlot", *srv) << "</timeSlotId>" << "\n";

				if (_withTisseoExtension)
				{
					const UseRule& hRule(srv->getUseRule(USER_HANDICAPPED));
					os <<
						"<mobilityRestrictedSuitability>" <<
						Conversion::ToString(hRule.getAccess()) <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
/*					if(	hRule.getReservationType() != UseRule::RESERVATION_FORBIDDEN)
					{
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								srv->getHandicappedCompliance()->getReservationRule()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}*/
					const UseRule& bRule(srv->getUseRule(USER_BIKE_IN_PT));
					os <<
						"<bikeSuitability>" <<
						Conversion::ToString(bRule.getAccess()) <<
						"</bikeSuitability>" <<
						"\n"
					;
/*					if (bRule.getReservationType() != UseRule::RESERVATION_FORBIDDEN)
					{
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								*srv->getBikeCompliance()->getReservationRule()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}*/
/*					if (isDRT)
					{
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", *srv->getReservationRule()) << "</reservationRule>" << "\n";
					}*/
				}
				os << "</VehicleJourney>" << "\n";
			}


			os << "</ChouetteLineDescription>" << "\n";
			}


			if (_withTisseoExtension)
			{
				// Reservation Rules -----------------------------------------------------------------------

// 				BOOST_FOREACH(shared_ptr<ReservationRule> r, _env->getRegistry<ReservationRule>())
// 				{
// 					const ReservationRule& rule(*r);
// 
// 					if (rule.getType() == RESERVATION_FORBIDDEN || (rule.getMinDelayDays() == 0 && rule.getMinDelayMinutes() == 0))	continue;
// 
// 					os << "<ReservationRule>" << "\n";
// 					os << "<objectId>" << TridentId (peerid, "ReservationRule", rule.getKey ()) << "</objectId>" << "\n";
// 					os << "<ReservationCompulsory>" << ((rule.getType() == RESERVATION_COMPULSORY) ? "compulsory" : "optional") << "</ReservationCompulsory>" << "\n";
// 					os << "<deadLineIsTheCustomerDeparture>" << Conversion::ToString(!rule.getOriginIsReference()) << "</deadLineIsTheCustomerDeparture>" << "\n";
// 					if (rule.getMinDelayMinutes() > 0)
// 					{
// 						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << "\n";
// 					}
// 					if (rule.getMinDelayDays() > 0)
// 					{
// 						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << "\n";
// 					}
// 					if (!rule.getHourDeadLine().isUnknown())
// 					{
// 						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << "\n";
// 					}
// 					if (rule.getMaxDelayDays() > 0)
// 					{
// 						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << "\n";
// 					}
// 					if (!rule.getPhoneExchangeNumber().empty())
// 					{
// 						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << "\n";
// 						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << "\n";
// 					}
// 					if (!rule.getWebSiteUrl().empty())
// 					{
// 						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << "\n";
// 					}
// 					os << "</ReservationRule>" << "\n";
// 				}

				// Non concurrency -----------------------------------------------------------------------
				BOOST_FOREACH(shared_ptr<NonConcurrencyRule> rule, _env->getRegistry<NonConcurrencyRule>())
				{
					os << "<LineConflict>" << "\n";
					os << "<objectId>" << TridentId (peerid, "LineConflict", *rule) << "</objectId>" << "\n";
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule->getHiddenLine()) << "</forbiddenLine>" << "\n";
					os << "<usedLine>" << TridentId (peerid, "Line", rule->getPriorityLine()) << "</usedLine>" << "\n";
					os << "<conflictDelay>" << ToXsdDuration(rule->getDelay()) << "</conflictDelay>" << "\n";
					os << "</LineConflict>" << "\n";
				}


				// CityMainStops --------------------------------------------------- 
				BOOST_FOREACH(shared_ptr<City> city, _env->getRegistry<City>())
				{
					vector<string> containedStopAreas;

					// Contained connection places
					Env senv;
					ConnectionPlaceTableSync::Search(senv, city->getKey(), true);
					BOOST_FOREACH(shared_ptr<const PublicTransportStopZoneConnectionPlace> cp, senv.getRegistry<PublicTransportStopZoneConnectionPlace>())
					{
						// filter physical stops not concerned by this line.
						if(!_env->getRegistry<PublicTransportStopZoneConnectionPlace>().contains(cp->getKey())) continue;

						containedStopAreas.push_back (TridentId (peerid, "StopArea", *cp));

					}
					if (containedStopAreas.size () == 0) continue;


					os << "<CityMainStops>" << "\n";
					os << "<objectId>" << TridentId (peerid, "CityMainStops", *city) << "</objectId>" << "\n";
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


		string TridentFileFormat::TridentId(
			const string& peer,
			const string clazz,
			const uid& id
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << id;
			return ss.str ();
		}


		string TridentFileFormat::TridentId(
			const string& peer,
			const string clazz,
			const string& s
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << s;
			return ss.str ();
		}


		string TridentFileFormat::TridentId(
			const string& peer,
			const string clazz,
			const Registrable& obj
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << obj.getKey();
			return ss.str ();
		}



		void TridentFileFormat::_parse(
			const std::string& text,
			std::ostream& os
		){
			XMLNode allNode = XMLNode::parseString (text.c_str (), "ChouettePTNetwork");
			
			// Title
			XMLNode chouetteLineDescriptionNode(allNode.getChildNode("ChouetteLineDescription"));
			XMLNode lineNode(chouetteLineDescriptionNode.getChildNode("Line"));
			XMLNode clineNameNode = lineNode.getChildNode("name");
				
			os << "<h2>Trident import of " << clineNameNode.getText() << "</h2>";
			
			// Network
			XMLNode networkNode =  allNode.getChildNode("PTNetwork", 0);
			XMLNode networkIdNode = networkNode.getChildNode("objectId", 0);
			string key(networkIdNode.getText());
			
			
			shared_ptr<TransportNetwork> network;
			Env nenv;
			TransportNetworkTableSync::Search(nenv, string(), key);
			if(!nenv.getRegistry<TransportNetwork>().empty())
			{
				if(nenv.getRegistry<TransportNetwork>().size() > 1)
				{
					os << "WARN : more than one network with key " << key << "<br />";
				}
				network = TransportNetworkTableSync::GetEditable(
					nenv.getEditableRegistry<TransportNetwork>().front()->getKey(),
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
				
				os << "LOAD : use of existing network " << network->getKey() << " (" << network->getName() << ")<br />";
			}
			else
			{
				XMLNode networkNameNode = networkNode.getChildNode("name", 0);
				os << "CREA : Creation of the network with key " << key << " (" << networkNameNode.getText() <<  ")<br />";

				network.reset(new TransportNetwork);
				network->setName(networkNameNode.getText());
				network->setCreatorId(key);
				network->setKey(TransportNetworkTableSync::getId());
				_env->getEditableRegistry<TransportNetwork>().add(network);
			}
			
			// Commercial lines
			XMLNode lineKeyNode(lineNode.getChildNode("objectId"));
			
			string ckey(networkIdNode.getText());
			
			shared_ptr<CommercialLine> cline;
			Env cenv;
			CommercialLineTableSync::Search(cenv, network->getKey(), string("%"), ckey);
			if(!cenv.getRegistry<CommercialLine>().empty())
			{
				if(cenv.getRegistry<CommercialLine>().size() > 1)
				{
					os << "WARN : more than one commercial line with key " << ckey << "<br />";
				}
				cline = CommercialLineTableSync::GetEditable(
					cenv.getEditableRegistry<CommercialLine>().front()->getKey(),
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
				
				os << "LOAD : use of existing commercial line" << cline->getKey() << " (" << cline->getName() << ")<br />";

			}
			else
			{
				cline.reset(new CommercialLine);
				XMLNode clineShortNameNode = lineNode.getChildNode("number", 0);
				
				os << "CREA : Creation of the commercial line with key " << ckey << " (" << clineNameNode.getText() <<  ")<br />";
				
				cline->setNetwork(network.get());
				cline->setName(clineNameNode.getText());
				cline->setCreatorId(ckey);
				cline->setShortName(clineShortNameNode.getText());
				cline->setKey(CommercialLineTableSync::getId());
				_env->getEditableRegistry<CommercialLine>().add(cline);
			}
			
			// Transport mode
			RegistryKeyType rollingStockId(UNKNOWN_VALUE);
			XMLNode rollingStockNode = lineNode.getChildNode("transportModeName");
			if(rollingStockNode.getText() == "RapidTransit") rollingStockId = 13792273858822590LL;
			else if(rollingStockNode.getText() == "LocalTrain") rollingStockId = 13792273858822159LL;
			else if(rollingStockNode.getText() == "LongDistanceTrain") rollingStockId = 13792273858822160LL;
			else if(rollingStockNode.getText() == "Coach") rollingStockId = 13792273858822584LL;
			else if(rollingStockNode.getText() == "Bus") rollingStockId = 13792273858822585LL;
			else if(rollingStockNode.getText() == "Metro") rollingStockId = 13792273858822586LL;
			else if(rollingStockNode.getText() == "Train") rollingStockId = 13792273858822587LL;
			else if(rollingStockNode.getText() == "Tramway") rollingStockId = 13792273858822588LL;

			// Stops
			map<string, PhysicalStop*> stops;
			XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
			int stopsNumber(chouetteAreaNode.nChildNode("StopArea"));
			bool failure(false);
			for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
			{
				XMLNode stopAreaNode(chouetteAreaNode.getChildNode("StopArea", stopRank));
				XMLNode extensionNode(stopAreaNode.getChildNode("StopAreaExtension", 0));
				XMLNode areaTypeNode(extensionNode.getChildNode("areaType",0));
				if(string(areaTypeNode.getText()) != string("BoardingPosition")) continue;
				
				XMLNode keyNode(stopAreaNode.getChildNode("objectId", 0));
				XMLNode nameNode(stopAreaNode.getChildNode("name", 0));
				string stopKey(keyNode.getText());
			
				Env nenv;
				PhysicalStopTableSync::Search(nenv, UNKNOWN_VALUE, stopKey);
				if(nenv.getRegistry<PhysicalStop>().empty())
				{
					os << "ERR  : stop not found " << stopKey << " (" << nameNode.getText() << ")<br />";
					failure = true;
					continue;
				}
				
				if(nenv.getRegistry<PhysicalStop>().size() > 1)
				{
					os << "WARN : more than one stop with key" << stopKey << "<br />";
				}
				
				RegistryKeyType stopId(nenv.getEditableRegistry<PhysicalStop>().front()->getKey());
				stops[stopKey] = PhysicalStopTableSync::GetEditable(stopId, *_env, UP_LINKS_LOAD_LEVEL).get();
				
				os << "LOAD : link between stops " << stopKey << " (" << nameNode.getText() << ") and "
					<< stops[stopKey]->getKey() << " (" << stops[stopKey]->getConnectionPlace()->getName() << ")<br />";
				
			}
			
		
			if(failure)
			{
				os << "<b>FAILURE : At least a stop is missing : load interrupted</b><br />";
				return;
			}

			// Line stops
			int stopPointsNumber(chouetteLineDescriptionNode.nChildNode("StopPoint"));
			map<string,PhysicalStop*> stopPoints;
			for(int stopPointRank(0); stopPointRank < stopPointsNumber; ++stopPointRank)
			{
				XMLNode stopPointNode(chouetteLineDescriptionNode.getChildNode("StopPoint", stopPointRank));
				XMLNode spKeyNode(stopPointNode.getChildNode("objectId"));
				XMLNode containedNode(stopPointNode.getChildNode("containedIn"));
				stopPoints[spKeyNode.getText()] = stops[containedNode.getText()];
			}
			
			// Load of existing routes
			LineTableSync::Search(*_env, cline->getKey(), _dataSource->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
			{
				LineStopTableSync::Search(*_env, line->getKey(), UNKNOWN_VALUE, 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
				ScheduledServiceTableSync::Search(*_env, line->getKey());
			}
			
			// Chouette routes
			map<string,string> routeNames;
			map<string,bool> routeWaybacks;
			int croutesNumber(chouetteLineDescriptionNode.nChildNode("ChouetteRoute"));
			for(int crouteRank(0); crouteRank < croutesNumber; ++crouteRank)
			{
				XMLNode routeNode(chouetteLineDescriptionNode.getChildNode("ChouetteRoute",crouteRank));
				XMLNode crouteKeyNode(routeNode.getChildNode("objectId"));
				XMLNode extNode(routeNode.getChildNode("RouteExtension"));
				XMLNode waybackNode(extNode.getChildNode("wayBack"));
				XMLNode nameNode(routeNode.getChildNode("name"));
				routeNames[crouteKeyNode.getText()] = nameNode.getText();
				routeWaybacks[crouteKeyNode.getText()] = (
					waybackNode.getText() == "R" ||
					waybackNode.getText() == "1"
				);
			}
			
			
			// Routes
			map<string,Line*> routes;
			int routesNumber(chouetteLineDescriptionNode.nChildNode("JourneyPattern"));
			for(int routeRank(0); routeRank < routesNumber; ++routeRank)
			{
				XMLNode routeNode(chouetteLineDescriptionNode.getChildNode("JourneyPattern",routeRank));
				XMLNode jpKeyNode(routeNode.getChildNode("objectId"));
				XMLNode routeIdNode(routeNode.getChildNode("routeId"));
				
				// Reading stops list
				vector<PhysicalStop*> routeStops;
				int lineStopsNumber(routeNode.nChildNode("stopPointList"));
				for(int lineStopRank(0); lineStopRank < lineStopsNumber; ++lineStopRank)
				{
					XMLNode lineStopNode(routeNode.getChildNode("stopPointList", lineStopRank));
					routeStops.push_back(stopPoints[lineStopNode.getText()]);
				}
				
				// Attempting to find an existing route
				shared_ptr<Line> route;
				BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
				{
					if(*line == routeStops)
					{
						route = line;
						continue;
					}
				}
				
				// Create a new route if necessary
				if(!route.get())
				{
					os << "CREA : Creation of route " << routeNames[routeIdNode.getText()] << " for " << routeIdNode.getText() << "<br />";
					route.reset(new Line);
					route->setCommercialLine(cline.get());
					route->setName(routeNames[routeIdNode.getText()]);
					route->setWayBack(routeWaybacks[routeIdNode.getText()]);
					route->setDataSource(_dataSource);
					route->setKey(LineTableSync::getId());
					_env->getEditableRegistry<Line>().add(route);
					
					int rank(0);
					BOOST_FOREACH(PhysicalStop* stop, routeStops)
					{
						shared_ptr<LineStop> ls(new LineStop);
						ls->setPhysicalStop(stop);
						ls->setRankInPath(rank);
						ls->setIsArrival(rank > 0);
						ls->setIsDeparture(rank < routeStops.size() - 1);
						ls->setLine(route.get());
						ls->setMetricOffset(0);
						ls->setKey(LineStopTableSync::getId());
						route->addEdge(ls.get());
						_env->getEditableRegistry<LineStop>().add(ls);
						
						++rank;
					}
				}
				else
				{
					os << "LOAD : Use of route " << route->getKey() << " (" << route->getName() << ") for " << routeIdNode.getText() << " (" << routeNames[routeIdNode.getText()] << ")<br />";

				}
				
				// Link with the route
				routes[jpKeyNode.getText()] = route.get();
			}
			
			// Services
			map<string, ScheduledService*> services;
			int servicesNumber(chouetteLineDescriptionNode.nChildNode("VehicleJourney"));
			for(int serviceRank(0); serviceRank < servicesNumber; ++serviceRank)
			{
				XMLNode serviceNode(chouetteLineDescriptionNode.getChildNode("VehicleJourney",serviceRank));
				XMLNode keyNode(serviceNode.getChildNode("objectId"));
				XMLNode jpKeyNode(serviceNode.getChildNode("journeyPatternId"));
				XMLNode numberNode(serviceNode.getChildNode("publishedJourneyName"));
				
				// Creation of the service
				Line* line(routes[jpKeyNode.getText()]);
				shared_ptr<ScheduledService> service(new ScheduledService);
				service->setPath(line);
				service->setServiceNumber(numberNode.getText());
				ScheduledService::Schedules deps;
				ScheduledService::Schedules arrs;
				int stopsNumber(serviceNode.nChildNode("VehicleJourneyAtStop"));
				Schedule lastDep(Hour(0,0,0),0);
				Schedule lastArr(Hour(0,0,0),0);
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode vjsNode(serviceNode.getChildNode("VehicleJourneyAtStop", stopRank));
					XMLNode depNode(vjsNode.getChildNode("departureTime"));
					XMLNode arrNode(vjsNode.getChildNode("arrivalTime"));
					Hour depHour(Hour::FromSQLTime(depNode.getText()));
					Hour arrHour(Hour::FromSQLTime(arrNode.getText()));
					Schedule depSchedule(depHour, lastDep.getDaysSinceDeparture() + (depHour < lastDep.getHour() ? 1 : 0));
					Schedule arrSchedule(arrHour, lastArr.getDaysSinceDeparture() + (arrHour < lastArr.getHour() ? 1 : 0));
					lastDep = depSchedule;
					lastArr = arrSchedule;
					deps.push_back(depSchedule);
					arrs.push_back(arrSchedule);
				}
				service->setDepartureSchedules(deps);
				service->setArrivalSchedules(arrs);
				
				// Search for a corresponding service
				ScheduledService* existingService(NULL);
				BOOST_FOREACH(Service* tservice, line->getServices())
				{
					ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));
					
					if(!curService) continue;
					
					if (*curService == *service)
					{
						existingService = curService;
						break;
					}
				}
				
				// If not found creation
				if(!existingService)
				{
					service->setKey(ScheduledServiceTableSync::getId());
					line->addService(service.get(), false);
					_env->getEditableRegistry<ScheduledService>().add(service);
					services[keyNode.getText()] = service.get();
					
					os << "CREA : Creation of service " << service->getServiceNumber() << " for " << keyNode.getText() << " (" << deps[0].toString() << ") on route " << line->getKey() << " (" << line->getName() << ")<br />";
				}
				else
				{
					services[keyNode.getText()] = existingService;
					
					os << "LOAD : Use of service " << existingService->getKey() << " (" << existingService->getServiceNumber() << ") for " << keyNode.getText() << " (" << deps[0].toString() << ") on route " << line->getKey() << " (" << line->getName() << ")<br />";

				}
			}
			
			// Calendars
			int calendarNumber(allNode.nChildNode("Timetable"));
			Date today(TIME_CURRENT);
			for(int calendarRank(0); calendarRank < calendarNumber; ++calendarRank)
			{
				XMLNode calendarNode(allNode.getChildNode("Timetable", calendarRank));
				
				int daysNumber(calendarNode.nChildNode("calendarDay"));
				int servicesNumber(calendarNode.nChildNode("vehicleJourneyId"));
				for(int dayRank(0); dayRank < daysNumber; ++dayRank)
				{
					XMLNode dayNode(calendarNode.getChildNode("calendarDay", dayRank));
					Date date(Date::FromSQLDate(dayNode.getText()));
					if(date < today) continue;
					
					for(int serviceRank(0); serviceRank < servicesNumber; ++serviceRank)
					{
						XMLNode serviceNode(calendarNode.getChildNode("vehicleJourneyId", serviceRank));
						shared_ptr<ServiceDate> sd(new ServiceDate);
						sd->setService(services[serviceNode.getText()]);
						sd->setDate(date);
						sd->setKey(ServiceDateTableSync::getId());
						_serviceDates.push_back(sd);
					}
				}
			}
			
			os << "<b>SUCCESS : Data loaded</b><br />";
		}
		
		void TridentFileFormat::save(std::ostream& os) const
		{
			DBModule::GetSQLite()->execUpdate("BEGIN TRANSACTION;");

			// Saving of each created or altered objects
			BOOST_FOREACH(shared_ptr<TransportNetwork> network, _env->getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.get());
			}
			BOOST_FOREACH(shared_ptr<CommercialLine> cline, _env->getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.get());
			}
			BOOST_FOREACH(shared_ptr<Line> line, _env->getRegistry<Line>())
			{
				LineTableSync::Save(line.get());
			}
			BOOST_FOREACH(shared_ptr<LineStop> lineStop, _env->getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.get());
			}
			BOOST_FOREACH(shared_ptr<ScheduledService> service, _env->getRegistry<ScheduledService>())
			{
				ServiceDateTableSync::DeleteDatesFromNow(service->getKey());
				ScheduledServiceTableSync::Save(service.get());
			}
			BOOST_FOREACH(shared_ptr<ServiceDate> date, _serviceDates)
			{
				ServiceDateTableSync::Save(date.get());
			}
			
			//TODO cleaning : delete services without dates and routes without service
						
			DBModule::GetSQLite()->execUpdate("COMMIT;");

			os << "<b>SUCCESS : Data saved.</b><br />";
		}



		TridentFileFormat::~TridentFileFormat()
		{

		}
	}
}

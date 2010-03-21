
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

#include "DataSource.h"
#include "SQLite.h"
#include "DBModule.h"
#include "TridentFileFormat.h"
#include "GraphConstants.h"
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
#include "CityTableSync.h"
#include "Service.h"
#include "RollingStock.h"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "PTUseRule.h"
#include "ServiceDate.h"
#include "PTConstants.h"
#include "Projection.h"
#include "Point2D.h"
#include "Conversion.h"
#include "XmlToolkit.h"
#include "SQLiteTransaction.h"

#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <utility>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::posix_time;


namespace synthese
{
	using namespace geography;
	using namespace geometry;
	using namespace env;
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace pt;
	using namespace server;
		

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::TridentFileFormat>::FACTORY_KEY("Trident");
	}

	namespace impex
	{
		template<> const FileFormat::Files FileFormatTemplate<TridentFileFormat>::FILES(
			""
		);
	}

	namespace pt
	{
		const string TridentFileFormat::PARAMETER_IMPORT_STOPS("impstp");
		const string TridentFileFormat::PARAMETER_WITH_OLD_DATES("wod");



		TridentFileFormat::TridentFileFormat(
			Env* env,
			RegistryKeyType lineId,
			bool withTisseoExtension
		):	FileFormatTemplate<TridentFileFormat>(),
			_commercialLineId(lineId),
			_withTisseoExtension(withTisseoExtension),
			_importStops(false),
			_withOldDates(false)
		{
			_env = env;
		}


		string ToXsdDaysDuration (date_duration daysDelay)
		{
			stringstream s;
			s << "P" << daysDelay.days() << "D";
			return s.str();
		}


		string ToXsdDuration(posix_time::time_duration duration)
		{
			stringstream s;
			s << "PT";
			if(duration.hours() > 0)
				s << duration.hours() << "H";
			s << duration.minutes() << "M";
			return s.str();
		}

		

		string ToXsdTime (const time_duration& time)
		{
			stringstream ss;
			ss << setw( 2 ) << setfill ( '0' )
			   << time.hours() << ":"
			   << setw( 2 ) << setfill ( '0' )
			   << time.minutes () << ":00";
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
				0,
				optional<size_t>(),
				true, true, UP_LINKS_LOAD_LEVEL
			);
			NonConcurrencyRuleTableSync::Search(
				*_env, _commercialLine->getKey(), _commercialLine->getKey(), false
			);

			// Lines
			const RollingStock* rollingStock(NULL);
			BOOST_FOREACH(Registry<Line>::value_type itline, _env->getRegistry<Line>())
			{
				const Line& line(*itline.second);
				if (line.getRollingStock())
					rollingStock = line.getRollingStock();
				LineStopTableSync::Search(
					*_env,
					line.getKey(),
					UNKNOWN_VALUE,
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
				ScheduledServiceTableSync::Search(
					*_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					optional<date>(),
					false,
					0,
					optional<size_t>(),
					true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
				BOOST_FOREACH(Registry<ScheduledService>::value_type itServ, _env->getRegistry<ScheduledService>())
				{
					ServiceDateTableSync::SetActiveDates(*itServ.second);
				}
				ContinuousServiceTableSync::Search(
					*_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
				BOOST_FOREACH(Registry<ContinuousService>::value_type itServC, _env->getRegistry<ContinuousService>())
				{
					ServiceDateTableSync::SetActiveDates(*itServC.second);
				}
			}


			// Writing of the header
			os << "<?xml version='1.0' encoding='ISO-8859-15'?>" << "\n" << "\n";
		    if (_withTisseoExtension)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.rcsmobility.com/synthese/include/35_pt/trident2-tisseo/tisseo-chouette-extension.xsd'>" << "\n";
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.rcsmobility.com/synthese/include/35_pt/chouette/Chouette.xsd'>" << "\n";

			// --------------------------------------------------- PTNetwork 
			const TransportNetwork* tn(_commercialLine->getNetwork());
			os << "<PTNetwork>" << "\n";
			os << "<objectId>" << TridentId (peerid, "PTNetwork", *tn) << "</objectId>" << "\n";
			os << "<versionDate>" << to_iso_extended_string(day_clock::local_day()) << "</versionDate>" << "\n";
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
			os << "<name>" << tn->getName() << "</name>" << "\n";
			os << "<shortName>" << tn->getName() << "</shortName>" << "\n";
			os << "<organisationalUnit></organisationalUnit>" << "\n";
			os << "<operatingDepartmentName></operatingDepartmentName>" << "\n";
			os << "<code>-</code>" << "\n";
			os << "<phone>-</phone>" << "\n";
			os << "<fax></fax>" << "\n";
			os << "<email></email>" << "\n";
			os << "<registration><registrationNumber>1</registrationNumber></registration>" << "\n";
			os << "</Company>" << "\n";

			os << "<ChouetteArea>" << "\n";

			// --------------------------------------------------- StopArea (type = BoardingPosition)
			// BoardingPosition corresponds to a very accurate location along a quay for instance.
			// Not implemented right now.

			// --------------------------------------------------- StopArea (type = Quay) <=> PhysicalStop
			BOOST_FOREACH(Registry<PhysicalStop>::value_type itps, _env->getRegistry<PhysicalStop>())
			{
				const PhysicalStop* ps(itps.second.get());
				if (ps->getDepartureEdges().empty() && ps->getArrivalEdges().empty()) continue;

				os << "<StopArea>" << "\n";

				os << "<objectId>" << TridentId (peerid, "StopArea", *ps) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getCodeBySource() << "</creatorId>" << "\n";

				os << "<name>" << ps->getConnectionPlace ()->getName ();
				if (!ps->getName().empty()) os << " (" + ps->getName () + ")";
				os << "</name>" << "\n";

				Vertex::Edges edges(ps->getDepartureEdges());
				edges.insert(ps->getArrivalEdges().begin(), ps->getArrivalEdges().end());
				BOOST_FOREACH(const Vertex::Edges::value_type& ls, edges)
				{
					os << "<contains>" << TridentId (peerid, "StopPoint", *ls.second)  << "</contains>" << "\n";
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", *ps) << "</centroidOfArea>" << "\n";
				os << "<StopAreaExtension>" << "\n";
				os << "<areaType>" << "Quay" << "</areaType>" << "\n";
				string rn = ps->getCodeBySource();
				if (rn.empty ()) rn = "0";
				os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << "\n";
				os << "</StopAreaExtension>" << "\n";
				os << "</StopArea>" << "\n";
			    
			}
		    
			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			BOOST_FOREACH(
				Registry<PublicTransportStopZoneConnectionPlace>::value_type itcp,
				_env->getRegistry<PublicTransportStopZoneConnectionPlace>()
			){
				const PublicTransportStopZoneConnectionPlace* cp(itcp.second.get());
				os << "<StopArea>" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopArea", *cp) << "</objectId>" << "\n";
				os << "<name>" << cp->getName () << "</name>" << "\n";

				// Contained physical stops
				const PublicTransportStopZoneConnectionPlace::PhysicalStops& stops(cp->getPhysicalStops());
				for(PublicTransportStopZoneConnectionPlace::PhysicalStops::const_iterator it(stops.begin()); it != stops.end(); ++it)
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
			BOOST_FOREACH(Registry<PhysicalStop>::value_type itps, _env->getRegistry<PhysicalStop>())
			{
				const PhysicalStop* ps(itps.second.get());
				os << "<AreaCentroid>" << "\n";
				os << "<objectId>" << TridentId (peerid, "AreaCentroid", *ps) << "</objectId>" << "\n";
			    

				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = WGS84FromLambert(pt);
				
				os << "<longitude>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(gp.getLongitude ())) << "</longitude>" << "\n";
				os << "<latitude>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(gp.getLatitude ())) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";

				// we do not provide full addresses right now.
				os << "<address><countryCode>" << ps->getConnectionPlace()->getCity()->getCode() << "</countryCode></address>";

				os << "<projectedPoint>" << "\n";
				os << "<X>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(pt.getX())) << "</X>" << "\n";
				os << "<Y>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(pt.getY())) << "</Y>" << "\n";
				os << "<projectionType>" << "LambertIIe" << "</projectionType>" << "\n";
				os << "</projectedPoint>" << "\n";

				os << "<containedIn>" << TridentId (peerid, "StopArea", ps->getKey ()) << "</containedIn>" << "\n";
				os << "<name>" << Conversion::ToString (ps->getKey ()) << "</name>" << "\n";

				os << "</AreaCentroid>" << "\n";
			}
		
			os << "</ChouetteArea>" << "\n";

			// --------------------------------------------------- ConnectionLink
			BOOST_FOREACH(
				Registry<PublicTransportStopZoneConnectionPlace>::value_type itcp,
				_env->getRegistry<PublicTransportStopZoneConnectionPlace>()
			){
				const PublicTransportStopZoneConnectionPlace& cp(*itcp.second);
				if(!cp.isConnectionPossible()) continue;
				
				// Contained physical stops
				const PublicTransportStopZoneConnectionPlace::PhysicalStops& stops(cp.getPhysicalStops());
				BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& it1, stops)
				{
					BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& it2, stops)
					{
						if(!cp.isConnectionAllowed(*it1.second, *it2.second))
							continue;

						os << "<ConnectionLink>" << "\n";
						stringstream clkey;
						clkey << it1.second->getKey () << "t" << it2.second->getKey ();
						os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopArea", it1.first) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopArea", it2.first) << "</endOfLink>" << "\n";
						os << "<defaultDuration>" << ToXsdDuration (cp.getTransferDelay (*it1.second, *it2.second)) << "</defaultDuration>" << "\n";
						os << "</ConnectionLink>" << "\n";
					}
				}
			}
			
			// --------------------------------------------------- Timetable
			// One timetable per service
			date today(day_clock::local_day());
			BOOST_FOREACH(Registry<ScheduledService>::value_type itsrv, _env->getRegistry<ScheduledService>())
			{
				const ScheduledService* srv(itsrv.second.get());
				
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const date& d, srv->getActiveDates())
				{
					if(!_withOldDates && d < today)
					{
						continue;
					}
					os << "<calendarDay>" << to_iso_extended_string(d) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}
			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env->getRegistry<ContinuousService>())
			{
				const ContinuousService* srv(itsrv.second.get());
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const date& d, srv->getActiveDates())
				{
					if(!_withOldDates && d < today)
					{
						continue;
					}
					os << "<calendarDay>" << to_iso_extended_string(d) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}


			// --------------------------------------------------- TimeSlot
			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env->getRegistry<ContinuousService>())
			{
				const ContinuousService* csrv(itsrv.second.get());
				string timeSlotId;
				timeSlotId = TridentId(peerid, "TimeSlot", *csrv);

				os << "<TimeSlot>" << "\n";
				os << "<objectId>" << timeSlotId << "</objectId>" << "\n";
				os << "<beginningSlotTime>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureBeginScheduleToIndex(false, 0))) << "</beginningSlotTime>" << "\n";
				os << "<endSlotTime>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureEndScheduleToIndex(false, 0))) << "</endSlotTime>" << "\n";
				os << "<firstDepartureTimeInSlot>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureBeginScheduleToIndex(false, 0))) << "</firstDepartureTimeInSlot>" << "\n";
				os << "<lastDepartureTimeInSlot>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureEndScheduleToIndex(false, 0))) << "</lastDepartureTimeInSlot>" << "\n";
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
			    
				BOOST_FOREACH(Registry<Line>::value_type line, _env->getRegistry<Line>())
				{
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", *line.second) << "</routeId>" << "\n";
				}
				os << "<registration>" << "\n";
				os << "<registrationNumber>" << Conversion::ToString (_commercialLine->getKey ()) << "</registrationNumber>" << "\n";
				os << "</registration>" << "\n";

				os << "</Line>" << "\n";
			}

			// --------------------------------------------------- ChouetteRoute
			BOOST_FOREACH(Registry<Line>::value_type itline, _env->getRegistry<Line>())
			{
				const Line* line(itline.second.get());
				
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
			BOOST_FOREACH(Registry<LineStop>::value_type itls, _env->getRegistry<LineStop>())
			{
				const LineStop* ls(itls.second.get());
				const PhysicalStop* ps = static_cast<const PhysicalStop*>(ls->getFromVertex());

				os << "<StopPoint" << (_withTisseoExtension ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopPoint", *ls) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getCodeBySource() << "</creatorId>" << "\n";

				Point2D pt (ps->getX (), ps->getY ());
				GeoPoint gp = WGS84FromLambert(pt);
				
				os << "<longitude>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(gp.getLongitude())) << "</longitude>" << "\n";
				os << "<latitude>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(gp.getLatitude())) << "</latitude>" << "\n";
				os << "<longLatType>" << "WGS84" << "</longLatType>" << "\n";
				
				os << "<address><countryCode>" << ps->getConnectionPlace()->getCity()->getCode() << "</countryCode></address>";

				os << "<projectedPoint>" << "\n";
				os << "<X>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(pt.getX())) << "</X>" << "\n";
				os << "<Y>" << ((ps->getX() <= 0 || ps->getY() <= 0) ? "0" : GetCoordinate(pt.getY())) << "</Y>" << "\n";
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
			BOOST_FOREACH(Registry<Line>::value_type line, _env->getRegistry<Line>())
			{
				const Edge* from(NULL);
				BOOST_FOREACH(const Edge* to, line.second->getEdges())
				{
					if (from != NULL)
					{
						os << "<PtLink>" << "\n";
						os << "<objectId>" << TridentId (peerid, "PtLink", *from) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopPoint", *from) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopPoint", *to) << "</endOfLink>" << "\n";
						os << "<linkDistance>" << Conversion::ToString(to->getMetricOffset() - from->getMetricOffset()) << "</linkDistance>" << "\n";   // in meters!
						os << "</PtLink>" << "\n";
					}
					from = to;
				}
			}		

			// --------------------------------------------------- JourneyPattern
			// One per route 
			BOOST_FOREACH(Registry<Line>::value_type itline, _env->getRegistry<Line>())
			{
				const Line* line(itline.second.get());
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
			BOOST_FOREACH(Registry<ScheduledService>::value_type itsrv, _env->getRegistry<ScheduledService>())
			{
				const ScheduledService* srv(itsrv.second.get());
				bool isDRT(
					dynamic_cast<const PTUseRule*>(&srv->getUseRule(USER_PEDESTRIAN)) != NULL &&
					static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN)).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
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
				
				LineStopTableSync::SearchResult linestops(
					LineStopTableSync::Search(*_env, srv->getPathId())
				);
				BOOST_FOREACH(shared_ptr<LineStop> ls, linestops)
				{
					os << "<vehicleJourneyAtStop>" << "\n";
					os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

					if (ls->getRankInPath() > 0 && ls->isArrival())
						os << "<arrivalTime>" << ToXsdTime (Service::GetTimeOfDay(srv->getArrivalBeginScheduleToIndex(false, ls->getRankInPath()))) 
						<< "</arrivalTime>" << "\n";

					os	<< "<departureTime>";
					if (ls->getRankInPath() != linestops.size () - 1 && ls->isDeparture())
					{
						os << ToXsdTime (Service::GetTimeOfDay(srv->getDepartureBeginScheduleToIndex(false, ls->getRankInPath())));
					}
					else
					{
						os << ToXsdTime (Service::GetTimeOfDay(srv->getArrivalBeginScheduleToIndex(false, ls->getRankInPath())));
					}
					os	<< "</departureTime>" << "\n";

					os << "</vehicleJourneyAtStop>" << "\n";
				}

				if (_withTisseoExtension)
				{
					const UseRule& hRule(srv->getUseRule(USER_HANDICAPPED));
					os <<
						"<mobilityRestrictedSuitability>" <<
						(hRule.getAccessCapacity() ? lexical_cast<string>(*hRule.getAccessCapacity()) : "999") <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
					if(	dynamic_cast<const PTUseRule*>(&hRule) != NULL &&
						static_cast<const PTUseRule&>(hRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(hRule).getKey()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}

					const UseRule& bRule(srv->getUseRule(USER_BIKE));
					os <<
						"<bikeSuitability>" <<
						(bRule.getAccessCapacity() ? Conversion::ToString(*bRule.getAccessCapacity()) : "999") <<
						"</bikeSuitability>" <<
						"\n"
					;
					if (dynamic_cast<const PTUseRule*>(&bRule) != NULL &&
						static_cast<const PTUseRule&>(bRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(bRule).getKey()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}
					if (isDRT)
					{
						const PTUseRule& pRule(static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN)));
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", pRule.getKey()) << "</reservationRule>" << "\n";
					}
				}
				os << "</VehicleJourney>" << "\n";
			}

			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env->getRegistry<ContinuousService>())
			{
				const ContinuousService* srv(itsrv.second.get());
				bool isDRT(
					dynamic_cast<const PTUseRule*>(&srv->getUseRule(USER_PEDESTRIAN)) &&
					static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN)).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
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
					LineStopTableSync::SearchResult linestops(
						LineStopTableSync::Search(*_env, srv->getPathId())
					);
					BOOST_FOREACH(shared_ptr<LineStop> ls, linestops)
					{
						os << "<vehicleJourneyAtStop>" << "\n";
						os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
						os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

						const time_duration& schedule((ls->getRankInPath() > 0 && ls->isArrival()) ? srv->getArrivalBeginScheduleToIndex(false, ls->getRankInPath()) : srv->getDepartureBeginScheduleToIndex(false, ls->getRankInPath()));
						os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(false, 0)) << "</elapseDuration>" << "\n";
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
						(hRule.getAccessCapacity() ? lexical_cast<string>(*hRule.getAccessCapacity()) : "999") <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
					if(	dynamic_cast<const PTUseRule*>(&hRule) != NULL &&
						static_cast<const PTUseRule&>(hRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(hRule).getKey()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}

					const UseRule& bRule(srv->getUseRule(USER_BIKE));
					os <<
						"<bikeSuitability>" <<
						(bRule.getAccessCapacity() ? Conversion::ToString(*bRule.getAccessCapacity()) : "999") <<
						"</bikeSuitability>" <<
						"\n"
					;
					if (dynamic_cast<const PTUseRule*>(&bRule) != NULL &&
						static_cast<const PTUseRule&>(bRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(bRule).getKey()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}
					if (isDRT)
					{
						const PTUseRule& pRule(static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN)));
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", pRule.getKey()) << "</reservationRule>" << "\n";
					}
				}
				os << "</VehicleJourney>" << "\n";
			}


			os << "</ChouetteLineDescription>" << "\n";
			}


			if (_withTisseoExtension)
			{
				// Reservation Rules -----------------------------------------------------------------------

 				BOOST_FOREACH(Registry<PTUseRule>::value_type r, _env->getRegistry<PTUseRule>())
 				{
 					const PTUseRule& rule(*r.second);
 
					if (rule.getReservationType() == PTUseRule::RESERVATION_RULE_FORBIDDEN || (rule.getMinDelayDays().days() == 0 && rule.getMinDelayMinutes().total_seconds() == 0))	continue;
 
 					os << "<ReservationRule>" << "\n";
 					os << "<objectId>" << TridentId (peerid, "ReservationRule", rule.getKey ()) << "</objectId>" << "\n";
					os << "<ReservationCompulsory>" << ((rule.getReservationType() == PTUseRule::RESERVATION_RULE_COMPULSORY) ? "compulsory" : "optional") << "</ReservationCompulsory>" << "\n";
 					os << "<deadLineIsTheCustomerDeparture>" << !rule.getOriginIsReference() << "</deadLineIsTheCustomerDeparture>" << "\n";
 					if (rule.getMinDelayMinutes().total_seconds() > 0)
 					{
						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << "\n";
					}
 					if (rule.getMinDelayDays().days() > 0)
 					{
						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << "\n";
 					}
 					if (!rule.getHourDeadLine().is_not_a_date_time())
 					{
 						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << "\n";
 					}
 					if (rule.getMaxDelayDays())
 					{
 						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(*rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << "\n";
 					}
// 					if (!rule.getPhoneExchangeNumber().empty())
 //					{
 //						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << "\n";
 //						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << "\n";
 //					}
// 					if (!rule.getWebSiteUrl().empty())
// 					{
// 						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << "\n";
// 					}
 					os << "</ReservationRule>" << "\n";
 				}

				// Non concurrency -----------------------------------------------------------------------
				BOOST_FOREACH(Registry<NonConcurrencyRule>::value_type itrule, _env->getRegistry<NonConcurrencyRule>())
				{
					const NonConcurrencyRule& rule(*itrule.second);
					os << "<LineConflict>" << "\n";
					os << "<objectId>" << TridentId (peerid, "LineConflict", rule) << "</objectId>" << "\n";
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule.getHiddenLine()->getKey()) << "</forbiddenLine>" << "\n";
					os << "<usedLine>" << TridentId (peerid, "Line", rule.getPriorityLine()->getKey()) << "</usedLine>" << "\n";
					os << "<conflictDelay>" << ToXsdDuration(rule.getDelay()) << "</conflictDelay>" << "\n";
					os << "</LineConflict>" << "\n";
				}


				// CityMainStops --------------------------------------------------- 
				BOOST_FOREACH(Registry<City>::value_type itcity, _env->getRegistry<City>())
				{
					const City* city(itcity.second.get());
					vector<string> containedStopAreas;

					// Contained connection places
					Env senv;
					ConnectionPlaceTableSync::SearchResult places(
						ConnectionPlaceTableSync::Search(senv, city->getKey(), true)
					);
					BOOST_FOREACH(shared_ptr<const PublicTransportStopZoneConnectionPlace> cp, places)
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
			const path& filePath,
			ostream& os,
			string fileKey
		){
			ifstream ifs(filePath.file_string().c_str());
			if (!ifs)
			{
				throw Exception("Could no open the file" + filePath.file_string());
			}

			bool failure(false);

			// Read the whole file into a string
			stringstream text;
			text << ifs.rdbuf();
			ifs.close();

			XMLNode allNode = XMLNode::parseString (text.str().c_str(), "ChouettePTNetwork");
			if(allNode.isEmpty())
			{
				os << "ERR  : XML parsing of file " << filePath.file_string() << " has failed.<br />";
				return;
			}
			
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
			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(*_env, string(), key)
			);
			if(!networks.empty())
			{
				if(networks.size() > 1)
				{
					os << "WARN : more than one network with key " << key << "<br />";
				}
				network = TransportNetworkTableSync::GetEditable(
					networks.front()->getKey(),
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
			
			string ckey(lineKeyNode.getText());
			
			shared_ptr<CommercialLine> cline;
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(*_env, network->getKey(), string("%"), ckey)
			);
			if(!lines.empty())
			{
				if(lines.size() > 1)
				{
					os << "WARN : more than one commercial line with key " << ckey << "<br />";
				}
				cline = CommercialLineTableSync::GetEditable(
					lines.front()->getKey(),
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
				if(!clineShortNameNode.isEmpty())
				{
					cline->setShortName(clineShortNameNode.getText());
				}
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

			// Places
			map<string, XMLNode> areaCentroids;
			if(_importStops)
			{
				XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
				int stopsNumber(chouetteAreaNode.nChildNode("AreaCentroid"));
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode stopAreaNode(chouetteAreaNode.getChildNode("AreaCentroid", stopRank));

					areaCentroids.insert(
						make_pair(
							stopAreaNode.getChildNode("objectId", 0).getText(),
							stopAreaNode
					)	);
				}
			}


			// Commercial stops
			map<string, PublicTransportStopZoneConnectionPlace*> commercialStopsByPhysicalStop;
			if(_importStops)
			{
				XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
				int stopsNumber(chouetteAreaNode.nChildNode("StopArea"));
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode stopAreaNode(chouetteAreaNode.getChildNode("StopArea", stopRank));
					XMLNode extensionNode(stopAreaNode.getChildNode("StopAreaExtension", 0));
					XMLNode areaTypeNode(extensionNode.getChildNode("areaType",0));
					if(string(areaTypeNode.getText()) != string("CommercialStopPoint")) continue;

					XMLNode keyNode(stopAreaNode.getChildNode("objectId", 0));
					XMLNode nameNode(stopAreaNode.getChildNode("name", 0));
					string stopKey(keyNode.getText());
					XMLNode areaNode(stopAreaNode.getChildNode("centroidOfArea", 0));
					
					// Place
					map<string,XMLNode>::iterator itPlace(areaCentroids.find(areaNode.getText()));
					if(itPlace == areaCentroids.end())
					{
						os << "ERR  : area centroid " << areaNode.getText() << " not found in CommercialStopPoint " << stopKey << " (" << nameNode.getText() << ")<br />";
						failure = true;
						continue;
					}
					XMLNode& areaCentroid(itPlace->second);
					XMLNode addressNode(areaCentroid.getChildNode("address", 0));
					CityTableSync::SearchResult cityResult(
						CityTableSync::Search(*_env, optional<string>(), optional<string>(), string(addressNode.getChildNode("countryCode", 0).getText()))
					);
					if(cityResult.empty())
					{
						os << "ERR  : commercial stop point " << stopKey << " with area centroid " << areaNode.getText() << " does not link to a valid city (" << addressNode.getChildNode("countryCode", 0).getText() << ")<br />";
						failure = true;
						continue;
					}
					shared_ptr<const City> city(cityResult.front());


					ConnectionPlaceTableSync::SearchResult cstops(
						ConnectionPlaceTableSync::Search(
							*_env,
							optional<RegistryKeyType>(),
							logic::indeterminate,
							stopKey,
							false
					)	);
					if(cstops.size() > 1)
					{
						os << "WARN : more than one stop with key" << stopKey << "<br />";
					}

					shared_ptr<PublicTransportStopZoneConnectionPlace> curStop;
					if(cstops.empty())
					{
						// Commercial stop point creation with some default values
						curStop.reset(new PublicTransportStopZoneConnectionPlace);
						curStop->setCodeBySource(stopKey);
						curStop->setAllowedConnection(true);
						curStop->setDefaultTransferDelay(minutes(8));
						curStop->setKey(ConnectionPlaceTableSync::getId());
						_env->getEditableRegistry<PublicTransportStopZoneConnectionPlace>().add(curStop);

						os << "CREA : Creation of the commercial stop with key " << stopKey << " (" << nameNode.getText() <<  ")<br />";
					}
					else
					{
						// Load existing commercial stop point
						RegistryKeyType stopId(cstops.front()->getKey());
						curStop = ConnectionPlaceTableSync::GetEditable(stopId, *_env, UP_LINKS_LOAD_LEVEL);
				
						os << "LOAD : link between stops " << stopKey << " (" << nameNode.getText() << ") and "
							<< curStop->getKey() << " (" << curStop->getFullName() << ")<br />";
					}
					curStop->setName(nameNode.getText());
					curStop->setCity(city.get());

					// Link from physical stops
					int pstopsNumber(stopAreaNode.nChildNode("contains"));
					for(int pstopRank(0); pstopRank < pstopsNumber; ++pstopRank)
					{
						commercialStopsByPhysicalStop[stopAreaNode.getChildNode("contains", pstopRank).getText()] = curStop.get();
					}
				}
			}

			// Stops
			map<string, PhysicalStop*> stops;
			XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
			int stopsNumber(chouetteAreaNode.nChildNode("StopArea"));
			for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
			{
				XMLNode stopAreaNode(chouetteAreaNode.getChildNode("StopArea", stopRank));
				XMLNode extensionNode(stopAreaNode.getChildNode("StopAreaExtension", 0));
				XMLNode areaTypeNode(extensionNode.getChildNode("areaType",0));
				if(	string(areaTypeNode.getText()) != string("BoardingPosition") &&
					string(areaTypeNode.getText()) != string("Quay") 
				){
					continue;
				}
				XMLNode areaCentroidNode(stopAreaNode.getChildNode("centroidOfArea", 0));

				XMLNode keyNode(stopAreaNode.getChildNode("objectId", 0));
				XMLNode nameNode(stopAreaNode.getChildNode("name", 0));
				string stopKey(keyNode.getText());
			
				PhysicalStopTableSync::SearchResult pstops(
					PhysicalStopTableSync::Search(
						*_env,
						optional<RegistryKeyType>(),
						stopKey
				)	);
				if(pstops.empty() && !_importStops)
				{
					os << "ERR  : stop not found " << stopKey << " (" << nameNode.getText() << ")<br />";
					failure = true;
					continue;
				}
				
				if(pstops.size() > 1)
				{
					os << "WARN : more than one stop with key" << stopKey << "<br />";
				}
				
				shared_ptr<PhysicalStop> curStop;
				if(pstops.empty())
				{
					// Stop creation
					map<string,PublicTransportStopZoneConnectionPlace*>::const_iterator itcstop(commercialStopsByPhysicalStop.find(stopKey));
					if(itcstop == commercialStopsByPhysicalStop.end())
					{
						os << "ERR  : stop " << stopKey << " not found in any commercia stop (" << nameNode.getText() << ")<br />";
						failure = true;
						continue;
					}
					curStop.reset(new PhysicalStop);
					curStop->setHub(itcstop->second);
					curStop->setCodeBySource(stopKey);
					curStop->setKey(PhysicalStopTableSync::getId());
					_env->getEditableRegistry<PhysicalStop>().add(curStop);

					os << "CREA : Creation of the physical stop with key " << stopKey << " (" << nameNode.getText() <<  ")<br />";
				}
				else
				{
					RegistryKeyType stopId(pstops.front()->getKey());
					curStop = PhysicalStopTableSync::GetEditable(stopId, *_env, UP_LINKS_LOAD_LEVEL);

					os << "LOAD : link between stops " << stopKey << " (" << nameNode.getText() << ") and "
						<< curStop->getKey() << " (" << curStop->getConnectionPlace()->getName() << ")<br />";
				}

				if(_importStops)
				{
					curStop->setName(nameNode.getText());
					map<string, XMLNode>::iterator itPlace(areaCentroids.find(areaCentroidNode.getText()));
					if(itPlace == areaCentroids.end())
					{
						os << "WARN : Physical stop with key " << stopKey << " links to a not found area centroid " << areaCentroidNode.getText() << " (" << nameNode.getText() <<  ")<br />";
					}
					else
					{
						XMLNode& areaCentroid(itPlace->second);
						XMLNode projectedPointNode(areaCentroid.getChildNode("projectedPoint", 0));
						curStop->setXY(
							lexical_cast<double>(projectedPointNode.getChildNode("X", 0).getText()),
							lexical_cast<double>(projectedPointNode.getChildNode("Y", 0).getText())
						);
					}
				}
	
				stops[stopKey] = curStop.get();
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
				map<string, PhysicalStop*>::iterator it(stops.find(containedNode.getText()));
				if(it == stops.end())
				{
					os << "ERR  : stop " << containedNode.getText() << " not found by stop point " << spKeyNode.getText() << ")<br />";
					failure = true;
					continue;
				}
				stopPoints[spKeyNode.getText()] = stops[containedNode.getText()];
			}

			if(failure)
			{
				os << "<b>FAILURE : At least a stop point is missing : load interrupted</b><br />";
				return;
			}

			// Load of existing routes
			LineTableSync::SearchResult sroutes(
				LineTableSync::Search(*_env, cline->getKey(), _dataSource->getKey())
			);
			BOOST_FOREACH(shared_ptr<Line> line, sroutes)
			{
				LineStopTableSync::Search(
					*_env,
					line->getKey(),
					UNKNOWN_VALUE,
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
				ScheduledServiceTableSync::Search(
					*_env,
					line->getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					optional<date>(),
					false,
					0, optional<size_t>(), true, true,
					UP_LINKS_LOAD_LEVEL
				);
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
				BOOST_FOREACH(Registry<Line>::value_type line, _env->getRegistry<Line>())
				{
					if(*line.second == routeStops)
					{
						route = line.second;
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
					route->setCodeBySource(routeNames[routeIdNode.getText()]);
					route->setWayBack(routeWaybacks[routeIdNode.getText()]);
					route->setDataSource(_dataSource);
					route->setKey(LineTableSync::getId());
					_env->getEditableRegistry<Line>().add(route);
					
					int rank(0);
					BOOST_FOREACH(PhysicalStop* stop, routeStops)
					{
						shared_ptr<LineStop> ls(new LineStop);
						ls->setLine(route.get());
						ls->setPhysicalStop(stop);
						ls->setRankInPath(rank);
						ls->setIsArrival(rank > 0);
						ls->setIsDeparture(rank < routeStops.size() - 1);
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
				service->setPathId(line->getKey());
				service->setServiceNumber(numberNode.getText());
				ScheduledService::Schedules deps;
				ScheduledService::Schedules arrs;
				int stopsNumber(serviceNode.nChildNode("VehicleJourneyAtStop"));
				time_duration lastDep(0,0,0);
				time_duration lastArr(0,0,0);
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode vjsNode(serviceNode.getChildNode("VehicleJourneyAtStop", stopRank));
					XMLNode depNode(vjsNode.getChildNode("departureTime"));
					XMLNode arrNode(vjsNode.getChildNode("arrivalTime"));
					time_duration depHour(duration_from_string(depNode.getText()));
					time_duration arrHour(duration_from_string(arrNode.getText()));
					time_duration depSchedule(depHour + hours(24 * (lastDep.hours() / 24 + (depHour < Service::GetTimeOfDay(lastDep) ? 1 : 0))));
					time_duration arrSchedule(arrHour + hours(24 * (lastArr.hours() / 24 + (arrHour < Service::GetTimeOfDay(lastArr) ? 1 : 0))));
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
					
					os << "CREA : Creation of service " << service->getServiceNumber() << " for " << keyNode.getText() << " (" << deps[0] << ") on route " << line->getKey() << " (" << line->getName() << ")<br />";
				}
				else
				{
					services[keyNode.getText()] = existingService;
					
					os << "LOAD : Use of service " << existingService->getKey() << " (" << existingService->getServiceNumber() << ") for " << keyNode.getText() << " (" << deps[0] << ") on route " << line->getKey() << " (" << line->getName() << ")<br />";

				}
			}
			
			// Calendars
			int calendarNumber(allNode.nChildNode("Timetable"));
			date today(day_clock::local_day());
			for(int calendarRank(0); calendarRank < calendarNumber; ++calendarRank)
			{
				XMLNode calendarNode(allNode.getChildNode("Timetable", calendarRank));
				
				int daysNumber(calendarNode.nChildNode("calendarDay"));
				int servicesNumber(calendarNode.nChildNode("vehicleJourneyId"));
				for(int dayRank(0); dayRank < daysNumber; ++dayRank)
				{
					XMLNode dayNode(calendarNode.getChildNode("calendarDay", dayRank));
					date d(from_string(dayNode.getText()));
					if(!_withOldDates && d < today)
					{
						continue;
					}
					
					for(int serviceRank(0); serviceRank < servicesNumber; ++serviceRank)
					{
						XMLNode serviceNode(calendarNode.getChildNode("vehicleJourneyId", serviceRank));
						shared_ptr<ServiceDate> sd(new ServiceDate);
						sd->setService(services[serviceNode.getText()]);
						sd->setDate(d);
						sd->setKey(ServiceDateTableSync::getId());
						_serviceDates.push_back(sd);
					}
				}
			}
			
			os << "<b>SUCCESS : Data loaded</b><br />";
		}
		
		void TridentFileFormat::save(std::ostream& os) const
		{
			SQLiteTransaction transaction;

			// Saving of each created or altered objects
			if(_importStops)
			{
				BOOST_FOREACH(Registry<PublicTransportStopZoneConnectionPlace>::value_type cstop, _env->getRegistry<PublicTransportStopZoneConnectionPlace>())
				{
					ConnectionPlaceTableSync::Save(cstop.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<PhysicalStop>::value_type stop, _env->getRegistry<PhysicalStop>())
				{
					PhysicalStopTableSync::Save(stop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env->getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env->getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<Line>::value_type line, _env->getRegistry<Line>())
			{
				LineTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env->getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<ScheduledService>::value_type service, _env->getRegistry<ScheduledService>())
			{
				if(_withOldDates)
				{
					ServiceDateTableSync::DeleteDates(service.second->getKey(), transaction);
				}
				else
				{
					ServiceDateTableSync::DeleteDatesFromNow(service.second->getKey(), transaction);
				}
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(shared_ptr<ServiceDate> date, _serviceDates)
			{
				ServiceDateTableSync::Save(date.get(), transaction);
			}
			
			//TODO cleaning : delete services without dates and routes without service
			transaction.run();					
	
			os << "<b>SUCCESS : Data saved.</b><br />";
		}



		TridentFileFormat::~TridentFileFormat()
		{

		}

		std::string TridentFileFormat::GetCoordinate( const double value )
		{
			return
				(value > 0) ?
				lexical_cast<string>(value) :
				string();
		}



		server::ParametersMap TridentFileFormat::_getParametersMap(bool import) const
		{
			ParametersMap result;
			if(import)
			{
				result.insert(PARAMETER_IMPORT_STOPS, _importStops);
			}
			result.insert(PARAMETER_WITH_OLD_DATES, _withOldDates);
			return result;
		}



		void TridentFileFormat::_setFromParametersMap(const ParametersMap& map, bool import)
		{
			if(import)
			{
				_importStops = map.getDefault<bool>(PARAMETER_IMPORT_STOPS, false);
			}
			_withOldDates = map.getDefault<bool>(PARAMETER_WITH_OLD_DATES, false);
		}



		void TridentFileFormat::setImportStops( bool value )
		{
			_importStops = value;
		}



		bool TridentFileFormat::getImportStops() const
		{
			return _importStops;
		}



		void TridentFileFormat::setWithOldDates( bool value )
		{
			_withOldDates = value;
		}



		bool TridentFileFormat::getWithOldDates() const
		{
			return _withOldDates;
		}
	}
}


/** EnvModule class implementation.
    @file EnvModule.cpp

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

#include "EnvModule.h"
#include "Journey.h"
#include "01_util/Constants.h"
#include "Conversion.h"
#include "UId.h"
#include "T9Filter.h"
#include "SentAlarm.h"
#include "17_messages/Types.h"
#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "Crossing.h"
#include "Line.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "PublicPlace.h"
#include "PublicPlaceTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "PlaceAlias.h"
#include "PlaceAliasTableSync.h"
#include "Address.h"
#include "AddressTableSync.h"
#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"
#include "12_security/Constants.h"
#include "Right.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace lexmatcher;
	using namespace security;
	using namespace util;
	using namespace graph;
	using namespace time;
	using namespace road;
	using namespace pt;
	using namespace geography;
	

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,env::EnvModule>::FACTORY_KEY("35_env");
	}

    namespace env
    {
		void EnvModule::initialize()
		{
		}

	

		std::vector<pair<uid, std::string> > EnvModule::getCommercialLineLabels(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, bool withAll
		){
			vector<pair<uid,string> > m;
			if (withAll)
			m.push_back(make_pair(UNKNOWN_VALUE, "(toutes)"));

			Env env;
			CommercialLineTableSync::Search(env, rights, totalControl, neededLevel);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
				m.push_back(make_pair(line->getKey(), line->getShortName()));
			return m;
		}



		shared_ptr<NonPermanentService> EnvModule::FetchEditableService(
			const RegistryKeyType& id,
			Env& env
		){
			int tableId(decodeTableId(id));
			if(tableId == ScheduledServiceTableSync::TABLE.ID)
				return static_pointer_cast<NonPermanentService, ScheduledService>(env.getEditableRegistry<ScheduledService>().getEditable(id));
			if (tableId == ContinuousServiceTableSync::TABLE.ID)
				return static_pointer_cast<NonPermanentService, ContinuousService>(env.getEditableRegistry<ContinuousService>().getEditable(id));
			
			return shared_ptr<NonPermanentService>();
		}



		void EnvModule::getNetworkLinePlaceRightParameterList(ParameterLabelsVector& m)
		{
			Env env;
			TransportNetworkTableSync::Search(env);
			CommercialLineTableSync::Search(env);

			m.push_back(make_pair(string(), "--- Réseaux ---"));
			BOOST_FOREACH(shared_ptr<TransportNetwork> network, env.getRegistry<TransportNetwork>())
				m.push_back(make_pair(Conversion::ToString(network->getKey()), network->getName() ));

			m.push_back(make_pair(string(), "--- Lignes ---"));
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
				m.push_back(make_pair(Conversion::ToString(line->getKey()), line->getName() ));

		}

		std::string EnvModule::getName() const
		{
			return  "Réseaux de transport public";
		}

		
		
		int EnvModule::GetMaxAlarmLevel(
			const Journey& journey
		){
			DateTime alarmStart(TIME_UNKNOWN);
			DateTime alarmStop(TIME_UNKNOWN);
			DateTime now(TIME_CURRENT);
			int maxAlarmLevel(0);
		 
			BOOST_FOREACH(const ServiceUse& leg, journey.getServiceUses())
			{
				const Service* service(leg.getService());
//				bool legIsConnection = (it < _journeyLegs.end() - 2);

					// -- Alarm on origin --
					// Alarm start = first departure
					// Alarm stop = last departure
					alarmStart = leg.getDepartureDateTime();
					alarmStop = alarmStart;
				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();
				
	/*				if ( leg->getOrigin ()->getFromVertex ()->getConnectionPlace ()
					 ->hasApplicableAlarm (alarmStart, alarmStop)
					 && maxAlarmLevel < leg->getOrigin()->getFromVertex ()->
					 getConnectionPlace ()->getAlarm ()->getLevel () )
						maxAlarmLevel = leg->getOrigin()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
	*/			
				

					// -- Service alarm --
					// Alarm start = first departure
					// Alarm stop = last arrival
					alarmStart = leg.getDepartureDateTime();
					alarmStop = leg.getArrivalDateTime();
				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();

	/*				if ( (leg->getService ()->getPath ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
					 (maxAlarmLevel < leg->getService ()->getPath ()->getAlarm ()->getLevel ()) )
				{
						maxAlarmLevel = leg->getService ()->getPath ()->getAlarm ()->getLevel ();
				}
	*/			
					// -- Alarm on arrival --
					// Alarm start = first arrival
					// Alarm stop = last arrival if connection, last arrival otherwise
					alarmStart = leg.getArrivalDateTime();
					alarmStop = alarmStart;
//					if (legIsConnection)
//				{
//						alarmStop = (it+1)->getDepartureDateTime ();
//				}

				if (service->isContinuous ()) 
					alarmStop += static_cast<const ContinuousService*>(service)->getRange ();

	/*				if ( (leg->getDestination ()->getFromVertex ()->getConnectionPlace ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
						 (maxAlarmLevel < leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ()) )
				{
					maxAlarmLevel = leg->getDestination()->getFromVertex ()->getConnectionPlace ()->getAlarm ()->getLevel ();
				}
*/			}
			return maxAlarmLevel;
		}

    }
}

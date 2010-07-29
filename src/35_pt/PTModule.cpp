
/** PTModule class implementation.
    @file PTModule.cpp

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

#include "ServerModule.h"
#include "PTModule.h"
#include "ScheduledService.h"
#include "Env.h"
#include "ContinuousService.h"
#include "Journey.h"
#include "01_util/Constants.h"
#include "T9Filter.h"
#include "SentAlarm.h"
#include "17_messages/Types.h"
#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "Crossing.h"
#include "JourneyPattern.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
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
#include "StopPoint.hpp"
#include "PhysicalStopTableSync.h"
#include "12_security/Constants.h"
#include "Right.h"
#include "PTUseRuleTableSync.h"
#include "RollingStockTableSync.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace graph;
	

	namespace graph
	{
		template<> const GraphIdType GraphModuleTemplate<PTModule>::GRAPH_ID(2);
	}

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,PTModule>::FACTORY_KEY("35_pt");
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<PTModule>::NAME("Transport public");
		
		template<> void ModuleClassTemplate<PTModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<PTModule>::Init()
		{
			// Data cleaner
			shared_ptr<thread> theThread(
				new thread(
					&PTModule::RTDataCleaner
			)	);
			ServerModule::AddThread(theThread, "Real time data cleaner");

			// Creation of each transport mode corresponding to Trident values except "Other" which is used for null pointer 
			Env env;
			vector<string> tridentKeys;
			tridentKeys.push_back("Air");
			tridentKeys.push_back("Train");
			tridentKeys.push_back("LongDistanceTrain");
			tridentKeys.push_back("LocalTrain");
			tridentKeys.push_back("RapidTransit");
			tridentKeys.push_back("Metro");
			tridentKeys.push_back("Tramway");
			tridentKeys.push_back("Coach");
			tridentKeys.push_back("Bus");
			tridentKeys.push_back("Ferry");
			tridentKeys.push_back("Waterborne");
			tridentKeys.push_back("PrivateVehicle");
			tridentKeys.push_back("Walk");
			tridentKeys.push_back("Trolleybus");
			tridentKeys.push_back("Bicycle");
			tridentKeys.push_back("Shuttle");
			tridentKeys.push_back("Taxi");
			tridentKeys.push_back("VAL");

			BOOST_FOREACH(const string& tridentKey, tridentKeys)
			{
				RollingStockTableSync::SearchResult rollingStocks(RollingStockTableSync::Search(env, tridentKey, true));
				if(rollingStocks.empty())
				{
					RollingStock s;
					s.setName(tridentKey);
					s.setTridentKey(tridentKey);
					s.setIsTridentKeyReference(true);
					RollingStockTableSync::Save(&s);
				}
			}

		}
		
		template<> void ModuleClassTemplate<PTModule>::End()
		{
		}
	}

	namespace pt
	{
		void PTModule::RTDataCleaner()
		{
			Registry<ScheduledService>& registry(
				Env::GetOfficialEnv().getEditableRegistry<ScheduledService>()
			);

			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				posix_time::ptime now(posix_time::second_clock::local_time());

				BOOST_FOREACH(Registry<ScheduledService>::value_type& service, registry)
				{
					if(now > service.second->getNextRTUpdate())
					{
						service.second->clearRTData();
					}
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(posix_time::minutes(1));
			}
		}



		PTModule::Labels PTModule::getCommercialLineLabels(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel
			, bool withAll
		){
			Labels m;
			if (withAll)
			m.push_back(make_pair(optional<RegistryKeyType>(), "(toutes)"));

			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(Env::GetOfficialEnv(), rights, totalControl, neededLevel)
			);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
				m.push_back(make_pair(line->getKey(), line->getShortName()));
			return m;
		}



		void PTModule::getNetworkLinePlaceRightParameterList(ParameterLabelsVector& m)
		{
			Env env;
			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(env)
			);
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(env)
			);

			m.push_back(make_pair(string(), "--- Réseaux ---"));
			BOOST_FOREACH(shared_ptr<TransportNetwork> network, networks)
				m.push_back(make_pair(lexical_cast<string>(network->getKey()), network->getName() ));

			m.push_back(make_pair(string(), "--- Lignes ---"));
			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
				m.push_back(make_pair(lexical_cast<string>(line->getKey()), line->getName() ));
		}
		
		
		
		int PTModule::GetMaxAlarmLevel(
			const Journey& journey
		){
			ptime alarmStart(not_a_date_time);
			ptime alarmStop(not_a_date_time);
			ptime now(second_clock::local_time());
			int maxAlarmLevel(0);
		 
			BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
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



		PTModule::Labels PTModule::GetPTUseRuleLabels()
		{
			Labels result;
			result.push_back(make_pair(RegistryKeyType(0), "(non défini)"));

			PTUseRuleTableSync::SearchResult rules(PTUseRuleTableSync::Search(Env::GetOfficialEnv()));
			BOOST_FOREACH(const PTUseRuleTableSync::SearchResult::value_type& element, rules)
			{
				result.push_back(make_pair(element->getKey(), element->getName()));
			}

			return result;
		}

	}
}
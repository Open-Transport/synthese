
/** DeparturesTableModule class implementation.
	@file DeparturesTableModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "StopArea.hpp"
#include "CommercialLine.h"

#include "DeparturesTableModule.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DisplayScreen.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace security;
	using namespace util;
	using namespace departure_boards;
	using namespace server;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, departure_boards::DeparturesTableModule>::FACTORY_KEY("54_departures_table");
	}

	namespace departure_boards
	{
		const string DeparturesTableModule::PARAMETER_INEO_SERVER_IP = "ineo_server_ip";
		const string DeparturesTableModule::PARAMETER_INEO_SERVER_PORT = "ineo_server_port";
		const string DeparturesTableModule::PARAMETER_INEO_SERVER_DB_LOGIN = "ineo_server_db_login";
		const string DeparturesTableModule::PARAMETER_INEO_SERVER_DB_PASSWORD = "ineo_server_db_password";
		const string DeparturesTableModule::PARAMETER_INEO_SERVER_DB_NAME = "ineo_server_db_name";

		string DeparturesTableModule::_ineoServerIP;
		string DeparturesTableModule::_ineoServerDBLogin;
		string DeparturesTableModule::_ineoServerDBPassword;
		string DeparturesTableModule::_ineoServerDBName;
		int DeparturesTableModule::_ineoServerPort = 0;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<DeparturesTableModule>::NAME("SAI Tableaux de départs");

		template<> void ModuleClassTemplate<DeparturesTableModule>::PreInit()
		{
			RegisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_IP, "", &DeparturesTableModule::ParameterCallback);
			RegisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_PORT, "", &DeparturesTableModule::ParameterCallback);
			RegisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_LOGIN, "", &DeparturesTableModule::ParameterCallback);
			RegisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_PASSWORD, "", &DeparturesTableModule::ParameterCallback);
			RegisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_NAME, "", &DeparturesTableModule::ParameterCallback);
		}

		template<> void ModuleClassTemplate<DeparturesTableModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<DeparturesTableModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<DeparturesTableModule>::End()
		{
			UnregisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_IP);
			UnregisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_PORT);
			UnregisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_LOGIN);
			UnregisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_PASSWORD);
			UnregisterParameter(DeparturesTableModule::PARAMETER_INEO_SERVER_DB_NAME);
		}



		template<> void ModuleClassTemplate<DeparturesTableModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<DeparturesTableModule>::CloseThread(
			
			){
		}
	}

	namespace departure_boards
	{
		DeparturesTableModule::Labels DeparturesTableModule::getDisplayTypeLabels(
			bool withAll, /*= false*/
			bool withNone
		){
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			}
			if (withNone)
			{
				m.push_back(make_pair(0, "(non défini)"));
			}
			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(Env::GetOfficialEnv())
			);
			BOOST_FOREACH(const boost::shared_ptr<DisplayType>& displayType, types)
			{
				m.push_back(make_pair(displayType->getKey(), displayType->getName()));
			}
			return m;
		}

		DeparturesTableModule::Labels DeparturesTableModule::getPlacesWithBroadcastPointsLabels(
			const security::RightsOfSameClassMap& rights
			, bool totalControl
			, RightLevel neededLevel
			, bool withAll /*= false*/
		){
			Labels localizations;
			if (withAll)
				localizations.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));
			std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > bpv = searchConnectionPlacesWithBroadcastPoints(
				Env::GetOfficialEnv(),
				rights,
				totalControl,
				neededLevel,
				string(),
				string(),
				AT_LEAST_ONE_BROADCASTPOINT
			);
			BOOST_FOREACH(const boost::shared_ptr<ConnectionPlaceWithBroadcastPoint>& con, bpv)
			{
				localizations.push_back(make_pair(con->place->getKey(), con->cityName + " " + con->place->getName()));
			}
			return localizations;
		}



		DeparturesTableModule::Labels DeparturesTableModule::getCommercialLineWithBroadcastLabels( bool withAll /*= false*/ )
		{
			Env env;
			Labels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), "(toutes)"));
			}
			vector<boost::shared_ptr<const CommercialLine> > c = getCommercialLineWithBroadcastPoints(env);
			std::sort(c.begin(), c.end(), CommercialLine::PointerComparator());

			BOOST_FOREACH(const boost::shared_ptr<const CommercialLine>& line, c)
			{
				m.push_back(make_pair(line->getKey(), line->getShortName()));
			}
			return m;
		}



		PlaceWithDisplayBoards* DeparturesTableModule::GetPlaceWithDisplayBoards(
			const geography::NamedPlace* place,
			Env& env
		){
			Registry<PlaceWithDisplayBoards>& registry(
				env.getEditableRegistry<PlaceWithDisplayBoards>()
			);
			if(place)
			{
				if(!registry.contains(place->getKey()))
				{
					boost::shared_ptr<PlaceWithDisplayBoards> result(
						new PlaceWithDisplayBoards(place)
					);
					registry.add(
						result
					);
					return result.get();
				}
				else
				{
					return registry.getEditable(
						place->getKey()
					).get();
				}
			}
			else
			{
				return NULL;
			}
		}



		void DeparturesTableModule::RemovePlaceWithDisplayBoards(
			const geography::NamedPlace* place,
			util::Env& env
		){
			Registry<PlaceWithDisplayBoards>& registry(
				env.getEditableRegistry<PlaceWithDisplayBoards>()
			);
			registry.remove(place->getKey());
		}

		void DeparturesTableModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if(name == PARAMETER_INEO_SERVER_IP)
			{
				_ineoServerIP = value;
			}
			else if(name == PARAMETER_INEO_SERVER_PORT)
			{
				int port(3306);

				try
				{
					port = lexical_cast<int>(value);
				}
				catch(bad_lexical_cast&)
				{
				}

				_ineoServerPort = port;
			}
			else if(name == PARAMETER_INEO_SERVER_DB_LOGIN)
			{
				_ineoServerDBLogin = value;
			}
			else if(name == PARAMETER_INEO_SERVER_DB_PASSWORD)
			{
				_ineoServerDBPassword = value;
			}
			else if(name == PARAMETER_INEO_SERVER_DB_NAME)
			{
				_ineoServerDBName = value;
			}
		}
}	}

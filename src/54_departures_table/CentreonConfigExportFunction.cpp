
/** CentreonConfigExportFunction class implementation.
	@file DisplayScreenContentRequest.cpp

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

#include <sstream>

#include "CentreonConfigExportFunction.h"

#include "RequestException.h"
#include "Request.h"

#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"

#include "PublicTransportStopZoneConnectionPlace.h"

#include "Interface.h"

#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace time;
	using namespace env;
	using namespace interfaces;
	using namespace db;

	template<> const string util::FactorableTemplate<Function,departurestable::CentreonConfigExportFunction>::FACTORY_KEY("CentreonConfigExportFunction");

	namespace departurestable
	{

		ParametersMap CentreonConfigExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void CentreonConfigExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
		}

		void CentreonConfigExportFunction::_run( std::ostream& stream ) const
		{
			// Deletion of existing objects
			stream <<
				"BEGIN TRANSACTION;" <<
				"DELETE FROM host;" <<
				"DELETE FROM hostgroup;" <<
				"DELETE FROM hostgroup_relation;"
			;

			// Insertion of the SYNTHESE command
			stream <<
				"REPLACE INTO command(command_id,command_name,command_line,command_type) VALUES(" <<
				"5000,\"check_synthese\",\"$USER1$#S#check_synthese.sh " << _request->getClientURL() << " $HOSTADDRESS$\",2);";

			// Insertion of the hostgroups
			Env env;
			DisplayTypeTableSync::Search(env);
			BOOST_FOREACH(shared_ptr<const DisplayType> type, env.getRegistry<DisplayType>())
			{
				stream <<
					"INSERT INTO hostgroup(hg_id,hg_name,hg_alias,hg_activate) VALUES(" <<
					decodeObjectId(type->getKey()) << "," <<
					Conversion::ToSQLiteString(type->getName()) << "," <<
					Conversion::ToSQLiteString(type->getName()) << "," <<
					"1" <<
					");";
			}
			// Insertion of the hosts
			DisplayScreenTableSync::Search(env);
			BOOST_FOREACH(shared_ptr<const DisplayScreen> screen, env.getRegistry<DisplayScreen>())
			{
				if(!screen->isMonitored()) continue;

				stream <<
					"INSERT INTO host(host_id,command_command_id,timeperiod_tp_id,timeperiod_tp_id2," <<
					"host_name,host_alias,host_address,host_max_check_attempts,host_check_interval," <<
					"host_active_checks_enabled,host_notification_interval,host_notification_enabled,host_activate) VALUES(" <<
					decodeObjectId(screen->getKey()) << ",2000,1,1," <<
					Conversion::ToSQLiteString(screen->getFullName()) << "," <<
					Conversion::ToSQLiteString(screen->getFullName()) << "," <<
					screen->getKey() << "," <<
					"1000,1,1,720,1,1" <<
					");" <<

					"INSERT INTO hostgroup_relation(hgr_id,hostgroup_hg_id,host_host_id) VALUES(" <<
					decodeObjectId(screen->getKey()) << "," <<
					decodeObjectId(screen->getType()->getKey()) << "," <<
					decodeObjectId(screen->getKey()) <<
					");"
				;
			}
		}

		CentreonConfigExportFunction::CentreonConfigExportFunction()
		{
	
		}



		bool CentreonConfigExportFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string CentreonConfigExportFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}

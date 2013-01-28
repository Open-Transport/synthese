
/** CentreonConfigExportFunction class implementation.
	@file DisplayScreenContentFunction.cpp

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

#include "CentreonConfigExportFunction.h"

#include "Conversion.h"
#include "RequestException.h"
#include "StaticFunctionRequest.h"
#include "UniqueStringsSet.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "Profile.h"
#include "User.h"
#include "UserTableSync.h"
#include "SecurityModule.h"
#include "StopArea.hpp"
#include "ProfileTableSync.h"
#include "DisplayMaintenanceRight.h"
#include "Interface.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "Env.h"
#include "PlainCharFilter.h"
#include "AlphanumericFilter.h"
#include "DisplayScreenContentFunction.h"
#include "URI.hpp"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace interfaces;
	using namespace db;
	using namespace security;
	using namespace departure_boards;

	template<> const string util::FactorableTemplate<Function,departure_boards::CentreonConfigExportFunction>::FACTORY_KEY("CentreonConfigExportFunction");

	namespace departure_boards
	{
		const string CentreonConfigExportFunction::PARAMETER_ACTION("ac");

		ParametersMap CentreonConfigExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ACTION, static_cast<int>(_action));
			return map;
		}

		void CentreonConfigExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_action = static_cast<Action>(map.get<int>(PARAMETER_ACTION));
		}



		util::ParametersMap CentreonConfigExportFunction::run(
			std::ostream& stream,
			const server::Request& request
		) const	{

			Env env;

			const string CHECK_SYNTHESE_COMMAND_ID("5000");
			const string CHECK_SYNTHESE_COMMAND_NAME("'check_synthese'");
			const string SYNTHESE_SERVER_ID("9999999");
			const string SYNTHESE_SERVER_CLASS("9999999");
			const string GENERATED_BY_SYNTHESE("'Generated by SYNTHESE3'");
			const string SYNTHESE_SERVER_NAME("'Serveur SYNTHESE3'");
			const string HARDWARE_SERVICE_ID("5000");
			const string HARDWARE_SERVICE_NAME("'Harware test'");
			const string SYNTHESE_PING_SERVICE_ID("5001");
			const string SYNTHESE_PING_SERVICE_NAME("'Ping'");
			const string DUMMY_CONTACT_ID("9999999");

			stream << "BEGIN;" << endl;

			if(_action == CLEAN)
			{
				// Deletion of existing objects
				stream <<
					"DELETE FROM contact WHERE contact_comment=" << GENERATED_BY_SYNTHESE << ";" << endl <<
					"DELETE FROM contactgroup WHERE cg_comment=" << GENERATED_BY_SYNTHESE << ";" << endl <<
					"DELETE FROM command WHERE command_id=" << CHECK_SYNTHESE_COMMAND_ID << ";" << endl <<
					"DELETE FROM host WHERE host_comment=" << GENERATED_BY_SYNTHESE << ";" << endl <<
					"DELETE FROM hostgroup WHERE hg_comment=" << GENERATED_BY_SYNTHESE << ";" << endl <<
					"DELETE FROM service WHERE service_comment=" << GENERATED_BY_SYNTHESE << ";" << endl
				;
			}
			else if(_action == GENERATE)
			{

				// Insertion of the SYNTHESE command
				stream <<
					"INSERT INTO command(command_id,command_name,command_line,command_type) VALUES(" <<
					CHECK_SYNTHESE_COMMAND_ID << "," << CHECK_SYNTHESE_COMMAND_NAME <<
					",'$USER1$#S#check_synthese.sh  \"http://" <<
					request.getHostName() << request.getClientURL() <<
					Request::PARAMETER_STARTER << Request::PARAMETER_SERVICE <<
					URI::PARAMETER_ASSIGNMENT << DisplayGetNagiosStatusFunction::FACTORY_KEY <<
					URI::PARAMETER_SEPARATOR << DisplayGetNagiosStatusFunction::PARAMETER_DISPLAY_SCREEN_ID <<
					URI::PARAMETER_ASSIGNMENT << "$HOSTADDRESS$\"',2);" << endl;



				// Services
				stream <<
					"INSERT INTO service(" <<
						"service_id,command_command_id,timeperiod_tp_id,timeperiod_tp_id2," << //4
						"service_description,service_is_volatile,service_max_check_attempts,service_normal_check_interval," << //4
						"service_retry_check_interval,service_active_checks_enabled,service_passive_checks_enabled," << //3
						"service_parallelize_check,service_obsess_over_service,service_check_freshness," << //3
						"service_event_handler_enabled,service_flap_detection_enabled,service_process_perf_data," << //3
						"service_retain_status_information,service_retain_nonstatus_information,service_notification_interval," << //3
						"service_notification_options,service_notifications_enabled,service_comment," << //3
						"service_register,service_activate" << //2
					") VALUES (" <<
						HARDWARE_SERVICE_ID << "," << CHECK_SYNTHESE_COMMAND_ID << ",1,1," << //4
						HARDWARE_SERVICE_NAME << ",'2',5000,1," << //4
						"1,'2','2'," << //3
						"'2','2','2'," << //3
						"'2','2','1'," << //3
						"'1','2',720," << //3
						"'w,u,c,r,f','2'," << GENERATED_BY_SYNTHESE << "," << //3
						"'1','1'" << //2
					");" << endl <<

				"INSERT INTO extended_service_information(esi_id,service_service_id) VALUES(" <<
					HARDWARE_SERVICE_ID << "," << HARDWARE_SERVICE_ID <<
				");" << endl <<

				"INSERT INTO service(" <<
					"service_id,command_command_id,timeperiod_tp_id,timeperiod_tp_id2," <<
					"service_description,service_is_volatile,service_max_check_attempts,service_normal_check_interval," <<
					"service_retry_check_interval,service_active_checks_enabled,service_passive_checks_enabled," <<
					"service_parallelize_check,service_obsess_over_service,service_check_freshness," <<
					"service_event_handler_enabled,service_flap_detection_enabled,service_process_perf_data," <<
					"service_retain_status_information,service_retain_nonstatus_information,service_notification_interval," <<
					"service_notification_options,service_notifications_enabled,service_comment," <<
					"command_command_id_arg,service_register,service_activate" <<
					") VALUES (" <<
					SYNTHESE_PING_SERVICE_ID << ",(SELECT command_id FROM command WHERE command_name='check_centreon_ping'),1,1," <<
					SYNTHESE_PING_SERVICE_NAME << ",'2',5000,1," <<
					"1,'2','2'," <<
					"'2','2','2'," <<
					"'2','2','1'," <<
					"'1','2',15," <<
					"'w,u,c,r,f','2'," << GENERATED_BY_SYNTHESE << "," <<
					"'!3!200,20%!400,50%','1','1'" <<
				");" << endl <<

				"INSERT INTO extended_service_information(esi_id,service_service_id) VALUES(" <<
					SYNTHESE_PING_SERVICE_ID << "," << SYNTHESE_PING_SERVICE_ID <<
				");" << endl
				;



				// Insertion of the hostgroups
				DisplayTypeTableSync::SearchResult types(
					DisplayTypeTableSync::Search(env)
				);
				UniqueStringsSet hostgroups;
				BOOST_FOREACH(const shared_ptr<const DisplayType>& type, types)
				{
					const string& hostgroup(hostgroups.getUniqueString(_ConvertToNagiosName(type->getName())));
					stream <<
						"INSERT INTO hostgroup(hg_id,hg_name,hg_alias,hg_comment,hg_activate) VALUES(" <<
							decodeObjectId(type->getKey()) << "," <<
							Conversion::ToDBString(hostgroup) << "," <<
							Conversion::ToDBString(hostgroup) << "," <<
							GENERATED_BY_SYNTHESE << "," <<
							"'1'" <<
						");" << endl <<

						"INSERT INTO host_service_relation(hsr_id,hostgroup_hg_id,service_service_id) VALUES (" <<
							decodeObjectId(type->getKey()) << "," <<
							decodeObjectId(type->getKey()) << "," <<
							HARDWARE_SERVICE_ID <<
						");" << endl
					;
				}
				stream <<
					"INSERT INTO hostgroup(hg_id,hg_name,hg_alias,hg_comment,hg_activate) VALUES(" <<
					SYNTHESE_SERVER_CLASS << "," <<
					SYNTHESE_SERVER_NAME << "," <<
					SYNTHESE_SERVER_NAME << "," <<
					GENERATED_BY_SYNTHESE << "," <<
					"'1'" <<
				");" << endl <<

				"INSERT INTO host_service_relation(hsr_id,hostgroup_hg_id,service_service_id) VALUES (" <<
					SYNTHESE_SERVER_CLASS << "," <<
					SYNTHESE_SERVER_CLASS << "," <<
					SYNTHESE_PING_SERVICE_ID <<
				");" << endl
				;


				// Insertion of the hosts
				StaticFunctionRequest<DisplayScreenContentFunction> displayRequest;

				DisplayScreenTableSync::SearchResult screens(
					DisplayScreenTableSync::Search(env)
				);
				UniqueStringsSet hosts;
				BOOST_FOREACH(const shared_ptr<const DisplayScreen>& screen, screens)
				{
					if(!screen->isMonitored()) continue;

					displayRequest.getFunction()->setScreen(screen);
					const string& host(hosts.getUniqueString(_ConvertToNagiosName(screen->getFullName())));

					stream <<
						"INSERT INTO host(host_id,command_command_id," <<
							"command_command_id_arg1,"
							"timeperiod_tp_id,timeperiod_tp_id2," <<
							"host_name,host_alias," <<
							"host_address,host_max_check_attempts," <<
							"host_check_interval,host_active_checks_enabled," <<
							"host_passive_checks_enabled,host_checks_enabled," <<
							"host_obsess_over_host,host_check_freshness," <<
							"host_event_handler_enabled,host_flap_detection_enabled," <<
							"host_process_perf_data,host_retain_status_information," <<
							"host_retain_nonstatus_information,host_notification_interval," <<
							"host_notification_options,host_notifications_enabled," <<
							"host_snmp_version,host_comment,host_register,host_activate" <<
						") VALUES(" <<
							decodeObjectId(screen->getKey()) << ",(SELECT command_id FROM command WHERE command_name='check_centreon_dummy')," <<
							"'!0!N/A'," <<
							"1,1," <<
							Conversion::ToDBString(host) << "," <<
							Conversion::ToDBString(host) << "," <<
							screen->getKey() << ",1000," <<
							"'1','2'," <<
							"'2','2'," <<
							"'2','2'," <<
							"'2','2'," <<
							"'2','2'," <<
							"'2',720," <<
							"'d,u,r,f','2'," <<
							"'0'," << GENERATED_BY_SYNTHESE << ",'1','1'" <<
						");" << endl <<

						"INSERT INTO extended_host_information(" <<
							"ehi_id,host_host_id,ehi_notes_url" <<
						") VALUES (" <<
							decodeObjectId(screen->getKey()) << "," <<
							decodeObjectId(screen->getKey()) << "," <<
							"'" << displayRequest.getURL() << "'"
						");" << endl <<

						"INSERT INTO hostgroup_relation(hgr_id,hostgroup_hg_id,host_host_id) VALUES(" <<
							decodeObjectId(screen->getKey()) << "," <<
							decodeObjectId(screen->getType()->getKey()) << "," <<
							decodeObjectId(screen->getKey()) <<
						");" << endl <<

						"INSERT INTO ns_host_relation(nagios_server_id,host_host_id) VALUES(" <<
							"(SELECT id FROM nagios_server ORDER BY id LIMIT 1)," <<
							decodeObjectId(screen->getKey()) <<
						");" << endl
					;
				}


				stream <<
					"INSERT INTO host(host_id,command_command_id," <<
						"timeperiod_tp_id,timeperiod_tp_id2," <<
						"host_name,host_alias," <<
						"host_address,host_max_check_attempts," <<
						"host_check_interval,host_active_checks_enabled," <<
						"host_passive_checks_enabled,host_checks_enabled," <<
						"host_obsess_over_host,host_check_freshness," <<
						"host_event_handler_enabled,host_flap_detection_enabled," <<
						"host_process_perf_data,host_retain_status_information," <<
						"host_retain_nonstatus_information,host_notification_interval," <<
						"host_notification_options,host_notifications_enabled," <<
						"host_snmp_version,host_comment,host_register,host_activate" <<
					") VALUES(" <<
						SYNTHESE_SERVER_ID << ",(SELECT command_id FROM command WHERE command_name='check_host_alive')," <<
						"1,1," <<
						SYNTHESE_SERVER_NAME << "," << SYNTHESE_SERVER_NAME << "," <<
						Conversion::ToDBString(request.getHostName()) << ",1000," <<
						"'1','2'," <<
						"'2','2'," <<
						"'2','2'," <<
						"'2','2'," <<
						"'2','2'," <<
						"'2',720," <<
						"'d,u,r,f','2'," <<
						"'0'," << GENERATED_BY_SYNTHESE << ",'1','1'" <<
					");" << endl <<

					"INSERT INTO extended_host_information(" <<
						"ehi_id,host_host_id" <<
						") VALUES (" <<
						SYNTHESE_SERVER_ID << "," << SYNTHESE_SERVER_ID <<
					");" << endl <<

					"INSERT INTO hostgroup_relation(hgr_id,hostgroup_hg_id,host_host_id) VALUES(" <<
						SYNTHESE_SERVER_ID << "," <<
						SYNTHESE_SERVER_CLASS << "," <<
						SYNTHESE_SERVER_ID  <<
					");" << endl <<

					"INSERT INTO ns_host_relation(nagios_server_id,host_host_id)" <<
					 " SELECT (SELECT id FROM nagios_server ORDER BY id LIMIT 1), host_id FROM host" <<
					 " WHERE host_comment=" << GENERATED_BY_SYNTHESE << ";" << endl <<

					 // Temporary
					 "INSERT INTO contact_host_relation(chr_id,host_host_id,contact_id) SELECT host_id,host_id,18 FROM host;" << endl
				;
			}

			stream << "COMMIT;";

			return util::ParametersMap();
		}

		CentreonConfigExportFunction::CentreonConfigExportFunction()
		{

		}



		bool CentreonConfigExportFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string CentreonConfigExportFunction::getOutputMimeType() const
		{
			return "text/plain";
		}



		std::string CentreonConfigExportFunction::_ConvertToNagiosName( const std::string& text )
		{
			stringstream sout;
			boost::iostreams::filtering_ostream out;
			out.push(AlphanumericFilter());
			out.push(PlainCharFilter());
			out.push(sout);
			out << text << flush;
			return sout.str();
		}
	}
}

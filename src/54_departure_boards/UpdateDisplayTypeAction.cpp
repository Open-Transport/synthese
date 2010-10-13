
/** UpdateDisplayTypeAction class implementation.
@file UpdateDisplayTypeAction.cpp

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

#include "UpdateDisplayTypeAction.h"

#include "DisplayTypeTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "DBLogModule.h"
#include "WebPageTableSync.h"
#include "Webpage.h"
#include "Interface.h"
#include "InterfaceTableSync.h"
#include "ObjectNotFoundException.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;
	using namespace dblog;
	using namespace security;
	using namespace cms;
		

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departure_boards::UpdateDisplayTypeAction>::FACTORY_KEY("updatedisplaytype");
	}

	namespace departure_boards
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_na";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtu_di";
		const string UpdateDisplayTypeAction::PARAMETER_AUDIO_INTERFACE_ID(Action_PARAMETER_PREFIX + "dtu_ai");
		const string UpdateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID(Action_PARAMETER_PREFIX + "dtu_mi");
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_ro";
		const string UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtu_st");
		const string UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS(Action_PARAMETER_PREFIX + "dtu_tc");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_MAIN_PAGE_ID(Action_PARAMETER_PREFIX + "mp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_ROW_PAGE_ID(Action_PARAMETER_PREFIX + "rp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_DESTINATION_PAGE_ID(Action_PARAMETER_PREFIX + "dp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID(Action_PARAMETER_PREFIX + "tp");


		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
				map.insert(PARAMETER_ID, _dt->getKey());
			map.insert(PARAMETER_NAME, _name);
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			map.insert(PARAMETER_MAX_STOPS_NUMBER, _max_stops_number);
			if(_displayMainPage.get())
			{
				map.insert(PARAMETER_DISPLAY_MAIN_PAGE_ID, _displayMainPage->getKey());
			}
			if(_displayRowPage.get())
			{
				map.insert(PARAMETER_DISPLAY_ROW_PAGE_ID, _displayRowPage->getKey());
			}
			if(_displayDestinationPage.get())
			{
				map.insert(PARAMETER_DISPLAY_DESTINATION_PAGE_ID, _displayDestinationPage->getKey());
			}
			if(_displayTransferDestinationPage.get())
			{
				map.insert(PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID, _displayTransferDestinationPage->getKey());
			}
			return map;
		}

		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			setTypeId(map.get<RegistryKeyType>(PARAMETER_ID));
			try
			{
				// Name
				_name = map.get<string>(PARAMETER_NAME);
				if (_name != _dt->getName())
				{
					if (_name.empty())
						throw ActionException("Le nom ne peut être vide.");

					Env env;
					DisplayTypeTableSync::Search(env, _name, optional<RegistryKeyType>(), 0, 1);
					if (!env.getRegistry<DisplayType>().empty())
						throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");
				}

				// Rows number
				_rows_number = map.get<int>(PARAMETER_ROWS_NUMBER);
				if (_rows_number < 0)
				{
					throw ActionException("Un nombre positif de rangées doit être choisi");
				}

				// Interface
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_INTERFACE_ID));
				if (id && *id)
				{
					_interface = InterfaceTableSync::Get(*id, *_env);
				}
				else
				{ // CMS webpage instead
					RegistryKeyType mid(map.getDefault<RegistryKeyType>(PARAMETER_DISPLAY_MAIN_PAGE_ID, 0));
					if(mid)
					{
						try
						{
							_displayMainPage = WebPageTableSync::Get(mid, *_env);
						}
						catch(ObjectNotFoundException<Webpage>&)
						{
							throw ActionException("No such main page");
						}

						RegistryKeyType rid(map.getDefault<RegistryKeyType>(PARAMETER_DISPLAY_ROW_PAGE_ID, 0));
						if(rid)
						{
							try
							{
								_displayRowPage = WebPageTableSync::Get(rid, *_env);
							}
							catch (ObjectNotFoundException<Webpage>&)
							{
								throw ActionException("No such row page");
							}

							RegistryKeyType did(map.getDefault<RegistryKeyType>(PARAMETER_DISPLAY_DESTINATION_PAGE_ID, 0));
							if(did)
							{
								try
								{
									_displayDestinationPage = WebPageTableSync::Get(did, *_env);
								}
								catch (ObjectNotFoundException<Webpage>&)
								{
									throw ActionException("No such destination page");
								}
							}

							RegistryKeyType sid(map.getDefault<RegistryKeyType>(PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID, 0));
							if(sid)
							{
								try
								{
									_displayTransferDestinationPage = WebPageTableSync::Get(sid, *_env);
								}
								catch (ObjectNotFoundException<Webpage>&)
								{
									throw ActionException("No such destination sorting page");
								}
							}
						}
					}
				}
				id = map.getOptional<RegistryKeyType>(PARAMETER_AUDIO_INTERFACE_ID);
				if (id && *id)
				{
					_audioInterface = InterfaceTableSync::Get(*id, *_env);
				}
				id = map.getOptional<RegistryKeyType>(PARAMETER_MONITORING_INTERFACE_ID);
				if (id && *id)
				{
					_monitoringInterface = InterfaceTableSync::Get(*id, *_env);
				}

				// Max stops number
				_max_stops_number = map.get<int>(PARAMETER_MAX_STOPS_NUMBER);
				if (_max_stops_number < 0)
				{
					throw ActionException("Un nombre positif d'arrêts intermédiaires doit être choisi");
				}

				// Time between checks
				_timeBetweenChecks = minutes(map.get<int>(PARAMETER_TIME_BETWEEN_CHECKS));
				if (_timeBetweenChecks.minutes() < 0)
				{
					throw ActionException("La durée entre les tests de supervision doit être positive.");
				}
			}
			catch(ObjectNotFoundException<Interface>& e)
			{
				throw ActionException("Interface not found / "+ e.getMessage());
			}
		}

		void UpdateDisplayTypeAction::run(Request& request)
		{
			// Log entry content
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _dt->getName(), _name);
			DBLogModule::appendToLogIfChange(log, "Interface d'affichage", (_dt->getDisplayInterface() != NULL) ? _dt->getDisplayInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Interface de supervision", (_dt->getMonitoringInterface() != NULL) ? _dt->getMonitoringInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Interface audio", (_dt->getAudioInterface() != NULL) ? _dt->getAudioInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Nombre de lignes", _dt->getRowNumber(), _rows_number);
			DBLogModule::appendToLogIfChange(log, "Nombre d'arrêts intermédiaires", _dt->getMaxStopsNumber(), _max_stops_number);
			DBLogModule::appendToLogIfChange(log, "Temps entre les contrôles de supervision", to_simple_string(_dt->getTimeBetweenChecks()), to_simple_string(_timeBetweenChecks));
			DBLogModule::appendToLogIfChange(
				log,
				"Page CMS principale",
				(_dt->getDisplayMainPage() != NULL) ? _dt->getDisplayMainPage()->getFullName() : "(aucune)",
				(_displayMainPage.get() != NULL) ? _displayMainPage->getFullName() : "(aucune)"
			);
			DBLogModule::appendToLogIfChange(
				log,
				"Page CMS pour rangée",
				(_dt->getDisplayRowPage() != NULL) ? _dt->getDisplayRowPage()->getFullName() : "(aucune)",
				(_displayRowPage.get() != NULL) ? _displayRowPage->getFullName() : "(aucune)"
			);
			DBLogModule::appendToLogIfChange(
				log,
				"Page CMS pour destination",
				(_dt->getDisplayDestinationPage() != NULL) ? _dt->getDisplayDestinationPage()->getFullName() : "(aucune)",
				(_displayDestinationPage.get() != NULL) ? _displayDestinationPage->getFullName() : "(aucune)"
			);
			DBLogModule::appendToLogIfChange(
				log,
				"Page CMS pour destination en correspondance",
				(_dt->getDisplayTransferDestinationPage() != NULL) ? _dt->getDisplayTransferDestinationPage()->getFullName() : "(aucune)",
				(_displayTransferDestinationPage.get() != NULL) ? _displayTransferDestinationPage->getFullName() : "(aucune)"
			);

			// Update
			_dt->setName(_name);
			_dt->setDisplayInterface(_interface.get());
			_dt->setMonitoringInterface(_monitoringInterface.get());
			_dt->setAudioInterface(_audioInterface.get());
			_dt->setRowNumber(_rows_number);
			_dt->setMaxStopsNumber(_max_stops_number);
			_dt->setTimeBetweenChecks(_timeBetweenChecks);
			_dt->setDisplayMainPage(_displayMainPage.get());
			_dt->setDisplayRowPage(_displayRowPage.get());
			_dt->setDisplayDestinationPage(_displayDestinationPage.get());
			_dt->setDisplayTransferDestinationPage(_displayTransferDestinationPage.get());
			DisplayTypeTableSync::Save(_dt.get());

			// Log
			ArrivalDepartureTableLog::addUpdateTypeEntry(_dt.get(), request.getUser().get(), log.str());
		}



		void UpdateDisplayTypeAction::setTypeId(
			util::RegistryKeyType id
		){
			try
			{
				// Display type
				_dt = DisplayTypeTableSync::GetEditable(id, *_env, UP_LINKS_LOAD_LEVEL);
			}			
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw ActionException("Display Type not found / "+ e.getMessage());
			}
		}



		bool UpdateDisplayTypeAction::isAuthorized(
			const Session* session
		) const {
			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE)
			;
		}
}	}

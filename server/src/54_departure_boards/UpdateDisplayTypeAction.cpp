
/** UpdateDisplayTypeAction class implementation.
@file UpdateDisplayTypeAction.cpp

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

#include "UpdateDisplayTypeAction.h"

#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "DBLogModule.h"
#include "DisplayTypeTableSync.h"
#include "MessageTypeTableSync.hpp"
#include "WebPageTableSync.h"
#include "Webpage.h"
#include "ObjectNotFoundException.h"
#include "ActionException.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "ParametersMap.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace cms;
	using namespace db;
	using namespace dblog;
	using namespace messages;
	using namespace security;
	using namespace server;
	using namespace util;


	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departure_boards::UpdateDisplayTypeAction>::FACTORY_KEY("updatedisplaytype");
	}

	namespace departure_boards
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_na";
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_ro";
		const string UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtu_st");
		const string UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS(Action_PARAMETER_PREFIX + "dtu_tc");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_MAIN_PAGE_ID(Action_PARAMETER_PREFIX + "mp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_ROW_PAGE_ID(Action_PARAMETER_PREFIX + "rp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_DESTINATION_PAGE_ID(Action_PARAMETER_PREFIX + "dp");
		const string UpdateDisplayTypeAction::PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID(Action_PARAMETER_PREFIX + "tp");
		const string UpdateDisplayTypeAction::PARAMETER_MONITORING_PARSER_PAGE_ID(Action_PARAMETER_PREFIX + "pp");
		const string UpdateDisplayTypeAction::PARAMETER_MESSAGE_IS_DISPLAYED_PAGE_ID = Action_PARAMETER_PREFIX + "_message_is_displayed_page_id";
		const string UpdateDisplayTypeAction::PARAMETER_MESSAGE_TYPE_ID = Action_PARAMETER_PREFIX + "_message_type_id";



		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
			{
				map.insert(PARAMETER_ID, _dt->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_rows_number)
			{
				map.insert(PARAMETER_ROWS_NUMBER, *_rows_number);
			}
			if(_max_stops_number)
			{
				map.insert(PARAMETER_MAX_STOPS_NUMBER, *_max_stops_number.value_or(0));
			}
			if(_displayMainPage)
			{
				if(_displayMainPage->get())
				{
					map.insert(PARAMETER_DISPLAY_MAIN_PAGE_ID, (*_displayMainPage)->getKey());
				}
			}
			if(_displayRowPage)
			{
				if(_displayRowPage->get())
				{
					map.insert(PARAMETER_DISPLAY_ROW_PAGE_ID, (*_displayRowPage)->getKey());
				}
			}
			if(_displayDestinationPage)
			{
				if(_displayDestinationPage->get())
				{
					map.insert(PARAMETER_DISPLAY_DESTINATION_PAGE_ID, (*_displayDestinationPage)->getKey());
				}
			}
			if(_displayTransferDestinationPage)
			{
				if(_displayTransferDestinationPage->get())
				{
					map.insert(PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID, (*_displayTransferDestinationPage)->getKey());
				}
			}
			if(_monitoringParserPage)
			{
				if(_monitoringParserPage->get())
				{
					map.insert(PARAMETER_MONITORING_PARSER_PAGE_ID, (*_monitoringParserPage)->getKey());
				}
			}
			return map;
		}



		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			setTypeId(map.get<RegistryKeyType>(PARAMETER_ID));
			try
			{
				// Name
				if(map.isDefined(PARAMETER_NAME))
				{
					_name = map.get<string>(PARAMETER_NAME);
					if(	*_name != _dt->get<Name>())
					{
						if (_name->empty())
							throw ActionException("Le nom ne peut être vide.");

						Env env;
						DisplayTypeTableSync::SearchResult v(
							DisplayTypeTableSync::Search(env, *_name, 0, 2)
						);
						BOOST_FOREACH(boost::shared_ptr<DisplayType> t, v)
						{
							if(t->getKey() == _dt->getKey())
							{
								continue;
							}
							throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");
						}
					}
				}

				// Rows number
				if(map.isDefined(PARAMETER_ROWS_NUMBER))
				{
					_rows_number = map.get<size_t>(PARAMETER_ROWS_NUMBER);
				}

				if(map.isDefined(PARAMETER_DISPLAY_MAIN_PAGE_ID))
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
					}
					else
					{
						_displayMainPage = boost::shared_ptr<const Webpage>();
					}
				}

				if(map.isDefined(PARAMETER_DISPLAY_ROW_PAGE_ID))
				{
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
					}
					else
					{
						_displayRowPage = boost::shared_ptr<const Webpage>();
					}
				}

				if(map.isDefined(PARAMETER_DISPLAY_DESTINATION_PAGE_ID))
				{
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
					else
					{
						_displayDestinationPage = boost::shared_ptr<const Webpage>();
					}
				}

				if(map.isDefined(PARAMETER_DISPLAY_TRANSFER_DESTINATION_PAGE_ID))
				{
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
					else
					{
						_displayTransferDestinationPage = boost::shared_ptr<const Webpage>();
					}
				}

				if(map.isDefined(PARAMETER_MONITORING_PARSER_PAGE_ID))
				{
					RegistryKeyType pid(map.getDefault<RegistryKeyType>(PARAMETER_MONITORING_PARSER_PAGE_ID, 0));
					if(pid)
					{
						try
						{
							_monitoringParserPage = WebPageTableSync::Get(pid, *_env);
						}
						catch (ObjectNotFoundException<Webpage>&)
						{
							throw ActionException("No such monitoring parser page");
						}
					}
					else
					{
						_monitoringParserPage = boost::shared_ptr<const Webpage>();
					}
				}

				// Max stops number
				if(map.isDefined(PARAMETER_MAX_STOPS_NUMBER))
				{
					_max_stops_number = map.getOptional<size_t>(PARAMETER_MAX_STOPS_NUMBER);
				}

				// Time between checks
				if(map.isDefined(PARAMETER_TIME_BETWEEN_CHECKS))
				{
					_timeBetweenChecks = minutes(map.get<int>(PARAMETER_TIME_BETWEEN_CHECKS));
					if (_timeBetweenChecks->minutes() < 0)
					{
						throw ActionException("La durée entre les tests de supervision doit être positive.");
					}
				}

				if(map.isDefined(PARAMETER_MESSAGE_IS_DISPLAYED_PAGE_ID))
				{
					RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_IS_DISPLAYED_PAGE_ID, 0));
					if(id)
					{
						try
						{
							_messageIsDisplayedPage = WebPageTableSync::Get(id, *_env);
						}
						catch (ObjectNotFoundException<Webpage>&)
						{
							throw ActionException("No such message is displayed rule page");
						}
					}
					else
					{
						_messageIsDisplayedPage = boost::shared_ptr<const Webpage>();
					}
				}

				// Message type
				if(map.isDefined(PARAMETER_MESSAGE_TYPE_ID))
				{
					RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_TYPE_ID, 0));
					if(id)
					{
						_messageType = MessageTypeTableSync::GetEditable(id, *_env);
					}
					else
					{
						_messageType = boost::shared_ptr<messages::MessageType>();
					}
				}
			}
			catch(ObjectNotFoundException<MessageType>& e)
			{
				throw ActionException("Message type not found"+ e.getMessage());
			}
		}

		void UpdateDisplayTypeAction::run(Request& request)
		{
			stringstream log;

			if(_name)
			{
				_dt->set<Name>(*_name);
				DBLogModule::appendToLogIfChange(log, "Nom", _dt->get<Name>(), *_name);
			}

			if(_rows_number)
			{
				_dt->set<RowsNumber>(*_rows_number);
				DBLogModule::appendToLogIfChange(
					log,
					"Nombre de lignes",
					lexical_cast<string>(_dt->get<RowsNumber>()),
					lexical_cast<string>(*_rows_number)
				);
			}

			if(_max_stops_number)
			{
				_dt->set<MaxStopsNumber>(**_max_stops_number);
				DBLogModule::appendToLogIfChange(
					log,
					"Nombre d'arrêts intermédiaires",
					_dt->get<MaxStopsNumber>() ? lexical_cast<string>(_dt->get<MaxStopsNumber>()) : string(),
					*_max_stops_number ? lexical_cast<string>(**_max_stops_number) : string()
				);
			}

			if(_timeBetweenChecks)
			{
				_dt->set<TimeBetweenChecks>(*_timeBetweenChecks);
				DBLogModule::appendToLogIfChange(
					log,
					"Temps entre les contrôles de supervision",
					to_simple_string(_dt->get<TimeBetweenChecks>()),
					to_simple_string(*_timeBetweenChecks)
				);
			}

			if(_displayMainPage)
			{
				_dt->set<DisplayMainPage>(*(const_cast<Webpage*>(_displayMainPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS principale",
					(_dt->get<DisplayMainPage>().get_ptr() != NULL) ? _dt->get<DisplayMainPage>()->getFullName() : "(aucune)",
					(_displayMainPage->get() != NULL) ? (*_displayMainPage)->getFullName() : "(aucune)"
				);
			}

			if(_displayRowPage)
			{
				_dt->set<DisplayRowPage>(*(const_cast<Webpage*>(_displayRowPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS pour rangée",
					(_dt->get<DisplayRowPage>().get_ptr() != NULL) ? _dt->get<DisplayRowPage>()->getFullName() : "(aucune)",
					(_displayRowPage->get() != NULL) ? (*_displayRowPage)->getFullName() : "(aucune)"
				);
			}

			if(_displayDestinationPage)
			{
				_dt->set<DisplayDestinationPage>(*(const_cast<Webpage*>(_displayDestinationPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS pour destination",
					(_dt->get<DisplayDestinationPage>().get_ptr() != NULL) ? _dt->get<DisplayDestinationPage>()->getFullName() : "(aucune)",
					(_displayDestinationPage->get() != NULL) ? (*_displayDestinationPage)->getFullName() : "(aucune)"
				);
			}

			if(_displayTransferDestinationPage)
			{
				_dt->set<DisplayTransferDestinationPage>(*(const_cast<Webpage*>(_displayTransferDestinationPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS pour destination en correspondance",
					(_dt->get<DisplayTransferDestinationPage>().get_ptr() != NULL) ? _dt->get<DisplayTransferDestinationPage>()->getFullName() : "(aucune)",
					(_displayTransferDestinationPage->get() != NULL) ? (*_displayTransferDestinationPage)->getFullName() : "(aucune)"
				);
			}

			if(_monitoringParserPage)
			{
				_dt->set<MonitoringParserPage>(*(const_cast<Webpage*>(_monitoringParserPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS pour parser les résultats de supervision",
					(_dt->get<MonitoringParserPage>().get_ptr() != NULL) ? _dt->get<MonitoringParserPage>()->getFullName() : "(aucune)",
					(_monitoringParserPage->get() != NULL) ? (*_monitoringParserPage)->getFullName() : "(aucune)"
				);
			}

			if(_messageIsDisplayedPage)
			{
				_dt->set<IsDisplayedMessagePage>(*(const_cast<Webpage*>(_messageIsDisplayedPage->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Page CMS définissant la règle d'affichage d'un message",
					(_dt->get<IsDisplayedMessagePage>().get_ptr() != NULL) ? _dt->get<IsDisplayedMessagePage>()->getFullName() : "(aucune)",
					(_messageIsDisplayedPage->get() != NULL) ? (*_messageIsDisplayedPage)->getFullName() : "(aucune)"
				);
			}

			if(_messageType)
			{
				_dt->set<MessageType>(*(const_cast<messages::MessageType*>(_messageType->get())));
				DBLogModule::appendToLogIfChange(
					log,
					"Type de message",
					(_dt->get<MessageType>().get_ptr() != NULL) ? _dt->get<MessageType>()->getName() : "(aucun)",
					(_messageType->get() != NULL) ? (*_messageType)->getName() : "(aucun)"
				);
			}

			// Update
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

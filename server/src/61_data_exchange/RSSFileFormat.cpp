
/** RSSFileFormat class implementation.
	@file RSSFileFormat.cpp

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

#include "RSSFileFormat.hpp"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipient.h"
#include "AlarmTableSync.h"
#include "BasicClient.h"
#include "BroadcastPointAlarmRecipient.hpp"
#include "Import.hpp"
#include "LineAlarmRecipient.hpp"
#include "SentScenarioTableSync.h"
#include "StopAreaAlarmRecipient.hpp"
#include "XmlToolkit.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace data_exchange;
	using namespace db;
	using namespace impex;
	using namespace messages;
	using namespace pt;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, RSSFileFormat>::FACTORY_KEY("RSS");
	}

	namespace data_exchange
	{
		const string RSSFileFormat::Importer_::PARAMETER_LINE_RECIPIENT_ID = "line_recipient_id";
		const string RSSFileFormat::Importer_::PARAMETER_STOP_RECIPIENT_ID = "stop_recipient_id";
		const string RSSFileFormat::Importer_::PARAMETER_BROADCAST_POINT_RECIPIENT_ID = "broadcast_point_recipient_id";



		bool RSSFileFormat::Importer_::_read(
		) const	{

			// Get the content
			try
			{
				// If no path defined, get the content online instead
				string content(
					BasicClient::Get(_address)
				);

				// Parsing of the content
				XMLResults pResults;
				XMLNode allNode = XMLNode::parseString(content.c_str(), "rss", &pResults);

				// Loop on items
				Items items;
				for(int c(0); c<allNode.nChildNode("channel"); ++c)
				{
					XMLNode channelNode(allNode.getChildNode("channel", c));
					for(int n(0); n<channelNode.nChildNode("item"); ++n)
					{
						XMLNode itemNode(channelNode.getChildNode("item", n));
						Item item;

						// Title
						if(itemNode.nChildNode("title"))
						{
							item.title = itemNode.getChildNode("title").getText();
						}

						// Content
						if(itemNode.nChildNode("description"))
						{
							XMLNode descriptionNode(itemNode.getChildNode("description"));
							if(descriptionNode.nText())
							{
								item.content = descriptionNode.getText();
							}
							else if(
								descriptionNode.nClear() &&
								descriptionNode.getClear(0).lpszValue
							){
								item.content = descriptionNode.getClear(0).lpszValue;
							}
						}

						// GUID
						if(itemNode.nChildNode("guid"))
						{
							item.guid = itemNode.getChildNode("guid").getText();
						}

						// Date
						if(itemNode.nChildNode("pubDate"))
						{
							try
							{
								stringstream s;
								string str(itemNode.getChildNode("pubDate").getText());
								s.str(str);
								s >> item.startDate;
							}
							catch (...)
							{
							}
						}

						// Jump over malformed items
						if(item.guid.empty())
						{
							continue;
						}

						items.push_back(item);
				}	}

				DataSource& dataSource(*_import.get<DataSource>());

				// Scenarios
				DataSource::LinkedObjects existingScenarios(
					dataSource.getLinkedObjects<SentScenario>()
				);
				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& existingScenario, existingScenarios)
				{
					_scenariosToRemove.insert(existingScenario.second->getKey());
				}

				// Loop on imported items
				BOOST_FOREACH(const Item& item, items)
				{
					boost::shared_ptr<SentScenario> updatedScenario;
					boost::shared_ptr<Alarm> updatedMessage;
					SentScenario* scenario(
						static_cast<SentScenario*>(
							dataSource.getObjectByCode<SentScenario>(lexical_cast<string>(item.guid))
					)	);
					Alarm* message(NULL);
					if(!scenario)
					{
						// Creation of the scenario
						updatedScenario.reset(
							new SentScenario(
								SentScenarioTableSync::getId()
						)	);
						updatedScenario->addCodeBySource(
							dataSource,
							lexical_cast<string>(item.guid)
						);
						updatedScenario->setIsEnabled(true);
						_env.getEditableRegistry<SentScenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new Alarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatedScenario->addMessage(*updatedMessage);
						scenario = updatedScenario.get();
						message = updatedMessage.get();
						_env.getEditableRegistry<Alarm>().add(updatedMessage);

						// Links creation
						if(_lineRecipientId)
						{
							boost::shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
							link->setKey(AlarmObjectLinkTableSync::getId());
							link->setAlarm(message);
							link->setObjectId(*_lineRecipientId);
							link->setRecipient(LineAlarmRecipient::FACTORY_KEY);
							_env.getEditableRegistry<AlarmObjectLink>().add(link);
						}
						if(_stopRecipientId)
						{
							boost::shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
							link->setKey(AlarmObjectLinkTableSync::getId());
							link->setAlarm(message);
							link->setObjectId(*_stopRecipientId);
							link->setRecipient(StopAreaAlarmRecipient::FACTORY_KEY);
							_env.getEditableRegistry<AlarmObjectLink>().add(link);
						}
						if(_broadcastPointRecipientId)
						{
							boost::shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
							link->setKey(AlarmObjectLinkTableSync::getId());
							link->setAlarm(message);
							link->setObjectId(*_broadcastPointRecipientId);
							link->setRecipient(BroadcastPointAlarmRecipient::FACTORY_KEY);
							_env.getEditableRegistry<AlarmObjectLink>().add(link);
						}

					}
					else
					{
						_scenariosToRemove.erase(scenario->getKey());

						// Message content
						const std::set<const Alarm*>& messages(scenario->getMessages());
						if(messages.size() != 1)
						{
							Log::GetInstance().warn(
								"Corrupted message : scenario should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							
							std::set<const Alarm*>::const_iterator it(messages.begin());
							for(++it; it != messages.end(); ++it)
							{
								_messagesToRemove.insert((*it)->getKey());
							}
						}
						message = const_cast<Alarm*>(*messages.begin());
						if(	message->getLongMessage() != item.content ||
							message->getShortMessage() != item.title
						){
							updatedMessage = AlarmTableSync::GetCastEditable<Alarm>(
								message->getKey(),
								_env
							);
						}

						// Scenario updates
						if(	scenario->getName() != item.title ||
							scenario->getPeriodStart() != item.startDate
						){
							updatedScenario = SentScenarioTableSync::GetCastEditable<SentScenario>(
								scenario->getKey(),
								_env
							);
						}
					}

					if(updatedMessage.get())
					{
						updatedMessage->setLongMessage(item.content);
						updatedMessage->setShortMessage(item.title);
					}
					if(updatedScenario.get())
					{
						updatedScenario->set<Name>(item.title);
						updatedScenario->setPeriodStart(item.startDate);
					}

				}
			}
			catch(...)
			{
				return false;
			}
			
			return true;
		}



		RSSFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			ConnectionImporter<RSSFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		ParametersMap RSSFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap pm;
			return pm;
		}



		void RSSFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			// Recipients
			string lineRecipient(map.getDefault<string>(PARAMETER_LINE_RECIPIENT_ID));
			if(!lineRecipient.empty())
			try
			{
				_lineRecipientId = lexical_cast<RegistryKeyType>(lineRecipient);
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("No such line recipient");
			}

			string stopRecipient(map.getDefault<string>(PARAMETER_STOP_RECIPIENT_ID));
			if(!stopRecipient.empty())
			try
			{
				_stopRecipientId = lexical_cast<RegistryKeyType>(stopRecipient);
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("No such stop recipient");
			}

			string broadcastRecipient(map.getDefault<string>(PARAMETER_BROADCAST_POINT_RECIPIENT_ID));
			if(!broadcastRecipient.empty())
			try
			{
				_broadcastPointRecipientId = lexical_cast<RegistryKeyType>(broadcastRecipient);
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("No such broadcast recipient");
			}

			// Datasource check
			if(!_import.get<DataSource>())
			{
				throw Exception("The import must link to a datasource");
			}
		}



		DBTransaction RSSFileFormat::Importer_::_save() const
		{
			//////////////////////////////////////////////////////////////////////////
			// Storage

			DBTransaction transaction;

			// Saving
			DBModule::SaveEntireEnv(_env, transaction);

			// Removals
			BOOST_FOREACH(RegistryKeyType id, _scenariosToRemove)
			{
				transaction.addDeleteStmt(id);
			}
			BOOST_FOREACH(RegistryKeyType id, _messagesToRemove)
			{
				transaction.addDeleteStmt(id);
			}

			return transaction;
		}
}	}


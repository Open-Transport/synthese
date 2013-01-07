
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

#include "AlarmRecipient.h"
#include "BasicClient.h"
#include "ScenarioTableSync.h"
#include "AlarmTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "XmlToolkit.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace data_exchange;
	using namespace db;
	using namespace impex;
	using namespace messages;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, RSSFileFormat>::FACTORY_KEY("RSS");
	}

	namespace data_exchange
	{
		const string RSSFileFormat::Importer_::PARAMETER_URL = "url";
		const string RSSFileFormat::Importer_::PARAMETER_RECIPIENT_KEY = "recipient_key";
		const string RSSFileFormat::Importer_::PARAMETER_RECIPIENT_ID = "recipient_id";



		bool RSSFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			boost::optional<const server::Request&> adminRequest
		) const	{

			// Get the content
			try
			{
				// If no path defined, get the content online instead
				string content;
				if(filePath.file_string().empty())
				{
					content = BasicClient::Get(_url);
				}
				else
				{
					string line;
					ifstream inFile;
					inFile.open(filePath.file_string().c_str());
					while(getline(inFile, line))
					{
						content += line;
					}
				}

				// Parsing of the content
				XMLResults pResults;
				XMLNode allNode = XMLNode::parseString(content.c_str(), "rss", &pResults);

				// Loop on items
				Items items;
				for(int c(0); c<allNode.nChildNode("channel"); ++c)
				{
					XMLNode channelNode(allNode.getChildNode("channel", c));
					for(int n(0); n<allNode.nChildNode("item"); ++n)
					{
						XMLNode itemNode(allNode.getChildNode("item", n));
						Item item;

						// Title
						if(itemNode.nChildNode("title"))
						{
							item.title = itemNode.getChildNode("title").getText();
						}

						// Content
						if(itemNode.nChildNode("description"))
						{
							item.content = itemNode.getChildNode("description").getText();
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


				// Scenarios
				DataSource::Links::mapped_type existingScenarios(
					_dataSource.getLinkedObjects<Scenario>()
				);
				BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingScenario, existingScenarios)
				{
					_scenariosToRemove.insert(existingScenario.second->getKey());
				}

				// Loop on imported items
				BOOST_FOREACH(const Item& item, items)
				{
					shared_ptr<SentScenario> updatedScenario;
					shared_ptr<Alarm> updatedMessage;
					SentScenario* scenario(
						static_cast<SentScenario*>(
							_dataSource.getObjectByCode<Scenario>(lexical_cast<string>(item.guid))
					)	);
					Alarm* message(NULL);
					if(!scenario)
					{
						// Creation of the scenario
						updatedScenario.reset(
							new SentScenario(
								ScenarioTableSync::getId()
						)	);
						updatedScenario->addCodeBySource(
							_dataSource,
							lexical_cast<string>(item.guid)
						);
						updatedScenario->setIsEnabled(true);
						_env.getEditableRegistry<Scenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new SentAlarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatedScenario->addMessage(*updatedMessage);
						scenario = updatedScenario.get();
						message = updatedMessage.get();
						_env.getEditableRegistry<Alarm>().add(updatedMessage);

						// Link creation
						shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
						link->setKey(AlarmObjectLinkTableSync::getId());
						link->setAlarm(message);
						link->setObjectId(_recipientId);
						link->setRecipientKey(_recipientKey);
						_env.getEditableRegistry<AlarmObjectLink>().add(link);
					}
					else
					{
						_scenariosToRemove.erase(scenario->getKey());

						// Message content
						const Scenario::Messages& messages(scenario->getMessages());
						if(messages.size() != 1)
						{
							Log::GetInstance().warn(
								"Corrupted message : scenario should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							
							SentScenario::Messages::const_iterator it(messages.begin());
							for(++it; it != messages.end(); ++it)
							{
								_messagesToRemove.insert((*it)->getKey());
							}
						}
						message = const_cast<Alarm*>(*messages.begin());
						if(	message->getLongMessage() != item.content ||
							message->getShortMessage() != item.title
						){
							updatedMessage = AlarmTableSync::GetCastEditable<SentAlarm>(
								message->getKey(),
								_env
							);
						}

						// Scenario updates
						if(	scenario->getName() != item.title ||
							scenario->getPeriodStart() != item.startDate
						){
							updatedScenario = ScenarioTableSync::GetCastEditable<SentScenario>(
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
						updatedScenario->setName(item.title);
						updatedScenario->setPeriodStart(item.startDate);
					}

				}
			}
			catch(...)
			{
				
			}
			
			return false;
		}



		RSSFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	OneFileTypeImporter<RSSFileFormat>(env, dataSource),
			Importer(env, dataSource)
		{}



		void RSSFileFormat::Importer_::displayAdmin( std::ostream& os, const server::Request& request ) const
		{

		}



		ParametersMap RSSFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap pm;
			return pm;
		}



		void RSSFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			// URL
			_url = map.get<string>(PARAMETER_URL);

			// Recipient key
			_recipientKey = map.get<string>(PARAMETER_RECIPIENT_KEY);
			if(!Factory<AlarmRecipient>::contains(_recipientKey))
			{
				throw Exception("No such recipient key");
			}

			// Recipient id
			_recipientId = map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID);
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


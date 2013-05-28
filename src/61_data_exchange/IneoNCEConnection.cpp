
/** IneoNCEConnection class implementation.
	@file IneoNCEConnection.cpp

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

#include "IneoNCEConnection.hpp"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "DBTransaction.hpp"
#include "LineAlarmRecipient.hpp"
#include "ScenarioTableSync.h"
#include "ScenarioFolderTableSync.h"
#include "MessagesLog.h"
#include "Env.h"
#include "Exception.h"
#include "Log.h"
#include "Session.h"
#include "CurrentJourney.hpp"
#include "ServerModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"
#include "XmlParser.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::gregorian;
using namespace boost::lambda;
using namespace boost::posix_time;
using namespace boost::system;
using namespace geos::geom;
using namespace std;

#define INEO_NCE_SCENARIO_NAME "_IneoNCEConnection_"
#define INEO_NCE_MESSAGE_ALARM "_IneoNCEConnection_Alarm"
#define INEO_NCE_MESSAGE_DETOUR1 "_IneoNCEConnection_Detour1"
#define INEO_NCE_MESSAGE_DETOUR2 "_IneoNCEConnection_Detour2"

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace messages;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace vehicle;
	
	
	namespace data_exchange
	{
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_HOST = "ineo_nce_host";
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_PORT = "ineo_nce_port";
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_DATASOURCE_ID = "ineo_nce_datasource_id";
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_WITH_LOCAL_MESSAGE = "ineo_nce_with_local_message";
		
		boost::shared_ptr<IneoNCEConnection> IneoNCEConnection::_theConnection(new IneoNCEConnection);
		

		IneoNCEConnection::IneoNCEConnection(
		):	_dataSource(NULL),
			_status(offline),
			_io_service(),
			_deadline(_io_service),
			_socket(_io_service),
			_iconv("ISO-8859-1","UTF-8")
		{
			// No deadline is required until the first socket operation is started. We
			// set the deadline to positive infinity so that the actor takes no action
			// until a specific deadline is set.
			_deadline.expires_at(boost::posix_time::pos_infin);

			// Start the persistent actor that checks for deadline expiry.
			checkDeadline();

		}



		void IneoNCEConnection::establishConnection()
		{
			// Attempt a connection
			// Get a list of endpoints corresponding to the server name.
			tcp::resolver resolver(_io_service);
			tcp::resolver::query query(_nceAddress, _ncePort);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::resolver::iterator end;
			_buf.reset(new boost::asio::streambuf);

			// Try each endpoint until we successfully establish a connection.
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				_socket.close();
				_socket.connect(*endpoint_iterator++, error);
			}
			if (error)
			{
				throw boost::system::system_error(error);
			}

			if(_status == connect)
			{
				_status = online;
			}
		}



		void IneoNCEConnection::read()
		{
			if(!_buf.get())
			{
				throw Exception("IneoNCEConnection : Buffer is null");
			}

			// Set a deadline for the asynchronous operation. Since this function uses
			// a composed operation (async_read_until), the deadline applies to the
			// entire operation, rather than individual reads from the socket.
			_deadline.expires_from_now(minutes(1));

			// Set up the variable that receives the result of the asynchronous
			// operation. The error code is set to would_block to signal that the
			// operation is incomplete. Asio guarantees that its asynchronous
			// operations will never fail with would_block, so any other value in
			// ec indicates completion.
			boost::system::error_code ec = boost::asio::error::would_block;

			// Start the asynchronous operation itself. The boost::lambda function
			// object is used as a callback and will update the ec variable when the
			// operation completes. The blocking_udp_client.cpp example shows how you
			// can use boost::bind rather than boost::lambda.
			boost::asio::async_read_until(_socket, *_buf, char(26), var(ec) = lambda::_1);

			// Block until the asynchronous operation has completed.
			do _io_service.run_one(); while (ec == boost::asio::error::would_block);

			if (ec)
			{
				throw boost::system::system_error(ec);
			}
		}



		void IneoNCEConnection::checkDeadline()
		{
			// Check whether the deadline has passed. We compare the deadline against
			// the current time since a new asynchronous operation may have moved the
			// deadline before this actor had a chance to run.
			if (_deadline.expires_at() <= deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
				_socket.close();

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				_deadline.expires_at(boost::posix_time::pos_infin);
			}

			// Put the actor back to sleep.
			_deadline.async_wait(boost::bind(&IneoNCEConnection::checkDeadline, this));
		}


		void IneoNCEConnection::RunThread()
		{
			// Main loop (never ends)
			while(true)
			{
				if(	_theConnection->_status == online ||
					_theConnection->_status == connect
				){
					try
					{
						ServerModule::SetCurrentThreadRunningAction();

						_theConnection->establishConnection();

						while(true)
						{
							if(_theConnection->_status == offline ||
								_theConnection->_status == connect
							){
								break;
							}

							// Read until eof
							_theConnection->read();

							_theConnection->handleData();
						}
					}
					catch(std::exception& e)
					{
						util::Log::GetInstance().info(
							string("IneoNCEConnection : ") + e.what()
						);
					}
					catch(thread_interrupted)
					{
						throw thread_interrupted();
					}
					catch(...)
					{

					}
				}
				
				VehicleModule::GetCurrentVehiclePosition().setStatus(VehiclePosition::UNKNOWN_STATUS);
				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}


		bool IneoNCEConnection::initScenario()
		{
			Env env;
			ScenarioTableSync::SearchResult scenarios(
				ScenarioTableSync::SearchSentScenarios(
					env,
					boost::optional<string>(INEO_NCE_SCENARIO_NAME),
					boost::optional<bool>(),
					boost::optional<bool>(),
					boost::optional<RegistryKeyType>(),
					boost::optional<int>(),
					boost::optional<size_t>(),
					true,
					false,
					false
			)	);
			if (scenarios.size())
			{
				DBTransaction transaction;
				_sentScenario = dynamic_pointer_cast<SentScenario>(scenarios[0]);
				_sentScenario->setIsEnabled(false);
				_sentScenario->setPeriodStart(second_clock::local_time());
				ScenarioTableSync::Save(_sentScenario.get(), transaction);
				transaction.run();
			}
			else
			{
				createScenario();
			}

			// Get the SentScenario (our Message)
			env.clear();
			AlarmTableSync::SearchResult alarms(
						AlarmTableSync::Search(env, _sentScenario->getKey()
			)	);
			if(alarms.size() > 0)
			{
				_messages[INEO_NCE_MESSAGE_ALARM] = dynamic_pointer_cast<SentAlarm>(alarms[0]);
				initMessage(INEO_NCE_MESSAGE_ALARM);
			}
			else
			{
				createMessage(INEO_NCE_MESSAGE_ALARM);
			}

			if(alarms.size() > 1)
			{
				_messages[INEO_NCE_MESSAGE_DETOUR1] = dynamic_pointer_cast<SentAlarm>(alarms[1]);
				initMessage(INEO_NCE_MESSAGE_DETOUR1);
			}
			else
			{
				createMessage(INEO_NCE_MESSAGE_DETOUR1);
			}

			if(alarms.size() > 2)
			{
				_messages[INEO_NCE_MESSAGE_DETOUR2] = dynamic_pointer_cast<SentAlarm>(alarms[2]);
				initMessage(INEO_NCE_MESSAGE_DETOUR2);
			}
			else
			{
				createMessage(INEO_NCE_MESSAGE_DETOUR2);
			}

			// Remove all the Alarm Links, will create one once we know our line
			clearScenarioLinks();

			return true;
		}

		void IneoNCEConnection::createScenario()
		{
			DBTransaction transaction;
			Env env(Env::GetOfficialEnv());
			// Creation of the scenario
			_sentScenario.reset(
						new SentScenario(
							ScenarioTableSync::getId()
			)	);
			_sentScenario->setIsEnabled(false);
			_sentScenario->setName(INEO_NCE_SCENARIO_NAME);
			_sentScenario->setPeriodStart(second_clock::local_time());
			env.getEditableRegistry<Scenario>().add(_sentScenario);
			ScenarioTableSync::Save(_sentScenario.get(), transaction);
			transaction.run();
		}

		void IneoNCEConnection::initMessage(const string &messageName)
		{
			_messages[messageName]->setScenario(_sentScenario.get());
			_messages[messageName]->setLongMessage("");
			_messages[messageName]->setShortMessage(messageName);
		}

		void IneoNCEConnection::createMessage(const string &messageName)
		{
			DBTransaction transaction;
			Env env(Env::GetOfficialEnv());
			_messages[messageName] = boost::shared_ptr<messages::SentAlarm>(new SentAlarm(AlarmTableSync::getId()));
			_messages[messageName]->setScenario(_sentScenario.get());
			_sentScenario->addMessage(*_messages[messageName]);
			_messages[messageName]->setLongMessage("");
			_messages[messageName]->setShortMessage(messageName);
			_messages[messageName]->setLevel(ALARM_LEVEL_WARNING);
			env.getEditableRegistry<Alarm>().add(_messages[messageName]);
			AlarmTableSync::Save(_messages[messageName].get(), transaction);
			transaction.run();
		}

		void IneoNCEConnection::clearScenarioLink(const string &messageName) const
		{
			Env env;
			AlarmObjectLinkTableSync::SearchResult alarmLinks(
						AlarmObjectLinkTableSync::Search(env, _messages[messageName]->getKey())
						);
			{
				DBTransaction transaction;
				boost::shared_ptr<Session> session(Session::New("0.0.0.0"));
				BOOST_FOREACH(boost::shared_ptr<AlarmObjectLink> alarmLink, alarmLinks)
				{
					AlarmObjectLinkTableSync::Remove(NULL, alarmLink->getKey(), transaction, false);
					DBTableSyncTemplate<AlarmObjectLinkTableSync>::Remove(session.get(), alarmLink->getKey(),
						transaction, false
					);
				}
				transaction.run();
				Session::Delete(session);
			}
		}

		void IneoNCEConnection::clearScenarioLinks() const
		{
			clearScenarioLink(INEO_NCE_MESSAGE_ALARM);
			clearScenarioLink(INEO_NCE_MESSAGE_DETOUR1);
			clearScenarioLink(INEO_NCE_MESSAGE_DETOUR2);
		}
		
		void IneoNCEConnection::createScenarioLink(const string &messageName,
											   CommercialLine *line) const
		{
			DBTransaction transaction;
			Env env(Env::GetOfficialEnv());
			_alarmObjectLink.reset(new AlarmObjectLink);
			_alarmObjectLink->setKey(AlarmObjectLinkTableSync::getId());
			_alarmObjectLink->setAlarm(_messages[messageName].get());
			_alarmObjectLink->setObjectId(line->getKey());
			_alarmObjectLink->setRecipient(LineAlarmRecipient::FACTORY_KEY);
			env.getEditableRegistry<AlarmObjectLink>().add(_alarmObjectLink);

			AlarmObjectLinkTableSync::Save(_alarmObjectLink.get(), transaction);
			transaction.run();
		}

		void IneoNCEConnection::createScenarioLinks(CommercialLine *line) const
		{
			createScenarioLink(INEO_NCE_MESSAGE_ALARM, line);
			createScenarioLink(INEO_NCE_MESSAGE_DETOUR1, line);
			createScenarioLink(INEO_NCE_MESSAGE_DETOUR2, line);
		}

		void IneoNCEConnection::setScenarioLine(CommercialLine *line) const
		{
			if(!_withLocalMessage)
			{
				return;
			}

			if(!line ||
				(_alarmObjectLink.get() && line->getKey() == _alarmObjectLink->getKey())
			)
			{
				// The line has not changed
				return;
			}

			clearScenarioLinks();
			createScenarioLinks(line);
		}

		// Return true if at least one message has data
		bool IneoNCEConnection::hasMessage() const
		{
			bool gotOne(false);
			BOOST_FOREACH(const MessageMap::value_type &message, _messages)
			{
				if(!message.second->getLongMessage().empty())
				{
					gotOne = true;
				}
			}
			return gotOne;
		}

		// If the given text message is empty then the scenario message is disabled
		void IneoNCEConnection::setMessage(const string &messageName,
										   const string &message) const
		{
			if(!_withLocalMessage)
			{
				return;
			}

			if(_messages[messageName].get())
			{
				DBTransaction transaction;
				_messages[messageName]->setLongMessage(message);

				// If there is at least one message enable the scenario
				_sentScenario->setIsEnabled(hasMessage());

				AlarmTableSync::Save(_messages[messageName].get(), transaction);
				ScenarioTableSync::Save(_sentScenario.get(), transaction);
				transaction.run();
			}
		}


		XMLNode IneoNCEConnection::ParseInput(
			const std::string& xml
		){
			XMLResults results;
			XMLNode allNode = XMLNode::parseString(xml.c_str(), NULL, &results);
			if (results.error != eXMLErrorNone)
			{
				throw Exception("IneoNCEConnection : Invalid XML");
			}
			return allNode;
		}



		void IneoNCEConnection::ParameterCallback( const std::string& name, const std::string& value )
		{
			// Host and port
			bool changed(false);
			if(name == MODULE_PARAM_INEO_NCE_HOST)
			{
				changed = (_theConnection->_nceAddress != value);
				_theConnection->_nceAddress = value;
			}
			else if(name == MODULE_PARAM_INEO_NCE_PORT)
			{
				changed = (_theConnection->_ncePort != value);
				_theConnection->_ncePort = value;
			}

			if(	changed
			){
				if(	!_theConnection->_nceAddress.empty() &&
					!_theConnection->_ncePort.empty()
				){
					_theConnection->_status = connect;
				}
				else
				{
					_theConnection->_status = offline;
				}
			}

			// Datasource
			if(name == MODULE_PARAM_INEO_NCE_DATASOURCE_ID)
			{
				_theConnection->_dataSource = NULL;
				try
				{
					_theConnection->_dataSource = Env::GetOfficialEnv().get<DataSource>(
						lexical_cast<RegistryKeyType>(value)
					).get();
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					// Log ?
				}
				catch(bad_lexical_cast&)
				{
					// Log ?
				}
			}

			// With Local Message
			if(name == MODULE_PARAM_INEO_NCE_WITH_LOCAL_MESSAGE)
			{
				_theConnection->_withLocalMessage = (value == "1");

				if(_theConnection->_withLocalMessage)
				{
					_theConnection->initScenario();
				}
			}
		}



		void IneoNCEConnection::handleData(
		) const	{

			// Copy the content obtained from the NCE into a string
			string bufStr;
			istream is(_buf.get());
			getline(is, bufStr, char(26));
			trim(bufStr);
			if(bufStr.empty())
			{
				return;
			}

			// Log the input
			if(Log::GetInstance().getLevel() <= Log::LEVEL_DEBUG)
			{
				string singleLine(bufStr);
				replace(singleLine.begin(), singleLine.end(), '\n', ' ');
				util::Log::GetInstance().debug("IneoNCEConnection : " + singleLine);
			}

			// Parsing
			XMLNode node(ParseInput(bufStr));
			if(node.isEmpty())
			{
				return;
			}
			XMLNode childNode(node.getChildNode(0));
			if(childNode.isEmpty())
			{
				return;
			}
			string tagName(childNode.getName());
			if(tagName == "GetId")
			{
				XMLNode nparcNode(childNode.getChildNode("NParc"));
				if(!nparcNode.isEmpty())
				{
					string vehicleNumber(nparcNode.getText());
					Vehicle* vehicle(VehicleModule::GetCurrentVehiclePosition().getVehicle());
					if(!vehicle || (vehicle->getNumber() != vehicleNumber) )
					{
						// Initial vehicle setting or
						// we are now running in a different vehicle
						try
						{
							VehicleModule::GetCurrentVehiclePosition().setVehicle(
								_theConnection->_dataSource->getObjectByCode<Vehicle>(vehicleNumber)
							);
						}
						catch(...)
						{
							VehicleModule::GetCurrentVehiclePosition().setVehicle(NULL);
							util::Log::GetInstance().error(
								"IneoNCEConnection : Could not find vehicle number " + vehicleNumber
							);
						}
					}
				}

				stringstream reply;
				ptime now(second_clock::local_time());
				reply <<
					"<IdReply><Type>SYNTHESE</Type><Id>1</Id>" <<
					"<Date>" << now.date().day() << "/" << now.date().month() << "/" << now.date().year() << "</Date>" <<
					"<Heure>" << now.time_of_day() << "</Heure>" <<
					"</IdReply>\n";

				boost::asio::write(_socket, boost::asio::buffer(reply.str()));
			}
			else if(tagName == "MsgLoc")
			{
				// EtatLoc
				XMLNode etatLocNode(childNode.getChildNode("EtatLoc"));
				if(!etatLocNode.isEmpty())
				{
					try
					{
						int etatLoc(lexical_cast<int>(etatLocNode.getText()));
						VehiclePosition::Status status(VehiclePosition::UNKNOWN_STATUS);
						switch(etatLoc)
						{
						case 0:
							status = VehiclePosition::OUT_OF_SERVICE;
							VehicleModule::GetCurrentJourney().setTerminusDepartureTime(posix_time::not_a_date_time);
							break;

						case 1:
							status = VehiclePosition::SERVICE;
							VehicleModule::GetCurrentJourney().setTerminusDepartureTime(posix_time::not_a_date_time);
							break;

						case 3:
						case 5:
							status = VehiclePosition::TERMINUS_START;
							// We will get a Departure Time from MsgInfo
							break;

						case 2:
						case 4:
						case 7:
						case 8:
							status = VehiclePosition::COMMERCIAL;
							VehicleModule::GetCurrentJourney().setTerminusDepartureTime(posix_time::not_a_date_time);
							break;

						case 6:
							status = VehiclePosition::DEAD_RUN_TRANSFER;
							VehicleModule::GetCurrentJourney().setTerminusDepartureTime(posix_time::not_a_date_time);
							break;

						case 9:
						case 10:
							status = VehiclePosition::NOT_IN_SERVICE;
							VehicleModule::GetCurrentJourney().setTerminusDepartureTime(posix_time::not_a_date_time);
							break;
						}
						VehicleModule::GetCurrentVehiclePosition().setStatus(status);
					}
					catch(bad_lexical_cast&)
					{
						util::Log::GetInstance().error("IneoNCEConnection : Failed to parse MsgLoc");
					}
				}

				// ZoneA
				XMLNode zoneANode(childNode.getChildNode("ZoneA"));
				bool lastInZone(VehicleModule::GetCurrentVehiclePosition().getInStopArea());
				bool inZoneHasChanged(false);
				if(!zoneANode.isEmpty())
				{
					bool inZone(zoneANode.getText() == string("1"));
					VehicleModule::GetCurrentVehiclePosition().setInStopArea(inZone);
					if(lastInZone && !inZone)
					{
						inZoneHasChanged = true;
					}
				}

				// Refresh of the next stops if the vehicle has exited from the stop area
				if(inZoneHasChanged)
				{
					CurrentJourney::NextStops nextStops;
					const CurrentJourney::NextStops& lastNextStops(
						VehicleModule::GetCurrentJourney().getNextStops()
					);
					BOOST_FOREACH(const CurrentJourney::NextStops::value_type& nextStop, lastNextStops)
					{
						if(nextStop.getInStopArea())
						{
							continue;
						}
						nextStops.push_back(nextStop);
					}
					VehicleModule::GetCurrentJourney().setNextStops(nextStops);
				}

				// Ord
				XMLNode ordANode(childNode.getChildNode("OrdA"));
				if(!ordANode.isEmpty())
				{
					_curOrd = ordANode.getText();
				}

				// Curv
				XMLNode curvNode(childNode.getChildNode("Curv"));
				if(!curvNode.isEmpty())
				{
					try
					{
						VehicleModule::GetCurrentVehiclePosition().setMeterOffset(
							lexical_cast<VehiclePosition::Meters>(
								curvNode.getText()
						)	);
					}
					catch(bad_lexical_cast&)
					{
						util::Log::GetInstance().error("IneoNCEConnection : Failed to parse Curv " +
													   string(curvNode.getText()));
					}
				}

				// GPS
				XMLNode gpsNode(childNode.getChildNode("GPS"));
				if(!gpsNode.isEmpty())
				{
					XMLNode longNode(gpsNode.getChildNode("Long"));
					XMLNode latNode(gpsNode.getChildNode("Lat"));
					if(!longNode.isEmpty() && !latNode.isEmpty())
					{
						try
						{
							// Convert Longitude suffix W to a negative value
							string longStr(longNode.getText());
							string longSuffix(longStr.substr(longStr.size()-1 , 1));
							double longVal(lexical_cast<double>(longStr.substr(0, longStr.size()-1)));
							if(longSuffix == "W")
							{
								longVal *= -1;
							}
							// Convert Latitude suffix S to a negative value
							string latStr(latNode.getText());
							string latSuffix(latStr.substr(latStr.size()-1 , 1));
							double latVal(lexical_cast<double>(latStr.substr(0, latStr.size()-1)));
							if(latSuffix == "S")
							{
								latVal *= -1;
							}
							// Create the coordinate point
							boost::shared_ptr<Point> point(
								CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
									longVal,
									latVal
							)	);
							VehicleModule::GetCurrentVehiclePosition().setGeometry(
								CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
									*point
							)	);
						}
						catch (bad_lexical_cast&)
						{
							util::Log::GetInstance().error("IneoNCEConnection : Failed to parse GPS Long / Lat " +
														   string(longNode.getText()) + " " +
														   string(latNode.getText()) );
						}
					}
				}

			}
			else if(tagName == "MsgVoyage")
			{
				XMLNode voyageNode(childNode.getChildNode("Voyage"));

				// Line object link
				XMLNode nligNode(
					(voyageNode.isEmpty() ? childNode : voyageNode).getChildNode("NLig")
				);
				if(	!nligNode.isEmpty() &&
					_theConnection->_dataSource
				){
					CommercialLine* line(
						_theConnection->_dataSource->getObjectByCode<CommercialLine>(nligNode.getText())
					);
					VehicleModule::GetCurrentJourney().setLine(line);
					setScenarioLine(line);
				}

				// Line number
				XMLNode nligIvNode(
					(voyageNode.isEmpty() ? childNode : voyageNode).getChildNode("NLigIv")
				);
				if(!nligIvNode.isEmpty())
				{
					VehicleModule::GetCurrentJourney().setLineNumber(nligNode.getText());
				}

				// Stop names and Ord Mna mapping
				XMLNode listeArretsNode(
					(voyageNode.isEmpty() ? childNode : voyageNode).getChildNode("ListeArrets")
				);
				if(!listeArretsNode.isEmpty())
				{
					_stopOrdMnaMap.clear();
					for(int i(0); i<listeArretsNode.nChildNode("BlocA"); ++i)
					{
						XMLNode blocANode(listeArretsNode.getChildNode("BlocA", i));
						if(blocANode.isEmpty())
						{
							continue;
						}

						// Stop identifier
						XMLNode mnaNode(blocANode.getChildNode("MnA"));
						XMLNode libANode(blocANode.getChildNode("LibA"));
						XMLNode ordANode(blocANode.getChildNode("OrdA"));
						if(	mnaNode.isEmpty() ||
							libANode.isEmpty() ||
							ordANode.isEmpty()
						){
							continue;
						}

						_stopMnaNameMap[mnaNode.getText()] = _iconv.convert(libANode.getText());
						_stopOrdMnaMap[ordANode.getText()] = mnaNode.getText();
					}
				}
			}
			else if(tagName == "MsgArrets")
			{
				ptime nceNow(not_a_date_time);
				ptime now(second_clock::local_time());
				XMLNode dateNode(childNode.getChildNode("Date"));
				XMLNode heureNode(childNode.getChildNode("Heure"));
				if(	!dateNode.isEmpty() &&
					!heureNode.isEmpty()
				){
					string dateStr(dateNode.getText());
					vector<string> parts;
					split(parts, dateStr, is_any_of("/"));
					if(parts.size() != 3)
					{
						util::Log::GetInstance().error("IneoNCEConnection : Failed to parse MsgArrets Date " +
													   string(dateNode.getText()) );
						throw Exception("IneoNCEConnection : Malformed date");
					}
					nceNow = ptime(
						date(
							lexical_cast<unsigned short>(parts[2]),
							lexical_cast<unsigned short>(parts[1]),
							lexical_cast<unsigned short>(parts[0])
						),
						duration_from_string(heureNode.getText())
					);
				}

				XMLNode listeArretsNode(childNode.getChildNode("ListeArrets"));
				if(	!listeArretsNode.isEmpty() &&
					!nceNow.is_not_a_date_time()
				){
					bool ok(true);
					CurrentJourney::NextStops nextStops;
					for(int i(0); i<listeArretsNode.nChildNode("BlocA"); ++i)
					{
						XMLNode blocANode(listeArretsNode.getChildNode("BlocA", i));
						if(blocANode.isEmpty())
						{
							ok = false;
							break;
						}

						// Stop identifier
						XMLNode mnaNode(blocANode.getChildNode("MnA"));
						XMLNode ordANode(blocANode.getChildNode("OrdA"));
						if(	mnaNode.isEmpty() ||
							ordANode.isEmpty()
						){
							ok = false;
							break;
						}
						string stopCode(mnaNode.getText());
						string ordA(ordANode.getText());

						// Adds the current stop if not in the list
						if(	i == 0 &&
							ordA != _curOrd
						){
							NextStop nextStop;

							// Stop identifier
							string curStopCode(_stopOrdMnaMap[_curOrd]);
							nextStop.setStopIdentifier(curStopCode);

							// In stop area
							nextStop.setInStopArea(
								VehicleModule::GetCurrentVehiclePosition().getInStopArea()
							);

							// Stop link
							StopPoint* stopPoint(NULL);
							if(_theConnection->_dataSource)
							{
								stopPoint = _theConnection->_dataSource->getObjectByCode<StopPoint>(curStopCode);
								nextStop.setStop(stopPoint);
							}

							// Stop name
							if(stopPoint && stopPoint->getConnectionPlace())
							{
								nextStop.setStopName(stopPoint->getConnectionPlace()->getName());
							}
							else if(!_stopMnaNameMap[curStopCode].empty())
							{
								nextStop.setStopName(_stopMnaNameMap[curStopCode]);
							}
							else
							{
								nextStop.setStopName(curStopCode);
							}

							// Arrival time
							ptime arrivalTime(second_clock::local_time());
							nextStop.setArrivalTime(arrivalTime);

							// Registration
							nextStops.push_back(nextStop);
						}

						NextStop nextStop;

						// Stop identifier
						nextStop.setStopIdentifier(stopCode);

						// In stop area
						if(	i == 0 &&
							ordA == _curOrd
						){
							nextStop.setInStopArea(
								VehicleModule::GetCurrentVehiclePosition().getInStopArea()
							);
						}

						// Stop link
						StopPoint* stopPoint(NULL);
						if(_theConnection->_dataSource)
						{
							stopPoint = _theConnection->_dataSource->getObjectByCode<StopPoint>(stopCode);
							nextStop.setStop(stopPoint);
						}

						// Stop name
						if(stopPoint && stopPoint->getConnectionPlace())
						{
							nextStop.setStopName(stopPoint->getConnectionPlace()->getName());
						}
						else if(!_stopMnaNameMap[stopCode].empty())
						{
							nextStop.setStopName(_stopMnaNameMap[stopCode]);
						}
						else
						{
							nextStop.setStopName(stopCode);
						}

						// Arrival time
						XMLNode haNode(blocANode.getChildNode("HA"));
						if(!haNode.isEmpty())
						{
							ptime arrivalTime(
								day_clock::local_day(),
								duration_from_string(haNode.getText())
							);
							time_duration arrivalDuration = arrivalTime - nceNow;
							arrivalTime = now + arrivalDuration;
							nextStop.setArrivalTime(arrivalTime);
						}

						// Registration
						nextStops.push_back(nextStop);
					}

					// Registration
					if(ok)
					{
						VehicleModule::GetCurrentJourney().setNextStops(nextStops);
					}
				}
			}
			else if(tagName == "GetStatus")
			{
				stringstream reply;
				ptime now(second_clock::local_time());
				reply <<
					"<StatusReply>" <<
					"<Id>1</Id>" <<
					"<Date>" << now.date().day() << "/" << now.date().month() << "/" << now.date().year() << "</Date>" <<
					"<Heure>" << now.time_of_day() << "</Heure>" <<
					"<Etat>0</Etat>" <<
					"<ListeTerm>" <<
					"<BlocTerm>" <<
					"<IdT>1</IdT>" <<
					"<EtatT>0</EtatT>" <<
					"</BlocTerm>" <<
					"<BlocTerm>" <<
					"<IdT>2</IdT>" <<
					"<EtatT>0</EtatT>" <<
					"</BlocTerm>" <<
					"</ListeTerm>" <<
					"</StatusReply>\n"
				;
				boost::asio::write(_socket, boost::asio::buffer(reply.str()));
			}
			else if(tagName == "MsgInfo")
			{
				XMLNode listeMsgNode(childNode.getChildNode("ListeMsg"));
				if(	!listeMsgNode.isEmpty() ){
					for(int i(0); i<listeMsgNode.nChildNode("BlocMsg"); ++i)
					{
						XMLNode blocMsgNode(listeMsgNode.getChildNode("BlocMsg", i));
						if(! blocMsgNode.isEmpty())
						{
							XMLNode contInfoNode(blocMsgNode.getChildNode("ContInf"));
							XMLNode typeInfoNode(blocMsgNode.getChildNode("TypeInf"));
							if(! typeInfoNode.isEmpty()
								&& ! contInfoNode.isEmpty()
							)
							{
								try
								{
									int typeInfo(lexical_cast<int>(typeInfoNode.getText()));
									string contInfoStr(contInfoNode.getText() ? contInfoNode.getText() : "");
									switch(typeInfo)
									{
									case 1:
										// This is an information message
										setMessage(INEO_NCE_MESSAGE_ALARM, _iconv.convert(contInfoStr));
										break;
									case 3:
										// This is a detour information message
										setMessage(INEO_NCE_MESSAGE_DETOUR1, _iconv.convert(contInfoStr));
										break;
									case 4:
										// This is a detour information message
										setMessage(INEO_NCE_MESSAGE_DETOUR2, _iconv.convert(contInfoStr));
										break;
									case 5:
										if(contInfoStr == "DEPART IMMINENT")
										{
											VehicleModule::GetCurrentJourney().setTerminusDepartureTime(
														ptime(second_clock::local_time().date())
														);
										}
										else if(contInfoStr.compare(0, 6, "DEPART") == 0)
										{
											vector<string> parts;
											split(parts, contInfoStr, is_any_of(" "));
											if(parts.size() != 4 ||
													( parts.size() == 4 &&
													  (parts[0] != "DEPART" ||
													   parts[1] != "DANS" ||
													   parts[3] != "MN")
													  )
													)
											{
												throw Exception("IneoNCEConnection : Malformed MsgInfo");
											}
											else
											{
												try
												{
													VehicleModule::GetCurrentJourney().setTerminusDepartureTime(
																ptime(second_clock::local_time().date(), 
																	  time_duration(0, lexical_cast<unsigned short>(parts[2]), 0))
															);
												}
												catch(bad_lexical_cast&)
												{
													throw Exception("IneoNCEConnection : Malformed MsgInfo minute");
												}
											}
										}
										break;
									}
								}
								catch(bad_lexical_cast&)
								{
									throw Exception("IneoNCEConnection : Malformed TypInfo number");
								}
							}
						}
					}
				}
			}

		}
}	}


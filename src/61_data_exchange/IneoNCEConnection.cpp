
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

#include "CommercialLine.h"
#include "Env.h"
#include "Exception.h"
#include "IConv.hpp"
#include "Log.h"
#include "CurrentJourney.hpp"
#include "ServerModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"
#include "XmlParser.h"

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio::ip;
using namespace geos::geom;
using namespace std;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace vehicle;
	
	
	namespace data_exchange
	{
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_HOST = "ineo_nce_host";
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_PORT = "ineo_nce_port";
		const string IneoNCEConnection::MODULE_PARAM_INEO_NCE_DATASOURCE_ID = "ineo_nce_datasource_id";
		
		boost::shared_ptr<IneoNCEConnection> IneoNCEConnection::_theConnection(new IneoNCEConnection);
		

		IneoNCEConnection::IneoNCEConnection(
		):	_dataSource(NULL),
			_status(offline)
		{}



		void IneoNCEConnection::InitThread()
		{
			// Local variables
			typedef std::map<std::string, std::string> StopMnaNameMap;
			StopMnaNameMap stopMnaNameMap;
			typedef std::map<std::string, std::string> StopOrdMnaMap;
			StopOrdMnaMap stopOrdMnaMap;
			string curOrd;
			IConv iconv("ISO-8859-1","UTF-8");

			// Main loop (never ends)
			while(true)
			{
				try
				{
					if(	_theConnection->_status == online ||
						_theConnection->_status == connect
					){
						ServerModule::SetCurrentThreadRunningAction();

						// Attempt a connection
						// Get a list of endpoints corresponding to the server name.
						asio::io_service io_service;
						tcp::resolver resolver(io_service);
						tcp::resolver::query query(_theConnection->_nceAddress, _theConnection->_ncePort);
						tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
						tcp::resolver::iterator end;

						// Try each endpoint until we successfully establish a connection.
						tcp::socket socket(io_service);
						boost::system::error_code error = boost::asio::error::host_not_found;
						while (error && endpoint_iterator != end)
						{
							socket.close();
							socket.connect(*endpoint_iterator++, error);
						}
						if (error)
						{
							throw boost::system::system_error(error);
						}

						if(_theConnection->_status == connect)
						{
							_theConnection->_status = online;
						}

						asio::streambuf buf;
						istream is(&buf);
						while(true)
						{
							if(_theConnection->_status == offline ||
								_theConnection->_status == connect
							){
								break;
							}

							// Read until eof
							boost::system::error_code error;
							boost::asio::read_until(socket, buf, char(26), error);

							// If the reading operation was broken by anything else than eof, leave the content loop
							if(error)
							{
								break;
							}

							// Copy the content obtained from the NCE into a string
							string bufStr;
							getline(is, bufStr, char(26));
							trim(bufStr);
							if(bufStr.empty())
							{
								continue;
							}

							// Log the input
							if(Log::GetInstance().getLevel() <= Log::LEVEL_DEBUG)
							{
								util::Log::GetInstance().info(
									bufStr
								);
							}

							// Parsing
							XMLNode node(ParseInput(bufStr));
							if(node.isEmpty())
							{
								continue;
							}
							XMLNode childNode(node.getChildNode(0));
							if(childNode.isEmpty())
							{
								continue;
							}
							string tagName(childNode.getName());
							if(tagName == "GetId")
							{
								XMLNode nparcNode(childNode.getChildNode("NParc"));
								if(!nparcNode.isEmpty())
								{
									string vehicleNumber(nparcNode.getText());
									Vehicle* vehicle(VehicleModule::GetCurrentVehiclePosition().getVehicle());
									if(vehicle)
									{
										if(_theConnection->_dataSource)
										{
											if(!vehicle->hasCodeBySource(*_theConnection->_dataSource, vehicleNumber))
											{
												try
												{
													vehicle = _theConnection->_dataSource->getObjectByCode<Vehicle>(vehicleNumber);
												}
												catch(...)
												{
													vehicle = NULL;
													// Log
												}
											}
										}
										else
										{
											if(lexical_cast<string>(vehicle->getKey()) != vehicleNumber)
											{
												try
												{
													vehicle = Env::GetOfficialEnv().getEditable<Vehicle>(
														lexical_cast<RegistryKeyType>(vehicleNumber)
													).get();
												}
												catch(...)
												{
													vehicle = NULL;
													// Log
												}
											}
										}
									}
									if(vehicle != VehicleModule::GetCurrentVehiclePosition().getVehicle())
									{
										VehicleModule::GetCurrentVehiclePosition().setVehicle(vehicle);
									}
								}

								stringstream reply;
								ptime now(second_clock::local_time());
								reply <<
									"<IdReply><Type>SYNTHESE</Type><Id>1</Id>" <<
									"<Date>" << now.date().day() << "/" << now.date().month() << "/" << now.date().year() << "</Date>" <<
									"<Heure>" << now.time_of_day() << "</Heure>" <<
									"</IdReply>\n";

								boost::asio::write(socket, boost::asio::buffer(reply.str()));
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
											break;

										case 1:
											status = VehiclePosition::SERVICE;
											break;

										case 2:
										case 3:
										case 4:
										case 5:
										case 7:
										case 8:
											status = VehiclePosition::COMMERCIAL;
											break;

										case 6:
											status = VehiclePosition::DEAD_RUN_TRANSFER;
											break;

										case 9:
										case 10:
											status = VehiclePosition::NOT_IN_SERVICE;
											break;
										}
										VehicleModule::GetCurrentVehiclePosition().setStatus(status);
									}
									catch(bad_lexical_cast&)
									{
										// Log
									}
								}

								// ZoneA
								XMLNode zoneANode(childNode.getChildNode("ZoneA"));
								if(!zoneANode.isEmpty())
								{
									VehicleModule::GetCurrentVehiclePosition().setInStopArea(
										zoneANode.getText() == string("1")
									);
								}

								// Ord
								XMLNode ordANode(childNode.getChildNode("OrdA"));
								if(!ordANode.isEmpty())
								{
									curOrd = ordANode.getText();
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
										// Log
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
											shared_ptr<Point> point(
												CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
													lexical_cast<double>(longNode.getText()),
													lexical_cast<double>(latNode.getText())
											)	);
											VehicleModule::GetCurrentVehiclePosition().setGeometry(
												CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
													*point
											)	);
										}
										catch (bad_lexical_cast&)
										{
											
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
									stopOrdMnaMap.clear();
									for(size_t i(0); i<listeArretsNode.nChildNode("BlocA"); ++i)
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

										stopMnaNameMap[mnaNode.getText()] = iconv.convert(libANode.getText());
										stopOrdMnaMap[ordANode.getText()] = mnaNode.getText();
									}
								}
							}
							else if(tagName == "MsgArrets")
							{
								XMLNode listeArretsNode(childNode.getChildNode("ListeArrets"));
								if(!listeArretsNode.isEmpty())
								{
									bool ok(true);
									CurrentJourney::NextStops nextStops;
									for(size_t i(0); i<listeArretsNode.nChildNode("BlocA"); ++i)
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
											ordA != curOrd
										){
											NextStop nextStop;

											// Stop identifier
											string curStopCode(stopOrdMnaMap[curOrd]);
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
											else if(!stopMnaNameMap[curStopCode].empty())
											{
												nextStop.setStopName(stopMnaNameMap[curStopCode]);
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
											ordA == curOrd
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
										else if(!stopMnaNameMap[stopCode].empty())
										{
											nextStop.setStopName(stopMnaNameMap[stopCode]);
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
								boost::asio::write(socket, boost::asio::buffer(reply.str()));
							}
						}
					}
				}
				catch(std::exception& e)
				{
					util::Log::GetInstance().info(
						e.what()
					);
				}

				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}



		XMLNode IneoNCEConnection::ParseInput(
			const std::string& xml
		){
			XMLResults results;
			XMLNode allNode = XMLNode::parseString(xml.c_str(), NULL, &results);
			if (results.error != eXMLErrorNone)
			{
				throw Exception("Invalid XML");
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
		}
}	}


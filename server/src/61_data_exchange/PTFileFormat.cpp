
/** PTFileFormat class implementation.
	@file PTFileFormat.cpp

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

#include "PTFileFormat.hpp"

#include "City.h"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "StopAreaTableSync.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "DestinationTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "ReservationContactTableSync.h"

#include <geos/operation/distance/DistanceOp.h>

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace pt;
	using namespace util;
	using namespace vehicle;
	using namespace impex;
	using namespace graph;
	using namespace admin;
	using namespace html;

	namespace data_exchange
	{
		const std::string PTFileFormat::ATTR_DISTANCE = "distance";
		const std::string PTFileFormat::ATTR_SOURCE_CODE = "source_code";
		const std::string PTFileFormat::ATTR_SOURCE_CITY_NAME = "source_city_name";
		const std::string PTFileFormat::ATTR_SOURCE_NAME = "source_name";
		const std::string PTFileFormat::ATTR_SOURCE_X = "source_x";
		const std::string PTFileFormat::ATTR_SOURCE_Y = "source_y";
		const std::string PTFileFormat::ATTR_SOURCE_SYNTHESE_X = "source_synthese_x";
		const std::string PTFileFormat::ATTR_SOURCE_SYNTHESE_Y = "source_synthese_y";
		const std::string PTFileFormat::TAG_LINKED_STOP_AREA = "linked_stop_area";
		const std::string PTFileFormat::TAG_LINKED_STOP_POINT = "linked_stop_point";
		const std::string PTFileFormat::TAG_STOP_AREA = "stop_area";
		const std::string PTFileFormat::TAG_STOP_POINT = "stop_point";
		const std::string PTFileFormat::TAG_SOURCE_LINE = "source_line";



		PTFileFormat::PTFileFormat(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// The created object is owned by the environment (it is not required to
		/// maintain the returned shared pointer)
		boost::shared_ptr<JourneyPattern> PTFileFormat::_createJourneyPattern(
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
			CommercialLine& line,
			const impex::DataSource& source
		) const {
			boost::shared_ptr<JourneyPattern> route(new JourneyPattern);
			route->setCommercialLine(&line);
			route->addCodeBySource(source, string());
			route->setKey(JourneyPatternTableSync::getId());
			_env.getEditableRegistry<JourneyPattern>().add(route);
			line.addPath(route.get());

			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& stop, stops)
			{
				boost::shared_ptr<LineStop> ls(
					new LineStop(
						LineStopTableSync::getId(),
						route.get(),
						rank,
						rank+1 < stops.size() && stop._departure,
						rank > 0 && stop._arrival,
						stop._metricOffset ? *stop._metricOffset : 0,
						**stop._stop.begin()
				)	);
				ls->set<ScheduleInput>((bool) stop._withTimes);
				ls->link(_env, true);
				_env.getEditableRegistry<LineStop>().add(ls);

				++rank;
			}

			return route;
		}



		//////////////////////////////////////////////////////////////////////////
		/// @return the created network object.
		/// The created object is owned by the environment (it is not required to
		/// maintain the returned shared pointer)
		TransportNetwork* PTFileFormat::_createOrUpdateNetwork(
			impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync>& networks,
			const std::string& id,
			const std::string& name,
			const impex::DataSource& source,
			const std::string& url,
			const std::string& timezone,
			const std::string& phone,
			const std::string& lang,
			const std::string& fareUrl
		) const {
			TransportNetwork* network;
			if(networks.contains(id))
			{
				set<TransportNetwork*> loadedNetworks(networks.get(id));
				if(loadedNetworks.size() > 1)
				{
					_logWarning("More than one network with key "+ id);
				}
				network = *loadedNetworks.begin();
				_logLoad(
					"Use of existing network "+ lexical_cast<string>(network->getKey()) +" ("+ network->getName() +")"
				);
			}
			else
			{
				network = new TransportNetwork(
					TransportNetworkTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				network->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<TransportNetwork>().add(boost::shared_ptr<TransportNetwork>(network));
				networks.add(*network);
				_logCreation(
					"Creation of the network with key "+ id +" ("+ name + ")"
				);
			}
			network->set<Name>(name);

			// Optional values

			// Timezone and language
			if (!timezone.empty())
				network->set<Timezone>(timezone);
			if (!lang.empty())
				network->set<Lang>(lang);

			// URL and phone
			// These are placed in a contact, so we check if a compatible one exists or create a new one
			// Compatible means it has the same phone and URL
			if ( ! url.empty() || ! phone.empty() )
			{
				boost::shared_ptr<pt::ReservationContact> contact;

				// Search for a contact with this phone and website
				BOOST_FOREACH(
					Registry<ReservationContact>::value_type crtContact,
					Env::GetOfficialEnv().getRegistry<ReservationContact>()
				){
					// If a contact has the same url and the phone is empty in the import, use it
					if (	(crtContact.second->get<PhoneExchangeNumber>() == phone || phone.empty() ) &&
							(crtContact.second->get<WebsiteURL>() == url || url.empty())
						)
					{
						contact = crtContact.second;
						_logInfo(
								"Using contact with key "+ boost::lexical_cast<std::string>(contact->getKey()) +" ("+ contact->get<Name>() +") for network "+ id +" ("+ name +")"
						);
						break;
					}
				}

				// Not found, creating a new one
				if (!contact)
				{
					contact.reset(new ReservationContact(
							pt::ReservationContactTableSync::getId()
					)	);
					contact->set<Name>(network->getName());
					contact->set<PhoneExchangeNumber>(phone);
					contact->set<WebsiteURL>(url);

					Importable::DataSourceLinks links;
					std::string contactId = boost::lexical_cast<std::string>(contact->getKey());
					links.insert(make_pair(&source, contactId));
					contact->setDataSourceLinksWithoutRegistration(links);
					_env.getEditableRegistry<ReservationContact>().add(boost::shared_ptr<ReservationContact>(contact));
					_logCreation(
						"Creation of a contact with key "+ contactId +" ("+ contact->getName() +") for network "+ id +" ("+ name +")"
					);
				}

				network->set<Contact>(*contact);
			}

			// Fare URL (to buy tickets)
			// Also in a contact.
			// If a contact has this same URL, use it. Else, create a new one.
			if ( ! fareUrl.empty() )
			{
				boost::shared_ptr<pt::ReservationContact> fareContact;

				// Search for a contact with this same URL
				BOOST_FOREACH(
					Registry<ReservationContact>::value_type crtContact,
					Env::GetOfficialEnv().getRegistry<ReservationContact>()
				){
					if (crtContact.second->get<WebsiteURL>() == fareUrl)
					{
						fareContact = crtContact.second;
						_logInfo(
								"Using fare contact with key "+ boost::lexical_cast<std::string>(fareContact->getKey()) +" ("+ fareContact->get<Name>() +") for network "+ id +" ("+ name +")"
						);
						break;
					}
				}

				// Not found, creating a new one
				if (!fareContact)
				{
					fareContact.reset(new ReservationContact(
							pt::ReservationContactTableSync::getId()
					)	);
					fareContact->set<Name>(network->getName() + " Fare");
					fareContact->set<WebsiteURL>(fareUrl);

					Importable::DataSourceLinks links;
					std::string contactId = boost::lexical_cast<std::string>(fareContact->getKey());
					links.insert(make_pair(&source, contactId));
					fareContact->setDataSourceLinksWithoutRegistration(links);
					_env.getEditableRegistry<ReservationContact>().add(boost::shared_ptr<ReservationContact>(fareContact));
					_logCreation(
						"Creation of a fare contact with key "+ contactId +" ("+ fareContact->get<Name>() +") for network "+ id +" ("+ name +")"
					);
				}

				network->set<FareContact>(*fareContact);
			}

			return network;
		}



		std::set<StopArea*> PTFileFormat::_getStopAreas(
			const impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			boost::optional<const std::string&> name,
			bool errorIfNotFound /*= true */
		) const {
			if(stopAreas.contains(id))
			{
				set<StopArea*> loadedStopAreas(stopAreas.get(id));

				stringstream logStream;
				logStream
					<< "Link between stop areas " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopArea* sp, loadedStopAreas)
				{
					logStream << sp->getKey() << " (" << sp->getFullName() << ") ";
				}
				_logLoad(logStream.str());

				return loadedStopAreas;
			}

			if(errorIfNotFound)
			{
				stringstream logStream;
				logStream << "Stop area not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				_logError(logStream.str());
			}
			return set<StopArea*>();
		}





		StopArea* PTFileFormat::_createStopArea(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			geography::City& city,
			boost::posix_time::time_duration defaultTransferDuration,
			bool mainStopArea,
			const impex::DataSource& source
		) const {
			StopArea* stopArea(
				new StopArea(
					StopAreaTableSync::getId(),
					true,
					defaultTransferDuration
			)	);
			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, id));
			stopArea->setDataSourceLinksWithoutRegistration(links);
			stopArea->setCity(&city);
			stopArea->setName(name);
			if(mainStopArea)
			{
				city.addIncludedPlace(*stopArea);
			}
			_env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(stopArea));
			stopAreas.add(*stopArea);

			_logCreation(
				"Creation of the stop area with key "+ id +" ("+ city.getName() +" "+ name + ")"
			);

			return stopArea;
		}



		set<StopArea*> PTFileFormat::_createOrUpdateStopAreas(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			const geography::City* city,
			bool updateCityIfExists,
			boost::posix_time::time_duration defaultTransferDuration,
			const impex::DataSource& source
		) const {
			// Load if possible
			set<StopArea*> result(_getStopAreas(stopAreas, id, name, false));

			// Create if necessary
			if(result.empty())
			{
				// Abort if undefined city
				if(!city)
				{
					_logWarning(
						"The stop area "+ name +" cannot be created because of undefined city."
					);
					return result;
				}

				result.insert(
					_createStopArea(
						stopAreas,
						id,
						name,
						*const_cast<geography::City*>(city), // Possible because of false in main parameter
						defaultTransferDuration,
						false,
						source
				)	);
			}

			// Update
			BOOST_FOREACH(StopArea* stopArea, result)
			{
				if(updateCityIfExists)
				{
					stopArea->setCity(const_cast<City*>(city));
				}
				stopArea->setName(name);
			}

			return result;
		}



		std::set<StopPoint*> PTFileFormat::_getStopPoints(
			const impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
			const std::string& id,
			boost::optional<const std::string&> name,
			bool errorIfNotFound
		) const {
			if(stopPoints.contains(id))
			{
				set<StopPoint*> loadedStopPoints(stopPoints.get(id));

				stringstream logStream;
				logStream << "Link between stops " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopPoint* sp, loadedStopPoints)
				{
					logStream << sp->getKey() << " (" << sp->getConnectionPlace()->getFullName() << ") ";
				}
				_logLoad(logStream.str());

				return loadedStopPoints;
			}
			if(errorIfNotFound)
			{
				stringstream logStream;
				logStream << "Stop not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				_logError(logStream.str());
			}
			return set<StopPoint*>();
		}



		StopPoint* PTFileFormat::_createStop(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			boost::optional<const std::string&> name,
			const StopArea& stopArea,
			const impex::DataSource& source
		) const {
			// Object creation
			StopPoint* stop(
				new StopPoint(
					StopPointTableSync::getId(),
					string(),
					&stopArea,
					boost::shared_ptr<Point>(),
					false
			)	);
			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, code));
			stop->setDataSourceLinksWithoutRegistration(links);
			_env.getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stop));
			stops.add(*stop);

			// Properties
			if(name)
			{
				stop->setName(*name);
			}

			// Log
			stringstream logStream;
			logStream << "Creation of the physical stop with key " << code;
			if(name)
			{
				logStream << " (" << *name <<  ")";
			}
			_logCreation(logStream.str());

			// Return
			return stop;
		}



		//////////////////////////////////////////////////////////////////////////
		///	Stop creation or update.
		/// The stop is identified by the specified datasource and the code.
		/// If the stop is not found, a stop is created in the specified stop area, if defined in
		/// the corresponding parameter. If not, a error message is written on the log stream.
		/// @param stops the stops which are registered to the data source
		/// @param code the code of the stop to update or create, as known by the datasource
		/// @param name the name of the stop (updated only if defined)
		/// @param stopArea the stop area which the stop belongs to (updated only if defined)
		/// @param geometry the location of the stop (updated only if defined)
		/// @param source the data source
		/// @param env the environment to read and populate
		/// @param logStream the stream to write the logs on
		/// @return the updated or created stops. Empty is no stop was neither found nor created in case
		/// of the stop area parameter is not defined
		set<StopPoint*> PTFileFormat::_createOrUpdateStop(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			boost::optional<const std::string&> name,
			boost::optional<const graph::RuleUser::Rules&> rules,
			boost::optional<const StopArea*> stopArea,
			boost::optional<const StopPoint::Geometry*> geometry,
			const impex::DataSource& source,
			bool doNotUpdate
		) const {
			// Load if possible
			bool creation(false);
			set<StopPoint*> result(_getStopPoints(stops, code, name, false));

			// Creation if necessary
			if(result.empty())
			{
				if(!stopArea || !*stopArea)
				{
					return result;
				}

				result.insert(
					_createStop(
						stops,
						code,
						name,
						**stopArea,
						source
				)	);
				creation = true;
			}
			else
			{
				stringstream logStream;
				logStream << "Link with existing stop " << (*result.begin())->getName() << " for stop " << code;
				if(name)
				{
					logStream << " (" << *name <<  ")";
				}
				_logLoad(logStream.str());
			}

			// Update
			if(!creation && doNotUpdate)
			{
				return result;
			}

			BOOST_FOREACH(StopPoint* stop, result)
			{
				if(name)
				{
					stop->setName(*name);
				}

				if(geometry && *geometry)
				{
					stop->setGeometry(
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(**geometry)
					);
				}

				if(rules)
				{
					stop->setRules(*rules);
				}

				if(stopArea)
				{
					stop->setHub(*stopArea);
				}
			}
			return result;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Stop creation or update with creation of a stop area based on the name if necessary.
		set<StopPoint*> PTFileFormat::_createOrUpdateStopWithStopAreaAutocreation(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			const std::string& name,
			boost::optional<const StopPoint::Geometry*> geometry,
			const geography::City& cityForStopAreaAutoGeneration,
			boost::optional<boost::posix_time::time_duration> defaultTransferDuration,
			const impex::DataSource& source,
			boost::optional<const graph::RuleUser::Rules&> rules
		) const {
			// Load if possible
			set<StopPoint*> result(_getStopPoints(stops, code, name, false));

			// Creation if necessary
			if(result.empty())
			{
				// Search for an existing stop area
				StopArea* curStop(NULL);
				StopAreaTableSync::SearchResult stopAreas(
					StopAreaTableSync::Search(
						_env,
						cityForStopAreaAutoGeneration.getKey(),
						logic::indeterminate,
						optional<string>(),
						name
				)	);
				if(stopAreas.empty())
				{
					BOOST_FOREACH(Registry<StopArea>::value_type stopArea, _env.getRegistry<StopArea>())
					{
						if((stopArea.second->getName() == name) && (cityForStopAreaAutoGeneration.getKey() == stopArea.second->getCity()->getKey()))
						{
							curStop = stopArea.second.get();
							break;
						}
					}

					if(!curStop)
					{
						curStop = new StopArea(StopAreaTableSync::getId(), true);
						Importable::DataSourceLinks links;
						links.insert(make_pair(&source, string()));
						curStop->setDataSourceLinksWithoutRegistration(links);
						if(defaultTransferDuration)
						{
							curStop->setDefaultTransferDelay(*defaultTransferDuration);
						}
						curStop->setName(name);
						curStop->setCity(const_cast<City*>(&cityForStopAreaAutoGeneration));
						_env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(curStop));
						_logCreation(
							"Auto generation of the commercial stop for stop "+ code +" ("+ name +")"
						);
					}
					else
					{
						_logLoad(
							"Link with existing commercial stop "+ curStop->getFullName() +" for stop "+ code +" ("+ name +")"
						);
					}
				}
				else
				{
					curStop = stopAreas.begin()->get();
					_logLoad(
						"Link with existing commercial stop "+ curStop->getFullName() +" for stop "+ code +" ("+ name +")"
					);
				}

				// Stop creation
				result.insert(
					_createStop(
						stops,
						code,
						name,
						*curStop,
						source
				)	);
			}
			else
			{
				_logLoad(
					"Link with existing stop "+ (*result.begin())->getName() +" for stop "+ code +" ("+ name +")"
				);
			}

			// Update
			BOOST_FOREACH(StopPoint* stop, result)
			{
				if(stop->getName() != name)
				{
					_logInfo(
						"Stop "+ code +" ("+ stop->getName() +") renamed to "+ name
					);
					stop->setName(name);
				}
				if(geometry && *geometry)
				{
					boost::shared_ptr<const Geometry> exGeom(stop->getGeometry());
					stop->setGeometry(
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(**geometry)
					);
					boost::shared_ptr<const Geometry> newGeom(stop->getGeometry());

					// If there was a previous geometry
					if(exGeom && (!newGeom || exGeom->distance(newGeom.get()) > 1))
					{
						_logInfo(
							"Stop "+ code +" ("+ stop->getName() +") moved from " + exGeom->toString() + " to " + (newGeom ? newGeom->toString() : "POINT(0 0)")
						);
				}
				}

				if(rules)
				{
					stop->setRules(*rules);
				}
			}
			return result;
		}



		//////////////////////////////////////////////////////////////////////////
		/// @return the created network object.
		/// The created object is owned by the environment (it is not required to
		/// maintain the returned shared pointer)
		/// The network of the line is never changed if an existing line is returned.
		CommercialLine* PTFileFormat::_createOrUpdateLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			optional<const std::string&> name,
			optional<const std::string&> shortName,
			boost::optional<util::RGBColor> color,
			TransportNetwork& defaultNetwork,
			const impex::DataSource& source,
			bool restrictInDefaultNetwork
		) const {
			CommercialLine* line(
				_getLine(
					lines,
					id,
					source,
					restrictInDefaultNetwork ? optional<TransportNetwork&>(defaultNetwork) : optional<TransportNetwork&>()
			)	);
			if(!line)
			{
				line = new CommercialLine(CommercialLineTableSync::getId());

				stringstream logStream;
				logStream << "Creation of the commercial line with key " << id;
				if(name)
				{
					logStream << " (" << *name <<  ")";
				}
				_logCreation(logStream.str());

				line->setParent(defaultNetwork);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				line->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<CommercialLine>().add(boost::shared_ptr<CommercialLine>(line));
				lines.add(*line);
			}

			if(name)
			{
				line->setName(*name);
			}
			if(shortName)
			{
				line->setShortName(*shortName);
			}
			if(color)
			{
				line->setColor(color);
			}

			return line;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Search for an existing route which matches with the defined parameters, or create a new one if no existing route is compliant.
		/// @param line The line
		/// @param removeOldCodes Removes codes on similar routes with the same code for the data source (routes with different stops are not cleaned)
		/// @pre The line object must link to all existing routes (use JourneyPatternTableSync::Search to populate the object)
		/// @author Hugues Romain
		JourneyPattern* PTFileFormat::_createOrUpdateRoute(
			pt::CommercialLine& line,
			boost::optional<const std::string&> id,
			boost::optional<const std::string&> name,
			boost::optional<const std::string&> destination,
			boost::optional<Destination*> destinationObj,
			boost::optional<const RuleUser::Rules&> rules,
			boost::optional<bool> wayBack,
			vehicle::RollingStock* rollingStock,
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
			const impex::DataSource& source,
			bool removeOldCodes,
			bool updateMetricOffsetOnUpdate,
			bool attemptToCopyExistingGeometries,
			bool allowDifferentStopPointsInSameStopArea
		) const {
			// Declaration
			bool creation(false);

			// Attempting to find an existing route by value comparison
			JourneyPattern* result(NULL);
			BOOST_FOREACH(Path* route, line.getPaths())
			{
				// Avoid junctions
				if(!dynamic_cast<JourneyPattern*>(route))
				{
					continue;
				}

				JourneyPattern* jp(static_cast<JourneyPattern*>(route));

				if(!jp->hasLinkWithSource(source))
				{
					continue;
				}

				if(	(!rollingStock || jp->getRollingStock() == rollingStock) &&
					(!id || jp->hasCodeBySource(source, *id)) &&
					(!rules || jp->getRules() == *rules) &&
					(!wayBack || jp->getWayBack() == *wayBack) &&
					(	(allowDifferentStopPointsInSameStopArea && jp->compareStopAreas(servedStops)) ||
						(!allowDifferentStopPointsInSameStopArea && *jp == servedStops)
					)
				){
					if(!result)
					{
						_logLoad(
							"Use of route "+ lexical_cast<string>(jp->getKey()) +" ("+ jp->getName() +") for "+ (id ? *id : string("unknown")) +")"
						);
						result = jp;
						if(!id)
						{
							break;
						}
					}
					else
					{
						if(removeOldCodes && id)
						{
							jp->removeSourceLink(source, *id);
							jp->addCodeBySource(source, string());
							_logInfo(
								"Code "+ *id +" was removed from route "+ lexical_cast<string>(jp->getKey())
							);
						}
						else
						{
							_logWarning(
								"Route "+ *id +") is defined twice or more."
							);
						}
					}
				}
			}

			// Create a new route if necessary
			if(!result)
			{
				creation = true;
				result = new JourneyPattern(
					JourneyPatternTableSync::getId()
				);
				_logCreation(
					"Creation of route " + lexical_cast<string>(result->getKey()) +
							" " + (name ? *name : string()) +
							" for "+ (id ? *id : string("unknown"))
				);

				// Line link
				result->setCommercialLine(&line);
				line.addPath(result);

				// Source links
				Importable::DataSourceLinks links;
				if(id)
				{
					links.insert(make_pair(&source, *id));
				}
				else
				{
					links.insert(make_pair(&source, string()));
				}
				result->setDataSourceLinksWithoutRegistration(links);

				// Storage in the environment
				_env.getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(result));

				// Served stops
				size_t rank(0);
				BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
				{
					// Check if the stop is linked to an existing synthese object
					// If not, jump over the stop
					if(stop._stop.empty())
					{
						_logWarning(
							"Stop at rank "+ lexical_cast<string>(rank) +" on route "+ (id ? *id : string()) +" "+ (name ? *name : string()) +" is ignored because not linked to a synthese stop"
						);
						continue;
					}

					boost::shared_ptr<LineStop> ls(
						new LineStop(
							LineStopTableSync::getId(),
							result,
							rank,
							rank+1 < servedStops.size() && stop._departure,
							rank > 0 && stop._arrival,
							stop._metricOffset ? *stop._metricOffset : 0,
							**stop._stop.begin()
					)	);
					ls->set<ScheduleInput>(stop._withTimes ? *stop._withTimes : true);
					ls->link(_env, true);
					_env.getEditableRegistry<LineStop>().add(ls);
					++rank;
				}

				// Geometries
				if(attemptToCopyExistingGeometries)
				{
					for(JourneyPattern::LineStops::const_iterator itEdge(result->getLineStops().begin());
						itEdge != result->getLineStops().end();
						++itEdge
					){
						JourneyPattern::LineStops::const_iterator it2(itEdge);
						++it2;
						if(it2 == result->getLineStops().end())
						{
							break;
						}

						// Don't update already defined geometry
						if(	(*itEdge)->get<LineStringGeometry>() ||
							!(*itEdge)->get<LineNode>() ||
							!dynamic_cast<const StopPoint*>(&*(*itEdge)->get<LineNode>()) ||
							!(*it2)->get<LineNode>() ||
							!dynamic_cast<const StopPoint*>(&*(*it2)->get<LineNode>())
						){
							continue;
						}


						Env env2;
						boost::shared_ptr<LineStop> templateObject(
							LineStopTableSync::SearchSimilarLineStop(
								dynamic_cast<const StopPoint&>(*(*itEdge)->get<LineNode>()),
								dynamic_cast<const StopPoint&>(*(*it2)->get<LineNode>()),
								env2
						)	);
						if(templateObject.get())
						{
							(*itEdge)->set<LineStringGeometry>(templateObject->get<LineStringGeometry>());
						}
				}	}
			}


			//////////////////////////////////////////////////////////////////////////
			// Updates

			// Metric offsets
			if(creation || updateMetricOffsetOnUpdate)
			{
				JourneyPattern::LineStops::const_iterator it(result->getLineStops().begin());
				BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
				{
					if(stop._metricOffset)
					{
						const_cast<LineStop*>(*it)->set<MetricOffsetField>(*stop._metricOffset);
					}
					if(it == result->getLineStops().end())
					{
						_logWarning(
							"Inconsistent stop number on route "+ (id ? *id : string()) +" "+ (name ? *name : string()) +" is ignored because not linked to a synthese stop"
						);
						break;
					}
					++it;
				}
			}

			// Geometries
			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
			{
				if(rank >= result->getEdges().size())
				{
					_logWarning(
						"Inconsistent stop number on route "+ (id ? *id : string()) +" "+ (name ? *name : string()) +" is ignored because not linked to a synthese stop"
					);
					break;
				}
				if(stop._geometry.get())
				{
					const_cast<LineStop*>(result->getLineStop(rank))->set<LineStringGeometry>(stop._geometry);
				}
				++rank;
			}

			// Name
			if(name)
			{
				result->setName(*name);
			}

			result->setNetwork(line.getNetwork());
			result->setTimetableName(line.getShortName());

			// Destination text
			if(destination)
			{
				result->setDirection(*destination);
			}

			// Destination sign id
			if(destinationObj)
			{
				result->setDirectionObj(*destinationObj);
			}

			// Transport mode
			if(rollingStock)
			{
				result->setRollingStock(rollingStock);
			}

			// Use rules
			if(rules)
			{
				result->setRules(*rules);
			}

			// Wayback
			if(wayBack)
			{
				result->setWayBack(*wayBack);
			}

			return result;
		}



		ScheduledService* PTFileFormat::_createOrUpdateService(
			JourneyPattern& route,
			const SchedulesBasedService::Schedules& departureSchedules,
			const SchedulesBasedService::Schedules& arrivalSchedules,
			const std::string& number,
			const impex::DataSource& source,
			boost::optional<const std::string&> team,
			boost::optional<const graph::RuleUser::Rules&> rules,
			boost::optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&> servedVertices,
			boost::optional<const std::string&> id
		) const {
			// Comparison of the size of schedules and the size of the route
			if(	route.getScheduledStopsNumber() != departureSchedules.size() ||
				route.getScheduledStopsNumber() != arrivalSchedules.size()
			){
				_logWarning(
					"Inconsistent schedules size in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
				return NULL;
			}

			// Checks for schedules validity
			BOOST_FOREACH(const time_duration& td, departureSchedules)
			{
				if(td.is_not_a_date_time())
				{
					_logWarning(
						"At least an undefined time in departure schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}
			BOOST_FOREACH(const time_duration& ta, arrivalSchedules)
			{
				if(ta.is_not_a_date_time())
				{
					_logWarning(
						"At least an undefined time in arrival schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}

			// Search for a corresponding service
			ScheduledService* result(NULL);
			{
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
					*route.sharedServicesMutex
				);
				BOOST_FOREACH(Service* tservice, route.getAllServices())
				{
					ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));

					if(!curService) continue;

					if(	curService->getServiceNumber() == number &&
						curService->comparePlannedSchedules(departureSchedules, arrivalSchedules) &&
						(!servedVertices || curService->comparePlannedStops(*servedVertices)) &&
						(team ? curService->getTeam() == *team : true) &&
						(rules ? curService->getRules() == *rules : true)
					){
						result = curService;
						break;
					}
				}
			}

			// If not found creation
			if(!result)
			{
				result = new ScheduledService(
					ScheduledServiceTableSync::getId(),
					number,
					&route
				);
				result->setDataSchedules(departureSchedules, arrivalSchedules);
				result->setPath(&route);

				if(team)
				{
					result->setTeam(*team);
				}

				if(rules)
				{
					result->setRules(*rules);
				}

				if(servedVertices)
				{
					SchedulesBasedService::ServedVertices vertices;
					size_t stopRank(0);
					BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& itStop, *servedVertices)
					{
						// Choosing the vertex in the same hub than in the path
						Vertex* vertex(route.getEdge(stopRank)->getFromVertex());
						if(	itStop._stop.find(static_cast<StopPoint*>(vertex)) == itStop._stop.end())
						{
							BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization::StopsSet::value_type& itStopLink, itStop._stop)
							{
								if(vertex->getHub() == itStopLink->getHub())
								{
									vertex = itStopLink;
									break;
								}
							}
						}
						vertices.push_back(vertex);
						++stopRank;
					}
					result->setVertices(vertices);
				}

				route.addService(*result, false);

				// Source links
				Importable::DataSourceLinks links;
				if(id)
				{
					links.insert(make_pair(&source, *id));
				}
				else
				{
					links.insert(make_pair(&source, string()));
				}
				result->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<ScheduledService>().add(boost::shared_ptr<ScheduledService>(result));

				_logCreation(
					"Creation of service "+ result->getServiceNumber() +" for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}
			else
			{
				if (!result->hasLinkWithSource(source))
				{
					// Add source link (service may have been created by another source)
					Importable::DataSourceLinks links;
					if(id)
					{
						links.insert(make_pair(&source, *id));
					}
					else
					{
						links.insert(make_pair(&source, string()));
					}
					result->setDataSourceLinksWithoutRegistration(links);
				}
				_logLoad(
					"Use of service "+ lexical_cast<string>(result->getKey()) +" ("+ result->getServiceNumber() +") for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}

			return result;
		}



		ContinuousService* PTFileFormat::_createOrUpdateContinuousService(
			JourneyPattern& route,
			const SchedulesBasedService::Schedules& departureSchedules,
			const SchedulesBasedService::Schedules& arrivalSchedules,
			const std::string& number,
			const boost::posix_time::time_duration& range,
			const boost::posix_time::time_duration& waitingTime,
			const impex::DataSource& source
		) const {
			// Comparison of the size of schedules and the size of the route
			if(	route.getScheduledStopsNumber() != departureSchedules.size() ||
				route.getScheduledStopsNumber() != arrivalSchedules.size()
			){
				_logWarning(
					"Inconsistent schedules size in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
				return NULL;
			}

			// Checks for schedules validity
			BOOST_FOREACH(const time_duration& td, departureSchedules)
			{
				if(td.is_not_a_date_time())
				{
					_logWarning(
						"At least an undefined time in departure schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}
			BOOST_FOREACH(const time_duration& ta, arrivalSchedules)
			{
				if(ta.is_not_a_date_time())
				{
					_logWarning(
						"At least an undefined time in arrival schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}

			// Search for a corresponding service
			ContinuousService* result(NULL);
			boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
				*route.sharedServicesMutex
			);
			BOOST_FOREACH(Service* tservice, route.getAllServices())
			{
				ContinuousService* curService(dynamic_cast<ContinuousService*>(tservice));

				if(!curService) continue;

				if(	curService->getServiceNumber() == number &&
					curService->comparePlannedSchedules(departureSchedules, arrivalSchedules) &&
					curService->getRange() == range &&
					curService->getMaxWaitingTime() == waitingTime
					){
					result = curService;
					break;
				}
			}

			// If not found creation
			if(!result)
			{
				result = new ContinuousService(
					ContinuousServiceTableSync::getId(),
					number,
					&route,
					range,
					waitingTime
				);
				result->setDataSchedules(departureSchedules, arrivalSchedules);
				result->setPath(&route);
				route.addService(*result, false);
				_env.getEditableRegistry<ContinuousService>().add(boost::shared_ptr<ContinuousService>(result));

				_logCreation(
					"Creation of continuous service "+ result->getServiceNumber() +" for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}
			else
			{
				_logLoad(
					"Use of continuous service "+ lexical_cast<string>(result->getKey()) +" ("+ result->getServiceNumber() +") for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}

			return result;
		}



		void PTFileFormat::_exportStopAreas(
			const ImportableStopAreas& objects
		) const {
			// Loop on source stop areas
			BOOST_FOREACH(const ImportableStopAreas::value_type& object, objects)
			{
				// The parameters map of the object
				boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
				_pm.insert(TAG_STOP_AREA, stopAreaPM);

				// Attributes
				stopAreaPM->insert(ATTR_SOURCE_CODE, object.operatorCode);
				stopAreaPM->insert(ATTR_SOURCE_CITY_NAME, object.cityName);
				stopAreaPM->insert(ATTR_SOURCE_NAME, object.name);

				// Linked stop areas
				BOOST_FOREACH(StopArea* stopArea, object.linkedStopAreas)
				{
					// Submap
					boost::shared_ptr<ParametersMap> linkPM(new ParametersMap);
					stopAreaPM->insert(TAG_LINKED_STOP_AREA, linkPM);
					
					// Export
					stopArea->toParametersMap(*stopAreaPM, true);
				}
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Exports a list of stop points in the import result parameters map.
		/// @param objects the stop points to export
		void PTFileFormat::_exportStopPoints(
			const ImportableStopPoints& objects
		) const {

			BOOST_FOREACH(const ImportableStopPoints::value_type& object, objects)
			{
				// The parameters map of the object
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
				_pm.insert(TAG_STOP_POINT, stopPM);

				// Properties
				stopPM->insert(ATTR_SOURCE_CODE, object.first);
				stopPM->insert(ATTR_SOURCE_CITY_NAME, object.second.cityName);
				stopPM->insert(ATTR_SOURCE_NAME, object.second.name);
				if(object.second.stopArea)
				{
					boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
					object.second.stopArea->toParametersMap(*stopAreaPM, true);
					stopPM->insert(TAG_LINKED_STOP_AREA, stopAreaPM);
				}

				// Lines
				BOOST_FOREACH(const string& line, object.second.lineCodes)
				{
					boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
					linePM->insert(ATTR_SOURCE_CODE, line);
					stopPM->insert(TAG_SOURCE_LINE, linePM);
				}

				// Coordinates
				boost::shared_ptr<geos::geom::Point> projectedPoint;
				if(object.second.coords.get())
				{
					projectedPoint = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*object.second.coords);

					stopPM->insert(ATTR_SOURCE_SYNTHESE_X, projectedPoint->getX());
					stopPM->insert(ATTR_SOURCE_SYNTHESE_Y, projectedPoint->getY());
					stopPM->insert(ATTR_SOURCE_X, object.second.coords->getX());
					stopPM->insert(ATTR_SOURCE_Y, object.second.coords->getY());
				}

				// Linked stop points
				BOOST_FOREACH(StopPoint* stopPoint, object.second.linkedStopPoints)
				{
					// Submap
					boost::shared_ptr<ParametersMap> linkPM(new ParametersMap);
					stopPM->insert(TAG_LINKED_STOP_POINT, linkPM);

					// StopPoint
					stopPoint->toParametersMap(*linkPM, true);

					// Distance between source and synthese stop
					if (stopPoint->getGeometry().get() && object.second.coords.get())
					{
						double distance(
							geos::operation::distance::DistanceOp::distance(*projectedPoint, *stopPoint->getGeometry())
						);
						_pm.insert(ATTR_DISTANCE, distance);
					}
				}
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// The created object is owned by the environment (it is not required to
		/// maintain the returned shared pointer)
		Destination* PTFileFormat::_createOrUpdateDestination(
			impex::ImportableTableSync::ObjectBySource<DestinationTableSync>& destinations,
			const std::string& id,
			const std::string& displayText,
			const std::string& ttsText,
			const impex::DataSource& source
		) const {
			Destination* destination;
			if(destinations.contains(id))
			{
				set<Destination*> loadedDestination(destinations.get(id));
				if(loadedDestination.size() > 1)
				{
					_logWarning(
						"More than one destination with key "+ id
					);
				}
				destination = *loadedDestination.begin();
				_logLoad(
					"Use of existing destination "+ lexical_cast<string>(destination->getKey()) +" ("+ destination->get<DisplayedText>() +")"
				);
			}
			else
			{
				destination = new Destination(
					DestinationTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				destination->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<Destination>().add(boost::shared_ptr<Destination>(destination));
				destinations.add(*destination);
				_logCreation(
					"Creation of the destination with key "+ id +" ("+ displayText +")"
				);
			}
			destination->set<DisplayedText>(displayText);
			destination->set<TtsText>(ttsText);
			return destination;
		}



		CommercialLine* PTFileFormat::_getLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			const impex::DataSource& source,
			optional<TransportNetwork&> network
		) const {
			CommercialLine* line(NULL);
			if(lines.contains(id))
			{
				set<CommercialLine*> loadedLines(lines.get(id));
				// Network
				if(network)
				{
					BOOST_FOREACH(CommercialLine* loadedLine, loadedLines)
					{
						if(loadedLine->getNetwork() == &(*network))
						{
							line = loadedLine;
							break;
						}
					}
				}
				else
				{
					if(loadedLines.size() > 1)
					{
						_logWarning(
							"More than one line with key "+ id
						);
					}
					line = *loadedLines.begin();
				}

				if(line)
				{
					if(line->getPaths().empty())
					{
						JourneyPatternTableSync::Search(_env, line->getKey());
						ScheduledServiceTableSync::Search(_env, optional<RegistryKeyType>(), line->getKey());
						ContinuousServiceTableSync::Search(_env, optional<RegistryKeyType>(), line->getKey());
						BOOST_FOREACH(const Path* route, line->getPaths())
						{
							LineStopTableSync::Search(_env, route->getKey());
						}
					}

					_logLoad(
						"Use of existing commercial line "+ lexical_cast<string>(line->getKey()) +" ("+ line->getName() +")"
					);
				}
			}
			return line;
		}



		std::set<JourneyPattern*> PTFileFormat::_getRoutes(
			pt::CommercialLine& line,
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
			const impex::DataSource& source
		) const {
			// Attempting to find an existing route by value comparison
			set<JourneyPattern*> result;
			BOOST_FOREACH(Path* route, line.getPaths())
			{
				// Avoid junctions
				if(!dynamic_cast<JourneyPattern*>(route))
				{
					continue;
				}

				JourneyPattern* jp(static_cast<JourneyPattern*>(route));

				if(!jp->hasLinkWithSource(source))
				{
					continue;
				}

				if(	*jp == servedStops
				){
					result.insert(jp);
					_logLoad(
						"Use of existing route "+ lexical_cast<string>(jp->getKey()) +" ("+ jp->getName() +")"
					);
				}
			}
			return result;
		}



		RollingStock* PTFileFormat::_getTransportMode(
			const impex::ImportableTableSync::ObjectBySource<RollingStockTableSync>& transportModes,
			const std::string& id
		) const {
			RollingStock* transportMode(NULL);
			if(transportModes.contains(id))
			{
				set<RollingStock*> loadedTransportModes(transportModes.get(id));
				if(loadedTransportModes.size() > 1)
				{
					_logWarning("more than one transport mode with key "+ id);
				}
				transportMode = *loadedTransportModes.begin();
			}
			return transportMode;
		}
}	}

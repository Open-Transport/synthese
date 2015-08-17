
/** OGTFileFormat class implementation.
	@file OGTFileFormat.cpp

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

#include "OGTFileFormat.hpp"

#include "Import.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "ScheduledServiceTableSync.h"
#include "CalendarTemplateTableSync.h"

#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace calendar;
	using namespace db;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, OGTFileFormat>::FACTORY_KEY("OGT");
	}

	namespace data_exchange
	{
		bool OGTFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {

			ExpatParser parser(*_import.get<DataSource>(), *this);
			// Open file to stream
			ifstream inFile(filePath.string().c_str());
			if(!inFile)
			{
				_logError(
					"Could not open the file " + filePath.string()
				);
				throw Exception("Could not open the file " + filePath.string());
			}
			parser.parse(inFile);
			return true;
		}



		db::DBTransaction OGTFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			_addRemoveQueries(transaction);
			return transaction;
		}



		OGTFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<OGTFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		void OGTFileFormat::Importer_::ExpatParser::startElement(
			void *d,
			const XML_Char* name,
			const XML_Char** attrs
		) throw(Exception) {

			expat_user_data *user_data = (expat_user_data*) d;

			AttributeMap attributes = ExpatParser::createAttributeMap(attrs);
			string tag(name);

			if(tag == "SCHEDULE")
			{
				string lineCode(attributes["APPLICATION_AREA"]);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines(
					user_data->_dataSource,
					user_data->_importer._env
				);
				impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Set lines(
					_lines.get(lineCode)
				);
				if(!lines.empty())
				{
					user_data->line = *lines.begin();
				}
			}
			else if(user_data->line)
			{
				if(tag == "SCHEDULE_PART")
				{
					user_data->wayBack = (attributes["DIRECTION"] == "LEFT");
				}
				else if(tag == "TRIP")
				{
					if(attributes["MISSION_TYPE"] != "Passager")
					{
						user_data->importTrip = false;
					}
					else
					{
						user_data->importTrip = true;
						user_data->tripNumber = attributes["JOURNEY_DESC"];
						user_data->arrivalSchedule = duration_from_string(attributes["ENTRY_TIME"]);
						user_data->arrivalSchedules.push_back(user_data->arrivalSchedule);
					}
				}
				else if(user_data->importTrip)
				{
					if(	tag == "STOP")
					{
						if(attributes["SITUATION"] == "REVENUE_SERVICE")
						{
							std::set<StopPoint*> stops(
								user_data->_importer._getStopPoints(
									user_data->stopPoints,
									attributes["TOP"],
									boost::optional<const std::string&>(),
									true
							)	);
							if(!user_data->tripStops.empty())
							{
								user_data->arrivalSchedules.push_back(
									user_data->arrivalSchedule.seconds() ?
									user_data->arrivalSchedule + seconds(60 - user_data->arrivalSchedule.seconds()) :
									user_data->arrivalSchedule
								);
							}
							user_data->tripStops.push_back(
								JourneyPattern::StopWithDepartureArrivalAuthorization(
									stops
							)	);
						}
						user_data->arrivalSchedule += seconds(lexical_cast<long>(attributes["DWELLTIME"]));
						if(attributes["SITUATION"] == "REVENUE_SERVICE")
						{
							user_data->departureSchedules.push_back(user_data->arrivalSchedule - seconds(user_data->arrivalSchedule.seconds()));
						}
					}
					else if(tag == "RUN")
					{
						user_data->arrivalSchedule += seconds(lexical_cast<long>(attributes["RUNTIME"]));
					}
				}
		}	}



		void OGTFileFormat::Importer_::ExpatParser::endElement(
			void *d,
			const XML_Char* name
		) throw(Exception) {
			expat_user_data *user_data = (expat_user_data*)d;
			string tag(name);

			if(tag == "SCHEDULE")
			{
				user_data->line = NULL;
			}

			if(tag == "TRIP")
			{
				if(user_data->importTrip && user_data->line && !user_data->tripStops.empty())
				{
					user_data->tripStops.begin()->_arrival = false;
					user_data->tripStops.rbegin()->_departure = false;

					JourneyPattern* route(
						user_data->_importer._createOrUpdateRoute(
							*user_data->line,
							boost::optional<const std::string&>(),
							boost::optional<const std::string&>(),
							boost::optional<const std::string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							user_data->wayBack,
							NULL, // (rolling stock)
							user_data->tripStops,
							user_data->_dataSource,
							true,
							true
					)	);
					ScheduledService* service(
						user_data->_importer._createOrUpdateService(
							*route,
							user_data->departureSchedules,
							user_data->arrivalSchedules,
							user_data->tripNumber,
							user_data->_dataSource
					)	);
					if(service)
					{
						*service |= user_data->_importer._calendar;
						service->addCodeBySource(user_data->_dataSource, user_data->tripNumber);
					}
				}
				user_data->importTrip = false;
				user_data->tripStops.clear();
				user_data->departureSchedules.clear();
				user_data->arrivalSchedules.clear();
				user_data->tripNumber.clear();
			}
		}



		OGTFileFormat::Importer_::ExpatParser::ExpatParser(
			const impex::DataSource& dataSource,
			const Importer_& importer
		):	user_data(
				dataSource,
				importer
			)
		{}



		OGTFileFormat::Importer_::ExpatParser::AttributeMap OGTFileFormat::Importer_::ExpatParser::createAttributeMap( const XML_Char **attrs )
		{
			int count = 0;
			AttributeMap attributes;
			while(attrs[count])
			{
				attributes[attrs[count]] = attrs[count+1];
				count += 2;
			}
			return attributes;
		}



		void OGTFileFormat::Importer_::ExpatParser::parse( std::istream &data ) throw(std::runtime_error)
		{
			int done, count = 0, n;
			char buf[4096];

			XML_Parser p = XML_ParserCreate(NULL);
			if (!p) {
				throw std::runtime_error("error creating expat parser");
			}


			XML_SetElementHandler(p, ExpatParser::startElement,ExpatParser::endElement);
			XML_SetCharacterDataHandler(p, ExpatParser::characters);

			user_data.importTrip = false;
			XML_SetUserData(p,&user_data);
			// straight from example
			do {
				data.read(buf, 4096);
				n = data.gcount();
				done = (n != 4096);
				if (XML_Parse(p, buf, n, done) == XML_STATUS_ERROR) {
					XML_Error errorCode = XML_GetErrorCode(p);
					int errorLine = XML_GetCurrentLineNumber(p);
					long errorCol = XML_GetCurrentColumnNumber(p);
					const XML_LChar *errorString = XML_ErrorString(errorCode);
					std::stringstream errorDesc;
					errorDesc << "XML parsing error at line " << errorLine << ":"
						<< errorCol;
					errorDesc << ": " << errorString;
					throw std::runtime_error(errorDesc.str());
				}
				count += n;
			} while (!done);

			XML_ParserFree(p);
		}



		OGTFileFormat::Importer_::ExpatParser::~ExpatParser()
		{

		}



		void OGTFileFormat::Importer_::ExpatParser::characters( void*, const XML_Char* txt, int txtlen )
		{

		}



		OGTFileFormat::Importer_::ExpatParser::expat_user_data::expat_user_data(
			const impex::DataSource& dataSource,
			const Importer_& importer
		):	_dataSource(dataSource),
			_importer(importer),
			line(NULL),
			wayBack(false),
			stopPoints(dataSource, importer._env)
		{
		}



		util::ParametersMap OGTFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			return map;
		}



		void OGTFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
		}
}	}

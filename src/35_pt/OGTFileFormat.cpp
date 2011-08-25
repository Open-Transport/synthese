
/** OGTFileFormat class implementation.
	@file OGTFileFormat.cpp

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

#include "OGTFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "ScheduledServiceTableSync.h"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "PropertiesHTMLTable.h"
#include "CalendarTemplateTableSync.h"

#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace impex;
	using namespace calendar;
	using namespace db;
	using namespace util;
	using namespace admin;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::OGTFileFormat>::FACTORY_KEY("OGT");
	}

	namespace pt
	{
		bool OGTFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {

			ExpatParser parser(_dataSource, _env, os, _calendar);
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			parser.parse(inFile);
			return true;
		}



		db::DBTransaction OGTFileFormat::Importer_::_save() const
		{
			_selectObjectsToRemove();
			DBTransaction transaction;
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
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


	
		OGTFileFormat::Importer_::Importer_( util::Env& env, const impex::DataSource& dataSource ):
			OneFileTypeImporter<OGTFileFormat>(env, dataSource),
			Importer(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource)
		{}



		void OGTFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const {
			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichier");
			stream << t.cell("Fichier", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, _cleanOldData));
			stream << t.cell("Ne pas importer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, _fromToday));
			stream << t.cell("Calendrier", 
				t.getForm().getSelectInput(
					PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList(),
					optional<RegistryKeyType>(_calendarTemplate.get() ? _calendarTemplate->getKey() : RegistryKeyType(0))
			)	);
			stream << t.close();
		}



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
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines(user_data->_dataSource, user_data->_env);
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
						user_data->tripNumber = attributes["NUMBER"];
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
								PTFileFormat::GetStopPoints(
									user_data->stopPoints,
									attributes["TOP"],
									boost::optional<const std::string&>(),
									user_data->_stream,
									true
							)	);
							if(!user_data->tripStops.empty())
							{
								user_data->arrivalSchedules.push_back(user_data->arrivalSchedule);
							}
							user_data->tripStops.push_back(
								JourneyPattern::StopWithDepartureArrivalAuthorization(
									stops
							)	);
						}
						user_data->arrivalSchedule += seconds(lexical_cast<long>(attributes["DWELLTIME"]));
						if(attributes["SITUATION"] == "REVENUE_SERVICE")
						{
							user_data->departureSchedules.push_back(user_data->arrivalSchedule);
						}
					}
					else if(tag == "RUN")
					{
						user_data->arrivalSchedule += seconds(lexical_cast<long>(attributes["RUNTIME"]));
					}
				}
		}	}



		void OGTFileFormat::Importer_::ExpatParser::endElement( void *d, const XML_Char* name ) throw(Exception)
		{
			expat_user_data *user_data = (expat_user_data*)d;
			string tag(name);

			if(tag == "SCHEDULE")
			{
				user_data->line = NULL;
			}

			if(tag == "TRIP")
			{
				if(user_data->importTrip && user_data->line)
				{
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							*user_data->line,
							boost::optional<const std::string&>(),
							user_data->tripNumber,
							boost::optional<const std::string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							user_data->wayBack,
							NULL, // (rolling stock)
							user_data->tripStops,
							user_data->_dataSource,
							user_data->_env,
							user_data->_stream
					)	);
					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							user_data->departureSchedules,
							user_data->arrivalSchedules,
							user_data->tripNumber,
							user_data->_dataSource,
							user_data->_env,
							user_data->_stream
					)	);
					if(service)
					{
						*service |= user_data->_calendar;
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
			util::Env& env,
			ostream& stream,
			const Calendar& calendar
		):	user_data(
				dataSource,
				env,
				stream,
				calendar
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

			//user_data.curNode=NULL;
			//user_data.curWay=NULL;
			//user_data.curRelation=NULL;
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
			util::Env& env,
			ostream& stream,
			const Calendar& calendar
		):	_dataSource(dataSource),
			_env(env),
			_stream(stream),
			_calendar(calendar),
			stopPoints(dataSource, env),
			line(NULL),
			wayBack(false)
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

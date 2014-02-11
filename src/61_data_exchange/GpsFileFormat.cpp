#include "GpsFileFormat.h"

#include "DataSource.h"
#include "DBModule.h"
#include "GraphConstants.h"
#include "Import.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "CityTableSync.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContactTableSync.h"
#include "PTUseRule.h"
#include "PTConstants.h"
#include "CoordinatesSystem.hpp"
#include "XmlToolkit.h"
#include "DBTransaction.hpp"
#include "CityAliasTableSync.hpp"
#include "JunctionTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "RequestException.h"
#include "ImpExModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "CalendarTemplateElementTableSync.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <utility>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace geography;
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace pt;
	using namespace server;
	using namespace calendar;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, GpsFileFormat>::FACTORY_KEY("GpsJson");
	}

	namespace data_exchange
	{
		//const string GpsFileFormat::Importer_::PARAMETER_IMPORT_STOPS("impstp");
		//const string GpsFileFormat::Importer_::PARAMETER_IMPORT_JUNCTIONS("impjun");
		//const string GpsFileFormat::Importer_::PARAMETER_DEFAULT_TRANSFER_DURATION("dtd");
		//const string GpsFileFormat::Importer_::PARAMETER_AUTOGENERATE_STOP_AREAS("asa");
		//const string GpsFileFormat::Importer_::PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY("sasp");
		//const string GpsFileFormat::Importer_::PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES("itt");
		//const string GpsFileFormat::Importer_::PARAMETER_MERGE_ROUTES("mr");
		//const string GpsFileFormat::Exporter_::PARAMETER_LINE_ID("li");
		//const string GpsFileFormat::Exporter_::PARAMETER_WITH_TISSEO_EXTENSION("wt");
		//const string GpsFileFormat::Exporter_::PARAMETER_WITH_OLD_DATES("wod");

		GpsFileFormat::SRIDConversionMap GpsFileFormat::_SRIDConversionMap;

		//////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR
		GpsFileFormat::Importer_::Importer_(
			Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<Importer_>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_importStops(false),
			_autoGenerateStopAreas(false),
			_importJunctions(false),
			_mergeRoutes(true),
			_defaultTransferDuration(minutes(8)),
			_importTimetablesAsTemplates(false),
			_calendarTemplates(*import.get<DataSource>(), env),
			_stopAreas(*import.get<DataSource>(), env),
			_stops(*import.get<DataSource>(), env),
			_networks(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		//////////////////////////////////////////////////////////////////////////
		// REQUESTS HANDLING
		util::ParametersMap GpsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result(PTDataCleanerFileFormat::_getParametersMap());
			//result.insert(PARAMETER_IMPORT_STOPS, _importStops);
			//result.insert(PARAMETER_IMPORT_JUNCTIONS, _importJunctions);
			//result.insert(PARAMETER_AUTOGENERATE_STOP_AREAS, _autoGenerateStopAreas);
			//if(!_defaultTransferDuration.is_not_a_date_time())
			//{
			//	result.insert(PARAMETER_DEFAULT_TRANSFER_DURATION, _defaultTransferDuration.total_seconds() / 60);
			//}
			//result.insert(PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY, _treatAllStopAreaAsQuay);
			//result.insert(PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES, _importTimetablesAsTemplates);
			//result.insert(PARAMETER_MERGE_ROUTES, _mergeRoutes);
			return result;
		}



		util::ParametersMap GpsFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;
			//if(_startDate < day_clock::local_day())
			//{
			//	date_duration du(day_clock::local_day() - _startDate);
			//	result.insert(PARAMETER_WITH_OLD_DATES, static_cast<int>(du.days()));
			//}
			//if(_line.get())
			//{
			//	result.insert(PARAMETER_LINE_ID, _line->getKey());
			//}
			//result.insert(PARAMETER_WITH_TISSEO_EXTENSION, _withTisseoExtension);
			return result;
		}



		void GpsFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			//PTDataCleanerFileFormat::_setFromParametersMap(map);
			//_importStops = map.getDefault<bool>(PARAMETER_IMPORT_STOPS, false);
			//_autoGenerateStopAreas = map.getDefault<bool>(PARAMETER_AUTOGENERATE_STOP_AREAS, false);
			//_importJunctions = map.getDefault<bool>(PARAMETER_IMPORT_JUNCTIONS, false);
			//if(map.getDefault<int>(PARAMETER_DEFAULT_TRANSFER_DURATION, 0))
			//{
			//	_defaultTransferDuration = minutes(map.get<int>(PARAMETER_DEFAULT_TRANSFER_DURATION));
			//}
			//_treatAllStopAreaAsQuay = map.getDefault<bool>(PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY, false);
			//_importTimetablesAsTemplates = map.getDefault<bool>(PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES, false);
			//_mergeRoutes = map.getDefault<bool>(PARAMETER_MERGE_ROUTES, true);
		}



		void GpsFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{
			//_startDate = day_clock::local_day();
			//_startDate -= days(map.getDefault<int>(PARAMETER_WITH_OLD_DATES, 0));

			//RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
			//if (id == 0)
			//	throw RequestException("JourneyPattern id must be specified");

			//try
			//{
			//	_line = CommercialLineTableSync::Get(id, _env);
			//}
			//catch (...)
			//{
			//	throw RequestException("No such line");
			//}

			//_withTisseoExtension = map.getDefault<bool>(PARAMETER_WITH_TISSEO_EXTENSION, false);
		}


		//////////////////////////////////////////////////////////////////////////
		// OUTPUT

		void GpsFileFormat::Exporter_::build(
			ostream& os
		) const {
			os << "NOT IMPLEMENTED" << std::endl;
		}


		//////////////////////////////////////////////////////////////////////////
		// INPUT

		bool GpsFileFormat::Importer_::_parse(
			const path& filePath
		) const {
			bool failure(false);
			return !failure;
		}


		DBTransaction GpsFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			// Add remove queries generated by _selectObjectsToRemove
			_addRemoveQueries(transaction);
			return transaction;
		}


		//////////////////////////////////////////////////////////////////////////
		// HELPERS

		std::string GpsFileFormat::Importer_::GetCoordinate( const double value )
		{
			return
				(value > 0) ?
				lexical_cast<string>(value) :
				string();
		}
	}
}

#pragma once


#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"

#include "CoordinatesSystem.hpp"
#include "ImportableTableSync.hpp"
#include "CalendarTemplateTableSync.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "JourneyPattern.hpp"

#include <iostream>
#include <vector>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/bimap.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplateElement;
	}

	namespace util
	{
		class Registrable;
	}

	namespace pt
	{
		class CommercialLine;
	}

	namespace data_exchange
	{
		class GpsFileFormat:
			public impex::FileFormatTemplate<GpsFileFormat>
		{
		private:
			typedef boost::bimap<CoordinatesSystem::SRID, std::string> SRIDConversionMap;
			static SRIDConversionMap _SRIDConversionMap;

			class UnknkownSRIDException:
				public synthese::Exception
			{
			public:
				UnknkownSRIDException():
				synthese::Exception("The specified SRID is not convertible into/from Trident Keyword")
				{}
			};

			static void _populateSRIDTridentConversionMap();
			static CoordinatesSystem::SRID _getSRIDFromTrident(const std::string& value);
			static const std::string& _getTridentFromSRID(const CoordinatesSystem::SRID value);

		public:

			class Importer_:
				public impex::OneFileTypeImporter<Importer_>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
			{
			private:
				//! @name Import parameters
				//@{
					bool		_importStops;
					bool		_autoGenerateStopAreas;
					bool		_importJunctions;
					bool		_mergeRoutes;
					boost::posix_time::time_duration	_defaultTransferDuration;
					bool		_treatAllStopAreaAsQuay;
					bool		_importTimetablesAsTemplates;
				//@}

				struct Route
				{
					pt::JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					std::string objectId;
					std::string name;
					bool wayBack;
					pt::JourneyPattern* journeyPattern;

					Route():
					wayBack(false),
					journeyPattern(NULL)
					{}
				};

				mutable std::set<boost::shared_ptr<calendar::CalendarTemplateElement> > _calendarElementsToRemove;
				mutable impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync> _calendarTemplates;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync> _stopAreas;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stops;
				mutable impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync> _networks;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;

				static std::string GetCoordinate(const double value);


			public:
				//static const std::string PARAMETER_IMPORT_STOPS;
				//static const std::string PARAMETER_IMPORT_JUNCTIONS;
				//static const std::string PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES;
				//static const std::string PARAMETER_DEFAULT_TRANSFER_DURATION;
				//static const std::string PARAMETER_AUTOGENERATE_STOP_AREAS;
				//static const std::string PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY;
				//static const std::string PARAMETER_MERGE_ROUTES;

				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);

	
				virtual util::ParametersMap _getParametersMap() const;
				virtual void _setFromParametersMap(const util::ParametersMap& map);
				virtual bool _parse(const boost::filesystem::path& filePath) const;

				virtual db::DBTransaction _save() const;
			};

			class Exporter_:
				public impex::OneFileExporter<GpsFileFormat>
			{
			//public:
			//	static const std::string PARAMETER_LINE_ID;	//!< Commercial line id parameter
			//	static const std::string PARAMETER_WITH_TISSEO_EXTENSION;	//!< With tisseo extension parameter
			//	static const std::string PARAMETER_WITH_OLD_DATES;

			private:
				//! @name Export parameters
				//@{
					boost::gregorian::date	_startDate;
					boost::shared_ptr<const pt::CommercialLine> _line; //!< Commercial line to export
					bool										_withTisseoExtension;
				//@}


			public:
				Exporter_():
					_withTisseoExtension(false)
				{}

				//! @name Setters
				//@{
					void setLine(boost::shared_ptr<const pt::CommercialLine> value){ _line = value; }
					void setWithTisseoExtension(bool value){ _withTisseoExtension = value; }
				//@}

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				/** -> ChouettePTNetwork
				 */
				virtual void build(std::ostream& os) const;

				virtual std::string getOutputMimeType() const { return "text/xml"; }
			};

			friend class Importer_;
			friend class Exporter_;
		};
	}
}

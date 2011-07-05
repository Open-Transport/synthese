
/** tridentexport class header.
	@file tridentexport.h

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


#ifndef SYNTHESE_IMPEX_TRIDENTEXPORT_H
#define SYNTHESE_IMPEX_TRIDENTEXPORT_H

#include "FileFormatTemplate.h"
#include "CoordinatesSystem.hpp"
#include "OneFileTypeImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTDataCleanerFileFormat.hpp"

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

		//////////////////////////////////////////////////////////////////////////
		/// Trident/Chouette file format.
		/// @ingroup m35File refFile
		///
		/// @todo Remove all running days of services of the datasource before Trident import.
		///
		///	<h3>The Trident file format</h3>
		///
		/// The Trident format is the French standard for public transport theoretical data exchange, based upon <a href="http://www.transmodel.org/fr/cadre1.html">Transmodel</a>.
		///
		/// The Trident format is defined by several XSD schemes :
		///	<ul>
		///		<li><a href="include/35_pt/chouette/Chouette.xsd">Chouette.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Global_schema.xsd">trident_Global_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Location_schema.xsd">trident_Location_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_PT_schema.xsd">trident_PT_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/chouette.zip">ZIP archive containing all Trident XSD files</a></li>
		/// </ul>
		///
		/// The Trident file format has been extended by <a href="http://www.tisseo.fr/">Tisséo</a>, the local public transportation authority of Toulouse, France. See details @ref trident2tisseo "below".
		///
		/// In most cases, the Trident objects correspond exactly to the SYNTHESE objects :
		///	<ul>
		///		<li>ConnectionLink = Junction</li>
		///	</ul>
		///
		/// <h3>Usage</h3>
		///
		/// Import parameters :
		///		- impstp (PARAMETER_IMPORT_STOPS / _importStops) : If true, the stops are imported from the Trident file. If false, the import function assumes that all stops linked in the Trident file exist already in the database.
		///		- impjun (PARAMETER_IMPORT_JUNCTIONS / _importJunctions) : If true, the junctions are imported from the Trident file. If false, they are ignored.
		///		- wod (PARAMETER_WITH_OLD_DATES / _startDate) : Number of past days to import (default 0).
		///		- dtd (PARAMETER_DEFAULT_TRANSFER_DURATION / _defaultTransferDuration) : Number of minutes for default transfer delay in case of connection place creation (default = 8).
		///
		/// Export parameters :
		///		- roid : id of the CommercialLine object to export
		///		- wte : If true, the generated file will contain "Tisséo Trident Extension". If false, the generated file will respect the pure Trident format.
		///		- wod (PARAMETER_WITH_OLD_DATES / _startDate) : Number of past days to export (default 0).
		///
		/// For the details about what is imported or exported, see the documentation of @ref TridentFileFormat::_parse
		///
		class TridentFileFormat:
			public impex::FileFormatTemplate<TridentFileFormat>
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

			/// @throws UnknownSRIDException
			static CoordinatesSystem::SRID _getSRIDFromTrident(const std::string& value);

			/// @throws UnknownSRIDException
			static const std::string& _getTridentFromSRID(const CoordinatesSystem::SRID value);

		public:

			class Importer_:
				public impex::OneFileTypeImporter<Importer_>,
				public PTDataCleanerFileFormat
			{
			private:
				//! @name Import parameters
				//@{
					bool		_importStops;
					bool		_autoGenerateStopAreas;
					bool		_importJunctions;
					boost::posix_time::time_duration	_defaultTransferDuration;
					boost::gregorian::date	_startDate;
					bool		_treatAllStopAreaAsQuay;
					bool		_importTimetablesAsTemplates;
				//@}

				mutable std::set<boost::shared_ptr<calendar::CalendarTemplateElement> > _calendarElementsToRemove;

				static std::string GetCoordinate(const double value);


			public:
				static const std::string PARAMETER_IMPORT_STOPS;
				static const std::string PARAMETER_IMPORT_JUNCTIONS;
				static const std::string PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES;
				static const std::string PARAMETER_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_WITH_OLD_DATES;
				static const std::string PARAMETER_AUTOGENERATE_STOP_AREAS;
				static const std::string PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY;

				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);


				//! @name Setters
				//@{
					void setImportStops(bool value);
				//@}

				//! @name Getters
				//@{
					bool getImportStops() const;
				//@}


				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual util::ParametersMap _getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// Trident file import.
				/// @param filePath path of the file to import
				/// @param os stream to write information messages on
				/// @author Hugues Romain
				///
				/// The Trident file import loads the following objects :
				///	<ul>
				///		<li>Commercial stop points (StopArea) : city, name, specific transfer delays (only if stop import mode)</li>
				///		<li>Physical stop points (StopPoint) : commercial stop point (only at physical stop creation), x, y, name (only if stop import mode)</li>
				///		<li>Scheduled services (ScheduledService) : all. Not imported if the service runs never.
				/// </ul>
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

				virtual db::DBTransaction _save() const;



				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;

			};

			class Exporter_:
				public impex::OneFileExporter<TridentFileFormat>
			{
			public:
				static const std::string PARAMETER_LINE_ID;	//!< Commercial line id parameter
				static const std::string PARAMETER_WITH_TISSEO_EXTENSION;	//!< With tisseo extension parameter
				static const std::string PARAMETER_WITH_OLD_DATES;

			private:
				//! @name Export parameters
				//@{
					boost::gregorian::date	_startDate;
					boost::shared_ptr<const pt::CommercialLine> _line; //!< Commercial line to export
					bool										_withTisseoExtension;
				//@}

				static std::string TridentId (const std::string& peer, const std::string clazz, const util::RegistryKeyType& id);
				static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
				static std::string TridentId (const std::string& peer, const std::string clazz, const util::Registrable& obj);

			public:
				Exporter_():
					_withTisseoExtension(false)
				{}

				//! @name Setters
				//@{
					void setLine(boost::shared_ptr<const CommercialLine> value){ _line = value; }
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

#endif

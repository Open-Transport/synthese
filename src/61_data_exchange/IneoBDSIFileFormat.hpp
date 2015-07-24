
//////////////////////////////////////////////////////////////////////////
/// Ineo BDSI file format class header.
///	@file IneoBDSIFileFormat.hpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_IneoBDSIFileFormat_H__
#define SYNTHESE_IneoBDSIFileFormat_H__

#include "DatabaseReadImporter.hpp"
#include "FactorableTemplate.h"
#include "FileFormatTemplate.h"
#include "NoExportPolicy.hpp"

#include "VehicleTableSync.hpp"
#include "VehiclePositionTableSync.hpp"

#include "SchedulesBasedService.h"

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace departure_boards
	{
		class DisplayScreen;
	}

	namespace pt_operation
	{
		class Depot;
	}

	namespace pt
	{
		class StopPoint;
		class CommercialLine;
		class JourneyPattern;
		class ScheduledService;
	}

	namespace messages
	{
		class MessagesSection;
	}

	namespace graph
	{
		class RuleUser;
	}

	namespace vehicle
	{
		class Vehicle;
		class VehiclePosition;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// 61.15 Action : IneoBDSIFileFormat.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.2-tl
		//////////////////////////////////////////////////////////////////////////
		/// Key : IneoBDSIFileFormat
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class IneoBDSIFileFormat:
			public impex::FileFormatTemplate<IneoBDSIFileFormat>
		{
		public:

			class Importer_:
				public impex::DatabaseReadImporter<IneoBDSIFileFormat>
			{
			public:
				static const std::string PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID;
				static const std::string PARAMETER_PLANNED_DATASOURCE_ID;
				static const std::string PARAMETER_HYSTERESIS;
				static const std::string PARAMETER_DELAY_BUS_STOP;
				static const std::string PARAMETER_DAY_BREAK_TIME;
				static const std::string PARAMETER_MESSAGES_SECTION;
				static const std::string PARAMETER_HANDICAPPED_FORBIDDEN_USE_RULE;
				static const std::string PARAMETER_HANDICAPPED_ALLOWED_USE_RULE;
				static const std::string PARAMETER_NEUTRALIZED;
				static const std::string PARAMETER_NON_COMMERCIAL;
				static const std::string PARAMETER_CLEAN_UNUSED_VEHICLES;

				static const std::map<std::string, vehicle::VehiclePosition::Status> vehiclePositionStatusBdsi2Synthese;

			private:
				boost::shared_ptr<const impex::DataSource> _plannedDataSource;
				boost::shared_ptr<const impex::DataSource> _messagesRecipientsDataSource;
				boost::shared_ptr<const messages::MessagesSection> _messagesSection;
				boost::posix_time::time_duration _hysteresis;
				boost::posix_time::time_duration _delay_bus_stop;
				boost::posix_time::time_duration _dayBreakTime;
				util::RegistryKeyType _handicappedForbiddenPTUseRuleId;
				util::RegistryKeyType _handicappedPTAllowedUseRuleId;
				bool _neutralized;
				bool _nonCommercial;
				bool _cleanUnusedVehicles;

				mutable std::set<util::RegistryKeyType> _scenariosToRemove;
				mutable std::set<util::RegistryKeyType> _alarmObjectLinksToRemove;
				mutable std::set<util::RegistryKeyType> _messagesToRemove;
				mutable std::set<pt::ScheduledService*> _servicesToSave;

				mutable std::set<util::RegistryKeyType> _vehiclesToRemove;
				mutable std::set<boost::shared_ptr<vehicle::VehiclePosition> > _vehiclePositionsToRemove;
				mutable std::set<boost::shared_ptr<vehicle::VehiclePosition> > _vehiclePositions;

				mutable impex::ImportableTableSync::ObjectBySource<vehicle::VehicleTableSync> _vehicles;

				struct VehiculePosition
				{
					std::string code;
					std::string name;
					std::string number;
					bool available;
					std::string type;
					boost::shared_ptr<geos::geom::Point> point;

					vehicle::VehiclePosition::Status status;
					vehicle::VehiclePosition::Meters position;
					std::size_t stopRank;
					boost::shared_ptr<pt::ScheduledService> service;
				};
				typedef std::map<std::string, VehiculePosition> VehiculePositions;

				struct Arret
				{
					std::string ref;
					std::string nom;

					pt::StopPoint* syntheseStop;
					pt_operation::Depot* syntheseDepot;
				};
				typedef std::map<std::string, Arret> Arrets;

				struct Ligne
				{
					std::string ref;

					pt::CommercialLine* syntheseLine;
				};
				typedef std::map<std::string, Ligne> Lignes;

				struct ArretChn
				{
					std::string ref;
					Arret* arret;
					int pos;
					std::string type;
				};

				struct Chainage
				{
					std::string ref;
					std::string nom;
					std::string direction;
					const Ligne* ligne;
					bool sens;
					typedef std::vector<ArretChn> ArretChns;
					ArretChns arretChns;

					typedef std::vector<const pt::JourneyPattern*> SYNTHESEJourneyPatterns;
					mutable SYNTHESEJourneyPatterns syntheseJourneyPatterns;

					const SYNTHESEJourneyPatterns& getSYNTHESEJourneyPatterns(
						const impex::DataSource& theoreticalDataSource,
						const impex::DataSource& realTimeDataSource,
						util::Env& temporaryEnvironment
					) const;
				};
				typedef std::map<std::string, Chainage> Chainages;
				void _selectAndLoadChainage(
					Chainages& chainages,
					const Chainage::ArretChns& arretchns,
					const Ligne& ligne,
					const std::string& nom,
					const std::string& direction,
					bool sens,
					const std::string& chainageRef
				) const;
				Chainage* _createAndReturnChainage(
					Chainages& chainages,
					const Chainage::ArretChns& arretchns,
					const Ligne& ligne,
					const std::string& nom,
					const std::string& direction,
					bool sens,
					const std::string& chainageRef
				) const;



				struct Horaire
				{
					boost::posix_time::time_duration htd;
					boost::posix_time::time_duration hta;
					boost::posix_time::time_duration hrd;
					boost::posix_time::time_duration hra;
					std::string arretchn;
				};


				struct Course
				{
					typedef std::vector<Horaire> Horaires;

					std::string ref;
					const Chainage* chainage;
					Horaires horaires;
					util::RegistryKeyType handicapped;

					mutable pt::ScheduledService* syntheseService;

					bool operator!=(const pt::ScheduledService& op) const;
					bool operator==(const pt::ScheduledService& service) const;
					bool operator==(const pt::SchedulesBasedService::Schedules& schedules) const;

					void createService(
						const boost::gregorian::date& today,
						util::Env& temporaryEnvironment
					) const;

					typedef boost::optional<std::pair<boost::posix_time::time_duration, boost::posix_time::time_duration> > UpdateDeltas;
					UpdateDeltas updateService(
						const boost::posix_time::time_duration& hysteresis
					) const;
				};
				typedef std::map<std::string, Course> Courses;
				void _selectAndLoadCourse(Courses& courses,
					const Course::Horaires& horaires,
					const Chainage& chainage,
					const std::string& courseRef,
					const boost::posix_time::time_duration& nowDuration,
					const util::RegistryKeyType &handicapped,
					Chainages& chainages
				) const;



				struct Destinataire
				{
					std::string destinataire;

					departure_boards::DisplayScreen* syntheseDisplayBoard;
				};

				struct Programmation
				{
					int ref;
					std::string content;
					std::string messageTitle;
					std::string title;
					boost::posix_time::ptime startTime;
					boost::posix_time::ptime endTime;
					bool active;
					bool priority;

					typedef std::vector<Destinataire> Destinataires;
					Destinataires destinataires;
				};
				typedef std::map<int, Programmation> Programmations;


				void _logLoadDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logWarningDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logDebugDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logTraceDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;
				const boost::posix_time::time_duration& getHysteresis() const { return _hysteresis; }

				std::set<vehicle::Vehicle*> _getVehicles(
					const impex::ImportableTableSync::ObjectBySource<vehicle::VehicleTableSync>& vehicles,
					const std::string& id,
					boost::optional<const std::string&> name,
					bool errorIfNotFound
					) const;

				std::set<vehicle::Vehicle*> _createOrUpdateVehicle(
					impex::ImportableTableSync::ObjectBySource<vehicle::VehicleTableSync>& vehicles,
					const std::string& code,
					boost::optional<const std::string&> name,
					boost::optional<const std::string&> number,
					bool available,
					const impex::DataSource& source
					) const;

				vehicle::Vehicle* _createVehicle(
					impex::ImportableTableSync::ObjectBySource<vehicle::VehicleTableSync>& vehicles,
					const std::string& code,
					boost::optional<const std::string&> name,
					const impex::DataSource& source
					) const;

				std::set<boost::shared_ptr<vehicle::VehiclePosition> > _getVehiclePositions(
					boost::optional<util::RegistryKeyType> vehicleId,
					bool errorIfNotFound
					) const;

				std::set<boost::shared_ptr<vehicle::VehiclePosition> > _createOrUpdateVehiclePosition(
					const std::string& code,
					vehicle::Vehicle* vehicle,
					boost::optional<vehicle::VehiclePosition::Status> status,
					boost::optional<vehicle::VehiclePosition::Meters> meterOffset,
					boost::optional<std::size_t> rankInPath,
					boost::optional<boost::shared_ptr<pt::ScheduledService> > service,
					boost::optional<boost::shared_ptr<geos::geom::Point> > point
					) const;

				boost::shared_ptr<vehicle::VehiclePosition> _createVehiclePosition(
					const std::string& code,
					vehicle::Vehicle* vehicle,
					boost::optional<vehicle::VehiclePosition::Status> status,
					boost::optional<vehicle::VehiclePosition::Meters> meterOffset,
					boost::optional<std::size_t> rankInPath,
					boost::optional<boost::shared_ptr<pt::ScheduledService> > service,
					boost::optional<boost::shared_ptr<geos::geom::Point> > point
					) const;

				vehicle::VehiclePosition::Status toVehiclePositionStatus(
					const std::string& etat) const;

			protected:
				//////////////////////////////////////////////////////////////////////////
				/// Generates a generic parameters map from the action parameters.
				/// @return The generated parameters map
				util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads the parameters of the action on a generic parameters map.
				/// @param map Parameters map to interpret
				/// @exception ActionException Occurs when some parameters are missing or incorrect.
				void _setFromParametersMap(const util::ParametersMap& map);

			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);


				virtual db::DBTransaction _save() const;


				//////////////////////////////////////////////////////////////////////////
				/// The action execution code.
				virtual bool _read(
				) const;
			};


			typedef impex::NoExportPolicy<IneoBDSIFileFormat> Exporter_;
		};
}	}

#endif // SYNTHESE_IneoBDSIFileFormat_H__


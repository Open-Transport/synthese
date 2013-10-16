
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

#include "SchedulesBasedService.h"

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
		
			private:
				boost::shared_ptr<const impex::DataSource> _plannedDataSource;
				boost::shared_ptr<const impex::DataSource> _messagesRecipientsDataSource;
				boost::posix_time::time_duration _hysteresis;
	
				mutable std::set<util::RegistryKeyType> _scenariosToRemove;
				mutable std::set<util::RegistryKeyType> _alarmObjectLinksToRemove;
				mutable std::set<util::RegistryKeyType> _messagesToRemove;

				struct Arret
				{
					std::string ref;
					std::string nom;

					pt::StopPoint* syntheseStop;
					pt_operation::Depot* syntheseDepot;
				};

				struct Ligne
				{
					std::string ref;

					pt::CommercialLine* syntheseLine;
				};

				struct ArretChn
				{
					std::string dateRef;
					Arret* arret;
					int pos;
					std::string type;
				};

				struct Chainage
				{
					std::string dateRef;
					std::string nom;
					Ligne* ligne;
					bool sens;
					typedef std::vector<ArretChn> ArretChns;
					ArretChns arretChns;

					typedef std::vector<const pt::JourneyPattern*> SYNTHESEJourneyPatterns;
					SYNTHESEJourneyPatterns syntheseJourneyPatterns;
				};

				struct Horaire
				{
					std::string dateRef;
					boost::posix_time::time_duration htd;
					boost::posix_time::time_duration hta;
					boost::posix_time::time_duration had;
					boost::posix_time::time_duration haa;
					boost::posix_time::time_duration hrd;
					boost::posix_time::time_duration hra;
				};


				struct Course
				{
					std::string dateRef;
					Chainage* chainage;
					typedef std::vector<Horaire> Horaires;
					Horaires horaires;
					bool mapped;

					pt::ScheduledService* syntheseService;

					bool operator!=(const pt::ScheduledService& op) const;
					bool operator==(const pt::ScheduledService& service) const;
					bool operator==(const pt::SchedulesBasedService::Schedules& schedules) const;

					bool mustBeImported() const;
					void updateService(
						const Importer_& importer,
						pt::ScheduledService& service
					) const;
				};

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

				typedef std::map<std::string, Course> Courses;
				typedef std::map<std::string, Ligne> Lignes;
				typedef std::map<std::string, Arret> Arrets;
				typedef std::map<std::string, Chainage> Chainages;
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


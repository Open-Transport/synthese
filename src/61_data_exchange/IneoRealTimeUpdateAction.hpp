
//////////////////////////////////////////////////////////////////////////
/// IneoRealTimeUpdateAction class header.
///	@file IneoRealTimeUpdateAction.hpp
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

#ifndef SYNTHESE_IneoRealtimeUpdateAction_H__
#define SYNTHESE_IneoRealtimeUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
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
		/// 61.15 Action : IneoRealTimeUpdateAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.2-tl
		//////////////////////////////////////////////////////////////////////////
		/// Key : IneoRealTimeUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class IneoRealTimeUpdateAction:
			public util::FactorableTemplate<server::Action, IneoRealTimeUpdateAction>
		{
		public:
			static const std::string PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID;
			static const std::string PARAMETER_PLANNED_DATASOURCE_ID;
			static const std::string PARAMETER_REAL_TIME_DATASOURCE_ID;
			static const std::string PARAMETER_DATABASE;
			static const std::string PARAMETER_TIME_FLOOR;

		private:
			boost::shared_ptr<const impex::DataSource> _plannedDataSource;
			boost::shared_ptr<const impex::DataSource> _realTimeDataSource;
			boost::shared_ptr<const impex::DataSource> _messagesRecipientsDataSource;
			std::string _database;
			// A number of seconds used to floor the real time date
			size_t _timeFloor;

			struct Arret
			{
				int ref;
				std::string nom;

				pt::StopPoint* syntheseStop;
				pt_operation::Depot* syntheseDepot;
			};

			struct Ligne
			{
				int ref;

				pt::CommercialLine* syntheseLine;
			};

			struct ArretChn
			{
				int ref;
				Arret* arret;
				int pos;
				std::string type;
			};

			struct Chainage
			{
				int ref;
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
				int ref;
				boost::posix_time::time_duration htd;
				boost::posix_time::time_duration hta;
				boost::posix_time::time_duration had;
				boost::posix_time::time_duration haa;
				boost::posix_time::time_duration hrd;
				boost::posix_time::time_duration hra;
			};


			struct Course
			{
				int ref;
				Chainage* chainage;
				typedef std::vector<Horaire> Horaires;
				Horaires horaires;
				bool mapped;

				pt::ScheduledService* syntheseService;

				bool operator!=(const pt::ScheduledService& op) const;
				bool operator==(const pt::ScheduledService& service) const;
				bool operator==(const pt::SchedulesBasedService::Schedules& schedules) const;

				bool mustBeImported() const;
				void updateService(pt::ScheduledService& service, size_t timeFloor) const;

				boost::posix_time::time_duration
				_applyTimeFloor(const boost::posix_time::time_duration &td, size_t timeFloor) const;
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

			struct IneoVehicle
			{
				std::string ref;
				bool available;
			};

			typedef std::map<int, Course> Courses;
			typedef std::map<int, Ligne> Lignes;
			typedef std::map<int, Arret> Arrets;
			typedef std::map<int, Chainage> Chainages;
			typedef std::map<int, Programmation> Programmations;
			typedef std::map<int, IneoVehicle> IneoVehicles;


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
			IneoRealTimeUpdateAction();



			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_IneoRealtimeUpdateAction_H__


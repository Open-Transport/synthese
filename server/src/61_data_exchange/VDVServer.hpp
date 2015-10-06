  
/** VDVServer class header.
	@file VDVServer.hpp

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

#ifndef SYNTHESE_cms_VDVClient_hpp__
#define SYNTHESE_cms_VDVClient_hpp__

#include "Object.hpp"

#include "DataSource.h"
#include "NumericField.hpp"
#include "ScheduledService.h"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "PointersVectorField.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace data_exchange
	{
		class VDVServerSubscription;

		FIELD_STRING(ServerAddress)
		FIELD_STRING(ServerPort)
		FIELD_STRING(ClientControlCentreCode)
		FIELD_STRING(ServerControlCentreCode)
		FIELD_STRING(ServiceUrl)
		FIELD_STRING(ServiceCode)
		FIELD_STRING(TracePath)
		FIELD_ID(PlannedDataSourceID)
		FIELD_BOOL(CutAboId)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(ServerAddress),
			FIELD(ServerPort),
			FIELD(ServerControlCentreCode),
			FIELD(ClientControlCentreCode),
			FIELD(ServiceUrl),
			FIELD(ServiceCode),
			FIELD(impex::DataSource),
			FIELD(PlannedDataSourceID),
			FIELD(TracePath),
			FIELD(CutAboId)
		> VDVServerRecord;



		//////////////////////////////////////////////////////////////////////////
		/// VDV 3rd party server.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.5.0
		class VDVServer:
			public Object<VDVServer, VDVServerRecord>
		{
		private:
			static const std::string TAG_SUBSCRIPTION;

		public:
		
			/// Chosen registry class.
			typedef util::Registry<VDVServer>	Registry;

			typedef std::set<VDVServerSubscription*> Subscriptions;

		private:
			Subscriptions _subscriptions;
			mutable boost::posix_time::ptime _startServiceTimeStamp;
			mutable bool _online;

			std::string _getURL( const std::string& request ) const;

			VDVServerSubscription* _getSubscription(std::string aboId) const;
			bool _checkStop(VDVServerSubscription* subscription, std::string stopCode) const;
			pt::ScheduledService* _getService(std::string serviceCode, boost::shared_ptr<impex::DataSource> plannedDataSource) const;
			size_t _getRank(pt::ScheduledService* service, VDVServerSubscription* subscription) const;
			void _updateService(
				pt::ScheduledService* service,
				std::string serviceCode,
				boost::posix_time::time_duration rtDepartureTime,
				boost::posix_time::time_duration rtArrivalTime,
				size_t rank
			) const;

		public:
			VDVServer(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				void addSubscription(VDVServerSubscription* subscription);
				const Subscriptions& getSubscriptions() const { return _subscriptions; }
				void removeSubscription(VDVServerSubscription* subscription);
				void connect() const;
				bool getOnline() const { return _online; }

				void updateSYNTHESEFromServer() const;

				//////////////////////////////////////////////////////////////////////////
				/// Adds parameters that are not intended to be saved (i.e. generated content).
				/// The default implementation adds nothing. This method may be overloaded
				/// @param map the map to populate
				/// @param prefix prefix to add to the keys of the map items
				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			void trace(
				const std::string& tag,
				const std::string& content
			) const;

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_cms_VDVClient_hpp__
